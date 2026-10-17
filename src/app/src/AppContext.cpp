#include "AppContext.h"

#include "sys/TunController.h"

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

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
    // 注意 debug 必须走 AppLogger::debug：它受日志级别控制（默认 Info 级直接丢弃）。
    // 以前这里把 debug 也当 info 写盘，于是 TUN 的「每个 UDP 包一条」「每次关连接
    // 一条」这类逐包日志全被落盘 —— 开 TUN 一小时能刷出几十万行、几 MB 文件，
    // 而且每条都要 open/append/close 一次，纯属浪费 IO。
    QObject::connect(m_hub, &core::Hub::logMessage, this,
                     [](const QString &level, const QString &message) {
                         if (level == QLatin1String("error")) {
                             AppLogger::instance().error(message);
                         } else if (level == QLatin1String("warning")) {
                             AppLogger::instance().warn(message);
                         } else if (level == QLatin1String("debug")) {
                             AppLogger::instance().debug(message);
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

    // 退出前务必收起 TUN：否则会留下一条指到 TUN 的默认路由（等于断网）
    stopTunInternal();

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


// ---------------------------------------------------------------------------
//  TUN 模式
//
//  权限模型与参照实现一致（clash-party/src/main/core/permissions.ts）：
//  特权辅助程序 clash-qt-tun 用 pkexec 做一次 chown root:root + chmod +sx，
//  之后主程序以普通用户身份拉起它，由它建 TUN、配地址、接管路由，
//  再把 TUN 的 fd 通过 SCM_RIGHTS 交回来 —— 数据面仍在主程序里。
// ---------------------------------------------------------------------------
QStringList AppContext::proxyServerIps() const
{
    QStringList ips;
    QStringList resolvedHosts; // 已解析过的服务器域名（去重 + 封顶用）
    if (!m_hub) return ips;

    const QJsonArray proxies = m_hub->config().proxies();
    for (const QJsonValue &value : proxies) {
        if (!value.isObject()) continue;
        const QString server = value.toObject().value(QStringLiteral("server")).toString();
        if (server.isEmpty()) continue;

        QHostAddress literal;
        if (literal.setAddress(server)) {
            // 回环地址不需要旁路（local 表优先级更高），加进去反而可能干扰本机服务
            if (!literal.isLoopback() && ips.size() < 32) ips.append(server);
        }
        // 域名形式的服务器：**必须**在 TUN 接管路由之前解析出真实 IP 并旁路，
        // 否则连节点这一步会被导进 TUN 自己（节点域名被分配假 IP → 连回本机）。
        // 为控制耗时，每个域名最多等 1.5 秒，且总数封顶。
        if (resolvedHosts.size() < 8 && ips.size() < 32 && m_hub && m_hub->resolver()) {
            // 订阅里上百个节点往往只有几个不同的服务器域名：先去重并封顶，
            // 避免开启 TUN 时在 GUI 线程里逐个解析把界面卡住。
            const QString key = server.toLower();
            if (resolvedHosts.contains(key)) continue;
            resolvedHosts.append(key);
            const QList<QHostAddress> resolved = m_hub->resolver()->resolveReal(server, 1500);
            for (const QHostAddress &address : resolved) {
                if (address.protocol() != QAbstractSocket::IPv4Protocol) continue;
                if (ips.size() >= 32) break;
                ips.append(address.toString());
            }
        }
    }
    ips.removeDuplicates();
    return ips;
}

void AppContext::stopTunInternal()
{
    m_tunStarting = false;
    if (m_hub) m_hub->stopTun();
    // 注意：这里**不能** m_tun.reset()。stopTunInternal 会被 TunController
    // 的 failed() 信号回调调用，那是在 TunController 自己的成员函数栈里，
    // 析构掉它就等于在栈上对象还活着时释放自己 —— use-after-free。
    // 控制器很轻，整个进程生命周期内只创建一次、复用即可。
    if (m_tun) m_tun->stop();
    m_tunRunning = false;
}

bool AppContext::tunAutoRoute() const
{
    return m_controledConfig ? m_controledConfig->tunAutoRoute() : false;
}

bool AppContext::setTunAutoRoute(bool enabled, QString *error)
{
    if (!m_controledConfig) {
        if (error) *error = QStringLiteral("配置尚未加载");
        return false;
    }
    if (m_controledConfig->tunAutoRoute() == enabled) return true;

    // 已启用时切换路由接管：必须重启 TUN 才生效（helper 在 up 时决定是否改路由）
    const bool wasRunning = m_tunRunning;
    if (wasRunning) {
        stopTunInternal();
        m_controledConfig->setTunAutoRoute(enabled);
        if (!setTunEnabled(true, error)) {
            m_controledConfig->setTunAutoRoute(!enabled); // 回滚配置
            return false;
        }
        return true;
    }

    QJsonObject tun = m_controledConfig->object().value(QStringLiteral("tun")).toObject();
    tun.insert(QStringLiteral("auto-route"), enabled);
    m_controledConfig->setTunAutoRoute(enabled);
    m_controledConfig->patch(QJsonObject{{QStringLiteral("tun"), tun}});
    return true;
}

bool AppContext::setTunEnabled(bool enabled, QString *error)
{
    if (!m_controledConfig) {
        if (error) *error = QStringLiteral("配置尚未加载");
        return false;
    }
    if (enabled == m_tunRunning) return true;
    if (enabled && m_tunStarting) return true; // 正在启用：忽略重复点击（静默）

    // 从这里开始就置位：下面要解析代理服务器地址（可能几十毫秒），
    // 期间任何再次触发的调用都必须被挡住，否则会出现「已拉起辅助程序后
    // 又报 正在启用」并让开关回弹
    const bool previousStarting = m_tunStarting;
    if (enabled) m_tunStarting = true;
    AppLogger::instance().debug(
        QStringLiteral("TUN 启用请求：enabled=%1 running=%2 starting(旧)=%3")
            .arg(enabled ? 1 : 0)
            .arg(m_tunRunning ? 1 : 0)
            .arg(previousStarting ? 1 : 0));

    if (!enabled) {
        stopTunInternal();
        m_controledConfig->setTunEnabled(false);
        m_controledConfig->patch(QJsonObject{
            {QStringLiteral("tun"),
             QJsonObject{{QStringLiteral("enable"), false}}}});
        emit toast(QStringLiteral("info"), QStringLiteral("TUN 已关闭，系统路由已还原"));
        emit tunStateChanged(false);
        return true;
    }

    // ---- 开启：前置条件 ----
    if (!m_tun) m_tun = std::make_unique<TunController>();
    if (!m_tun->helperExists()) {
        if (error) {
            *error = QStringLiteral("找不到 TUN 辅助程序（应与主程序同目录）：%1")
                         .arg(m_tun->helperPath());
        }
        m_tunStarting = false;
        return false;
    }
    // 先清掉上次异常退出可能残留的路由状态（up 内部也会做，这里双保险）
    if (!m_tun->hasPermission()) {
        // 未授权：弹 pkexec 密码框（用户在此时知道要输密码）
        if (!m_tun->grantPermission(error)) {
            m_tunStarting = false;
            return false;
        }
    }

    // ---- 迁移：fake-ip-filter 里的通配 "*" ----
    // 早期默认配置带了 "*"，它会命中所有域名 → 所有域名都跳过 fake-ip：
    //   · 流里没有域名，域名规则全部失效；
    //   · 系统 DNS 返回真实 AAAA，应用改用 IPv6，而 TUN 不支持 IPv6 → 打不开。
    // 这里就地修正一次（改配置 + 重载内核），否则 TUN 永远拿不到域名。
    bool configFixed = false;
    {
        QJsonObject dnsObj = m_controledConfig->object().value(QStringLiteral("dns")).toObject();
        if (!dnsObj.isEmpty()) {
            const QJsonArray filter = dnsObj.value(QStringLiteral("fake-ip-filter")).toArray();
            QJsonArray       cleaned;
            bool             removed = false;
            for (const QJsonValue &value : filter) {
                if (value.toString().trimmed() == QLatin1String("*")) {
                    removed = true;
                    continue;
                }
                cleaned.append(value);
            }
            if (removed) {
                dnsObj.insert(QStringLiteral("fake-ip-filter"), cleaned);
                m_controledConfig->patch(QJsonObject{{QStringLiteral("dns"), dnsObj}});
                configFixed = true;
                AppLogger::instance().warn(QStringLiteral(
                    "已从 fake-ip-filter 移除通配 \"*\"（它会让所有域名都跳过 fake-ip）"));
                emit toast(QStringLiteral("info"),
                           QStringLiteral("已修正 DNS 配置：fake-ip-filter 里的 \"*\" 会导致"
                                          "所有域名都不走 fake-ip，已移除"));
            }
        }
    }

    // 迁移：设备名沿用参照实现的 "Mihomo"，改成本程序自己的名字
    {
        QJsonObject tunObj = m_controledConfig->object().value(QStringLiteral("tun")).toObject();
        if (!tunObj.isEmpty()
            && tunObj.value(QStringLiteral("device")).toString() == QLatin1String("Mihomo")) {
            tunObj.insert(QStringLiteral("device"), QStringLiteral("clash-qt-tun"));
            m_controledConfig->patch(QJsonObject{{QStringLiteral("tun"), tunObj}});
            configFixed = true;
            AppLogger::instance().info(
                QStringLiteral("TUN 设备名已从 Mihomo 改为 clash-qt-tun"));
        }
    }

    // TUN 的硬前置：内核 DNS 必须开启（fake-ip）。
    // 关掉 DnsServer 的后果是：① 没有域名，域名规则全部失效，流量走错节点；
    // ② 应用拿到真实 AAAA 记录后改用 IPv6，而 TUN 层不支持 IPv6（包被丢弃），
    //    表现为 Google/YouTube 这类双栈站点直接超时打不开。
    if (!m_hub->dnsEnabled()) {
        m_appConfig->setControlDns(true);
        m_appConfig->save();
        m_hub->setDnsEnabled(true);
        AppLogger::instance().warn(
            QStringLiteral("TUN 模式需要内核 DNS（fake-ip），已自动开启 DNS 覆写"));
        emit toast(QStringLiteral("info"),
                   QStringLiteral("TUN 模式需要内核 DNS，已自动开启「DNS 覆写」"));
    }

    // 配置刚被修正且内核已在运行：必须重载，否则 DnsServer 还用着旧过滤器
    if (configFixed && m_hub->isRunning()) {
        if (!reloadKernel(error)) {
            m_tunStarting = false;
            return false;
        }
    }

    // TUN 之下要跑规则与 DNS，内核必须在运行
    if (!m_hub->isRunning()) {
        if (!startKernel(error)) {
            m_tunStarting = false;
            if (error && error->isEmpty()) *error = QStringLiteral("内核启动失败，无法启用 TUN");
            return false;
        }
    }

    TunOptions options;
    options.device    = m_controledConfig->tunDevice();
    options.mtu       = m_controledConfig->tunMtu();
    options.autoRoute = m_controledConfig->tunAutoRoute();
    // 说明：TUN 只接管 fake-ip 网段（198.18.0.0/16 → TUN），不改系统默认路由，
    // 因此不再需要「代理服务器 / DNS 上游旁路」那些地址 —— 它们本来就是为了
    // 防止默认路由被改后形成自环。这样也省掉了开启时逐个解析节点域名的耗时。
    options.routeExcludes.clear();


    // ---- 异步启用：ready() 回来后再接 fd 给内核 ----
    if (!m_tunSignalsConnected) {
        QObject::connect(m_tun.get(), &TunController::ready, this,
                         [this]() { finishTunStart(); });
        QObject::connect(m_tun.get(), &TunController::failed, this,
                         [this](const QString &reason) {
                             stopTunInternal();
                             emit toast(QStringLiteral("error"),
                                        QStringLiteral("TUN 启用失败：%1").arg(reason));
                             emit tunStateChanged(false);
                         });
        m_tunSignalsConnected = true;
    }

    m_tunAutoRoutePending = options.autoRoute;

    if (m_tun->isPending() || m_tun->isRunning()) {
        // 已在启用中：重复请求静默忽略，不要弹错误把开关打回
        return true;
    }
    if (!m_tun->start(options, error)) {
        m_tunStarting = false;
        return false;
    }

    // 看门狗：无论底层卡在哪个环节，25 秒后一定把状态收回来，
    // 否则界面会永远停在「启用中」——既关不掉、也再打不开。
    QTimer::singleShot(25000, this, [this]() {
        if (!m_tunStarting) return;
        AppLogger::instance().error(QStringLiteral("TUN 启用超时，已强制复位"));
        m_tunStarting = false;
        stopTunInternal();
        emit toast(QStringLiteral("error"), QStringLiteral("TUN 启用超时，已复位，请重试"));
        emit tunStateChanged(false);
    });

    emit toast(QStringLiteral("info"), QStringLiteral("正在启用 TUN…"));
    return true;
}

void AppContext::finishTunStart()
{
    m_tunStarting = false;
    if (!m_tun || !m_hub) return;

    const int fd = m_tun->takeDeviceFd();
    int       dnsUdp = -1;
    int       dnsTcp = -1;
    m_tun->takeDnsFds(&dnsUdp, &dnsTcp);
    QString   startError;
    if (fd < 0 || !m_hub->startTun(fd, dnsUdp, dnsTcp, &startError)) {
        if (fd >= 0) ::close(fd);
        if (dnsUdp >= 0) ::close(dnsUdp);
        if (dnsTcp >= 0) ::close(dnsTcp);
        stopTunInternal();
        emit toast(QStringLiteral("error"),
                   QStringLiteral("TUN 启用失败：%1")
                       .arg(startError.isEmpty() ? QStringLiteral("无法接管设备") : startError));
        emit tunStateChanged(false);
        return;
    }

    m_tunRunning = true;
    m_controledConfig->setTunEnabled(true);
    m_controledConfig->patch(QJsonObject{
        {QStringLiteral("tun"), QJsonObject{{QStringLiteral("enable"), true}}}});

    emit toast(QStringLiteral("info"),
               m_tunAutoRoutePending
                   ? QStringLiteral("TUN 已启用（已接管系统路由）")
                   : QStringLiteral("TUN 已启用（未接管路由）"));
    emit tunStateChanged(true);
}


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
