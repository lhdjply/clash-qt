#include "AppContext.h"

#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "config/Factory.h"
#include "config/OverrideConfig.h"
#include "config/ProfileConfig.h"
#include "sys/AutoRun.h"
#include "sys/SysProxy.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>
#include <QTimer>
#include <QUrl>

#include "common/Yaml.h"

namespace clashqt {
namespace {

// 探测本机端口是否真的在监听。系统代理会把整个桌面指向 127.0.0.1:<port>，
// 若那里其实没人监听，结果就是「设置成功但整机断网」——因此设置前必须先探。
bool probeLocalPort(quint16 port, int timeoutMs)
{
    if (port == 0) return false;
    QTcpSocket socket;
    socket.connectToHost(QStringLiteral("127.0.0.1"), port);
    return socket.waitForConnected(timeoutMs > 0 ? timeoutMs : 800);
}

// 订阅下载的超时上限；实际超时取配置里的 subscriptionTimeout
constexpr int kDefaultSubscriptionTimeoutMs = 30000;

// 订阅请求的 User-Agent。
// 机场按 UA 识别客户端：不含 clash 系标识时，多数机场会返回精简配置
// 甚至直接提示「不支持你的代理软件」。对齐 clash-party 的做法
// （profile.ts:570），默认在 UA 中带上 (clash.meta) 标识。
QString subscriptionUserAgent(const QString &itemUserAgent, const QString &appUserAgent)
{
    if (!itemUserAgent.isEmpty()) return itemUserAgent;
    if (!appUserAgent.isEmpty()) return appUserAgent;
    return QStringLiteral("clash-qt/v%1 (clash.meta)").arg(QStringLiteral(CLASHQT_VERSION));
}

// 读取本地配置文件并校验是合法的 Clash 配置。避免把坏内容写进订阅库后
// 污染内核配置（本地导入与「编辑时替换文件」共用）。
bool readValidatedProfile(const QString &filePath, QString *content, QString *error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) {
            *error = QStringLiteral("无法读取文件 %1：%2").arg(filePath, file.errorString());
        }
        return false;
    }
    const QString text = QString::fromUtf8(file.readAll());
    file.close();

    QString    parseError;
    const auto parsed = core::RawConfig::fromYaml(text, &parseError);
    if (!parsed) {
        if (error) *error = QStringLiteral("不是合法的 Clash 配置：%1").arg(parseError);
        return false;
    }

    if (content) *content = text;
    return true;
}

} // namespace

AppContext::AppContext(QObject *parent)
    : QObject(parent),
      m_appConfig(new AppConfig(this)),
      m_controledConfig(new ControledKernelConfig(m_appConfig, this)),
      m_profileConfig(new ProfileConfig(this)),
      m_overrideConfig(new OverrideConfig(this)),
      m_hub(new core::Hub(this)),
      m_sysProxy(new SysProxy(this)),
      m_autoRun(new AutoRun(this))
{
    m_factory = std::make_unique<Factory>(m_appConfig, m_controledConfig, m_profileConfig,
                                          m_overrideConfig);

    // 内核日志转发到应用日志（同时落盘并推给界面）
    QObject::connect(m_hub, &core::Hub::logMessage, this,
                     [](const QString &level, const QString &message) {
                         if (level == QLatin1String("error")) {
                             AppLogger::instance().error(message);
                         } else if (level == QLatin1String("warning")) {
                             AppLogger::instance().warn(message);
                         } else {
                             AppLogger::instance().info(message);
                         }
                     });

    QObject::connect(m_hub, &core::Hub::configChanged, m_controledConfig,
                     [this]() {
                         // 内核运行期改了模式/规则启停时，把最新状态写回受控配置，
                         // 保证下次启动与当前运行状态一致
                         m_controledConfig->patch(
                             QJsonObject{{QStringLiteral("mode"), m_hub->mode()
                                              == core::OutboundMode::Global
                                          ? QStringLiteral("global")
                                          : (m_hub->mode() == core::OutboundMode::Direct
                                                 ? QStringLiteral("direct")
                                                 : QStringLiteral("rule"))}},
                             /*persist=*/true);
                     });
}

AppContext::~AppContext()
{
    if (m_hub) m_hub->stop();
}

bool AppContext::initialize(QString *error)
{
    const bool appOk = m_appConfig->load();
    const bool controledOk = m_controledConfig->load();
    m_profileConfig->load();
    m_overrideConfig->load();

    if (!appOk || !controledOk) {
        const QString message = QStringLiteral("配置加载失败，请检查 %1 目录下的文件是否可写")
                                    .arg(paths::dataDir());
        AppLogger::instance().error(message);
        if (error) *error = message;
        return false;
    }

    // 日志设置随配置生效
    AppLogger::instance().setMaxLogDays(m_appConfig->maxLogDays());
    AppLogger::instance().setMaxFileSizeMB(m_appConfig->maxLogFileSizeMB());
    AppLogger::instance().setAppLogDisabled(m_appConfig->disableAppLog());
    AppLogger::instance().setCoreLogDisabled(m_appConfig->disableCoreLog());

    AppLogger::instance().info(
        QStringLiteral("数据目录：%1（便携模式：%2）")
            .arg(paths::dataDir(), paths::isPortable() ? QStringLiteral("是")
                                                       : QStringLiteral("否")));
    return true;
}

// ---------------------------------------------------------------------------
//  内核
// ---------------------------------------------------------------------------

bool AppContext::applyGeneratedConfig(QString *error)
{
    const Factory::Result result = m_factory->generate();
    if (!result.success) {
        if (error) *error = result.error;
        AppLogger::instance().error(QStringLiteral("配置生成失败：%1").arg(result.error));
        return false;
    }

    QString applyError;
    if (!m_hub->applyConfig(m_factory->runtimeConfig(), &applyError)) {
        if (error) *error = applyError;
        return false;
    }

    // 将应用配置中的「DNS / 嗅探 覆写开关」同步到内核运行状态
    m_hub->setDnsEnabled(m_appConfig->controlDns());
    m_hub->setSnifferEnabled(m_appConfig->controlSniff());

    // 恢复上次的手动选择：适配器是刚重建的，不恢复的话任何一次配置重载
    // （换订阅、改端口、切 DNS 覆写开关）都会把用户选的节点退回列表第一个。
    m_hub->restoreSelectedProxies(m_appConfig->selectedProxies());
    return true;
}

bool AppContext::startKernel(QString *error)
{
    if (m_hub->isRunning()) return true;

    if (!applyGeneratedConfig(error)) return false;

    QString startError;
    if (!m_hub->start(&startError)) {
        if (error) *error = startError;
        AppLogger::instance().error(QStringLiteral("启动失败：%1").arg(startError));
        emit toast(QStringLiteral("error"), QStringLiteral("启动失败：%1").arg(startError));
        return false;
    }

    emit kernelRunningChanged(true);
    return true;
}

void AppContext::stopKernel()
{
    if (!m_hub->isRunning()) return;
    m_hub->stop();
    emit kernelRunningChanged(false);
}

bool AppContext::isKernelRunning() const { return m_hub->isRunning(); }

bool AppContext::reloadKernel(QString *error)
{
    if (!applyGeneratedConfig(error)) return false;

    // 已在运行时需要重新应用监听配置（端口可能变化）
    if (m_hub->isRunning()) {
        m_hub->stop();
        QString startError;
        if (!m_hub->start(&startError)) {
            if (error) *error = startError;
            emit kernelRunningChanged(false);
            return false;
        }
    }
    emit kernelRunningChanged(m_hub->isRunning());
    return true;
}

// ---------------------------------------------------------------------------
//  系统代理
// ---------------------------------------------------------------------------

bool AppContext::setSysProxyEnabled(bool enabled, QString *error)
{
    if (enabled) {
        // 兜底：内核本应常驻运行，此处仅防御性确保监听已就绪。
        // 顺序不可颠倒 —— 系统代理需要 127.0.0.1:<mixed-port> 真的在监听，
        // 没在监听就写系统设置，等于把整个桌面指向一个空端口。
        if (!m_hub->isRunning()) {
            QString startError;
            if (!startKernel(&startError)) {
                const QString message =
                    QStringLiteral("启动失败，未启用系统代理：%1").arg(startError);
                AppLogger::instance().error(message);
                if (error) *error = message;
                emit toast(QStringLiteral("error"), message);
                return false;
            }
        }

        // 系统代理指向本机的混合端口
        const int port = m_controledConfig->mixedPort() > 0 ? m_controledConfig->mixedPort()
                                                            : m_controledConfig->socksPort();
        if (port <= 0) {
            const QString message = QStringLiteral("没有可用的入站端口，无法设置系统代理");
            if (error) *error = message;
            emit toast(QStringLiteral("error"), message);
            return false;
        }

        // 设置前自检：该端口必须真的在监听。否则系统代理会把整个桌面
        // 指向一个空端口，表现为「代理已启用但完全无法上网」。
        if (!probeLocalPort(quint16(port), 800)) {
            const QString message =
                QStringLiteral("本机 127.0.0.1:%1 未在监听，已拒绝启用系统代理"
                               "（否则整个桌面会因指向空端口而断网）")
                    .arg(port);
            AppLogger::instance().error(message);
            if (error) *error = message;
            emit toast(QStringLiteral("error"), message);
            return false;
        }
        QString proxyError;
        if (!m_sysProxy->enable(QStringLiteral("127.0.0.1"), port, m_appConfig->sysProxyBypass(),
                                &proxyError)) {
            if (error) *error = proxyError;
            emit toast(QStringLiteral("error"),
                       QStringLiteral("设置系统代理失败：%1").arg(proxyError));
            return false;
        }
    } else {
        QString proxyError;
        if (!m_sysProxy->disable(&proxyError)) {
            if (error) *error = proxyError;
            emit toast(QStringLiteral("error"),
                       QStringLiteral("关闭系统代理失败：%1").arg(proxyError));
            return false;
        }
    }

    m_appConfig->setSysProxyEnabled(enabled);
    m_appConfig->save();
    // 通知界面刷新
    m_appConfig->notifyChanged();
    return true;
}

bool AppContext::isSysProxyEnabled() const { return m_sysProxy->isEnabled(); }

// ---------------------------------------------------------------------------
//  出站模式
// ---------------------------------------------------------------------------

void AppContext::saveProxySelections()
{
    if (!m_hub || !m_appConfig) return;
    m_appConfig->setSelectedProxies(m_hub->selectedProxies());
}

bool AppContext::setOutboundMode(const QString &mode, QString *error)
{
    const QString normalized = mode.trimmed().toLower();
    core::OutboundMode target = core::OutboundMode::Rule;
    if (normalized == QLatin1String("global")) {
        target = core::OutboundMode::Global;
    } else if (normalized == QLatin1String("direct")) {
        target = core::OutboundMode::Direct;
    } else if (normalized != QLatin1String("rule")) {
        if (error) *error = QStringLiteral("未知的出站模式：%1").arg(mode);
        return false;
    }

    m_hub->setMode(target);
    m_controledConfig->setMode(normalized);
    m_controledConfig->save();
    m_appConfig->setMode(normalized);
    m_appConfig->save();
    return true;
}

QString AppContext::outboundMode() const
{
    switch (m_hub->mode()) {
    case core::OutboundMode::Global: return QStringLiteral("global");
    case core::OutboundMode::Direct: return QStringLiteral("direct");
    case core::OutboundMode::Rule:   break;
    }
    return QStringLiteral("rule");
}

// ---------------------------------------------------------------------------
//  订阅
// ---------------------------------------------------------------------------

bool AppContext::addLocalProfile(const QString &filePath, const QString &name, QString *error)
{
    QString content;
    if (!readValidatedProfile(filePath, &content, error)) return false;

    ProfileItem item;
    item.id   = m_profileConfig->generateId();
    item.name = name.trimmed().isEmpty() ? QFileInfo(filePath).fileName() : name.trimmed();
    item.type = QStringLiteral("local");
    item.file = item.id + QStringLiteral(".yaml");
    item.updated = QDateTime::currentSecsSinceEpoch();

    if (!m_profileConfig->setProfileContent(item.id, content)) {
        if (error) *error = QStringLiteral("订阅内容写入失败");
        return false;
    }

    if (!m_profileConfig->addItem(item, /*switchTo=*/true)) {
        if (error) *error = QStringLiteral("订阅列表写入失败");
        return false;
    }

    emit toast(QStringLiteral("info"), QStringLiteral("已添加本地配置：%1").arg(item.name));
    return reloadKernel(error);
}

bool AppContext::replaceProfileContent(const QString &id, const QString &filePath, QString *error)
{
    const ProfileItem item = m_profileConfig->item(id);
    if (item.id.isEmpty()) {
        if (error) *error = QStringLiteral("订阅不存在：%1").arg(id);
        return false;
    }

    QString content;
    if (!readValidatedProfile(filePath, &content, error)) return false;

    if (!m_profileConfig->setProfileContent(id, content)) {
        if (error) *error = QStringLiteral("订阅内容写入失败");
        return false;
    }
    m_profileConfig->touchUpdated(id);

    emit toast(QStringLiteral("info"),
               QStringLiteral("已替换「%1」的配置文件").arg(item.name));

    // 只有当前生效的订阅才需要重载内核
    if (m_profileConfig->current() != id) return true;
    return reloadKernel(error);
}

bool AppContext::addSubscription(const QString &url, const QString &name, QString *error)
{
    const QUrl parsedUrl(url.trimmed());
    if (!parsedUrl.isValid() || (parsedUrl.scheme() != QLatin1String("http")
                                 && parsedUrl.scheme() != QLatin1String("https"))) {
        if (error) *error = QStringLiteral("订阅地址无效：%1").arg(url);
        return false;
    }

    // 同步下载：这是用户显式触发的单次操作，用局部事件循环等待完成，
    // 超时由配置项 subscriptionTimeout 控制。后续可改为异步并在界面显示进度。
    QNetworkAccessManager manager;
    QNetworkRequest request(parsedUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      subscriptionUserAgent(QString(), m_appConfig->userAgent()));

    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QTimer     timer;
    timer.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    const int timeoutMs = m_appConfig->object().value(QStringLiteral("subscriptionTimeout"))
                              .toInt(kDefaultSubscriptionTimeoutMs);
    timer.start(timeoutMs > 0 ? timeoutMs : kDefaultSubscriptionTimeoutMs);
    loop.exec();

    if (!reply->isFinished()) {
        reply->abort();
        reply->deleteLater();
        if (error) *error = QStringLiteral("订阅下载超时（%1 ms）").arg(timeoutMs);
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString reason = reply->errorString();
        reply->deleteLater();
        if (error) *error = QStringLiteral("订阅下载失败：%1").arg(reason);
        return false;
    }

    const QByteArray payload = reply->readAll();
    const QString    content = QString::fromUtf8(payload);

    // subscription-userinfo 响应头提供流量与到期信息
    std::optional<SubscriptionUserInfo> userInfo;
    const QByteArray header = reply->rawHeader("subscription-userinfo");
    if (!header.isEmpty()) {
        userInfo = ProfileConfig::parseUserInfo(QString::fromLatin1(header));
    }
    reply->deleteLater();

    QString    parseError;
    const auto parsed = core::RawConfig::fromYaml(content, &parseError);
    if (!parsed) {
        if (error) {
            *error = QStringLiteral("订阅内容不是合法的 Clash 配置：%1").arg(parseError);
        }
        return false;
    }

    ProfileItem item;
    item.id   = m_profileConfig->generateId();
    item.name = name.trimmed().isEmpty() ? parsedUrl.host() : name.trimmed();
    item.type = QStringLiteral("remote");
    item.url  = url.trimmed();
    item.file = item.id + QStringLiteral(".yaml");
    item.updated = QDateTime::currentSecsSinceEpoch();
    item.autoUpdate = true;
    item.override   = {};
    if (userInfo) item.extra = userInfo;

    if (!m_profileConfig->setProfileContent(item.id, content)) {
        if (error) *error = QStringLiteral("订阅内容写入失败");
        return false;
    }
    if (!m_profileConfig->addItem(item, /*switchTo=*/true)) {
        if (error) *error = QStringLiteral("订阅列表写入失败");
        return false;
    }

    emit toast(QStringLiteral("info"), QStringLiteral("已添加订阅：%1").arg(item.name));
    return reloadKernel(error);
}

bool AppContext::updateSubscription(const QString &id, QString *error)
{
    const ProfileItem item = m_profileConfig->item(id);
    if (item.id.isEmpty()) {
        if (error) *error = QStringLiteral("订阅不存在：%1").arg(id);
        return false;
    }
    if (item.type != QLatin1String("remote") || item.url.isEmpty()) {
        if (error) *error = QStringLiteral("订阅「%1」不是远程订阅，无需更新").arg(item.name);
        return false;
    }

    QNetworkAccessManager manager;
    QNetworkRequest       request(QUrl(item.url));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      subscriptionUserAgent(item.userAgent, m_appConfig->userAgent()));

    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QTimer     timer;
    timer.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    const int timeoutMs = item.updateTimeout > 0
                              ? item.updateTimeout
                              : m_appConfig->object().value(QStringLiteral("subscriptionTimeout"))
                                    .toInt(kDefaultSubscriptionTimeoutMs);
    timer.start(timeoutMs > 0 ? timeoutMs : kDefaultSubscriptionTimeoutMs);
    loop.exec();

    if (!reply->isFinished()) {
        reply->abort();
        reply->deleteLater();
        if (error) *error = QStringLiteral("更新「%1」超时").arg(item.name);
        return false;
    }
    if (reply->error() != QNetworkReply::NoError) {
        const QString reason = reply->errorString();
        reply->deleteLater();
        if (error) *error = QStringLiteral("更新「%1」失败：%2").arg(item.name, reason);
        return false;
    }

    const QString content = QString::fromUtf8(reply->readAll());

    std::optional<SubscriptionUserInfo> userInfo;
    const QByteArray header = reply->rawHeader("subscription-userinfo");
    if (!header.isEmpty()) {
        userInfo = ProfileConfig::parseUserInfo(QString::fromLatin1(header));
    }
    reply->deleteLater();

    // 内容不合法时保留旧订阅：宁可暂时不更新，也不能把可用配置换成坏配置
    QString    parseError;
    const auto parsed = core::RawConfig::fromYaml(content, &parseError);
    if (!parsed) {
        if (error) {
            *error = QStringLiteral("更新「%1」返回的内容不合法，已保留原配置：%2")
                         .arg(item.name, parseError);
        }
        return false;
    }

    if (!m_profileConfig->setProfileContent(id, content)) {
        if (error) *error = QStringLiteral("订阅内容写入失败");
        return false;
    }

    m_profileConfig->touchUpdated(id);
    if (userInfo) {
        QJsonObject extra;
        extra.insert(QStringLiteral("upload"), double(userInfo->upload));
        extra.insert(QStringLiteral("download"), double(userInfo->download));
        extra.insert(QStringLiteral("total"), double(userInfo->total));
        extra.insert(QStringLiteral("expire"), double(userInfo->expire));
        m_profileConfig->updateItem(id, QJsonObject{{QStringLiteral("extra"), extra}});
    }

    emit toast(QStringLiteral("info"), QStringLiteral("订阅已更新：%1").arg(item.name));

    // 仅当更新的是当前订阅时才需要重载内核
    if (m_profileConfig->current() == id) return reloadKernel(error);
    return true;
}

int AppContext::updateAllSubscriptions(QStringList *errors)
{
    int count = 0;
    for (const ProfileItem &item : m_profileConfig->items()) {
        if (item.type != QLatin1String("remote") || !item.autoUpdate || item.url.isEmpty()) {
            continue;
        }
        QString error;
        if (updateSubscription(item.id, &error)) {
            ++count;
        } else if (errors) {
            *errors << error;
        }
    }
    return count;
}

bool AppContext::switchProfile(const QString &id, QString *error)
{
    if (!m_profileConfig->setCurrent(id)) {
        if (error) *error = QStringLiteral("订阅不存在：%1").arg(id);
        return false;
    }

    const ProfileItem item = m_profileConfig->item(id);
    emit toast(QStringLiteral("info"), QStringLiteral("已切换到订阅：%1").arg(item.name));
    return reloadKernel(error);
}

// ---------------------------------------------------------------------------
//  覆写
// ---------------------------------------------------------------------------

bool AppContext::addYamlOverride(const QString &name, const QString &yaml, bool global,
                                 QString *error)
{
    QString    parseError;
    const auto parsed = core::Yaml::parse(yaml, &parseError);
    if (!parsed || !parsed->isObject()) {
        if (error) {
            *error = QStringLiteral("覆写内容必须是 YAML 映射：%1").arg(parseError);
        }
        return false;
    }

    OverrideItem item;
    item.id     = m_overrideConfig->generateId();
    item.name   = name.trimmed().isEmpty() ? QStringLiteral("未命名覆写") : name.trimmed();
    item.type   = QStringLiteral("local");
    item.ext    = QStringLiteral("yaml");
    item.global = global;
    item.updated = QDateTime::currentSecsSinceEpoch();

    if (!m_overrideConfig->setContent(item.id, QStringLiteral("yaml"), yaml)) {
        if (error) *error = QStringLiteral("覆写内容写入失败");
        return false;
    }
    if (!m_overrideConfig->addItem(item)) {
        if (error) *error = QStringLiteral("覆写列表写入失败");
        return false;
    }

    emit toast(QStringLiteral("info"), QStringLiteral("已添加覆写：%1").arg(item.name));
    return reloadKernel(error);
}

core::RawConfig AppContext::runtimeConfig() const { return m_factory->runtimeConfig(); }

} // namespace clashqt
