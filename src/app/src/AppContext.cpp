#include "AppContext.h"
#include <QCoreApplication>

#include "sys/TunController.h"

// 平台差异（close / socket / 非阻塞）统一走这里，避免在 app 层散落 #ifdef
#include "base/Platform.h"

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

#include "common/Yaml.h"

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
    return QStringLiteral(CLASHQT_APP_NAME "/v%1 (clash.meta)")
        .arg(QStringLiteral(CLASHQT_VERSION_TAG));
}

// 读取本地配置文件并校验是合法的 Clash 配置。避免把坏内容写进订阅库后
// 污染内核配置（本地导入与「编辑时替换文件」共用）。
bool readValidatedProfile(const QString &filePath, QString *content, QString *error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::app", "Could not read file %1: %2").arg(filePath, file.errorString());
        }
        return false;
    }
    const QString text = QString::fromUtf8(file.readAll());
    file.close();

    QString    parseError;
    const auto parsed = core::RawConfig::fromYaml(text, &parseError);
    if (!parsed) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Not a valid Clash config: %1").arg(parseError);
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
    // 正常退出时 aboutToQuit 已经调用过；这里是兜底（例如某个分支直接
    // 析构了 context 而没有走事件循环的退出流程）。shutdown() 幂等。
    shutdown();
}

void AppContext::shutdown()
{
    if (m_shutdownDone) return;
    m_shutdownDone = true;

    // 顺序不可颠倒：先撤系统代理，再停内核。反过来的话，从停内核到进程退出
    // 这段时间桌面仍指向一个已经没人监听的本机端口，任何联网操作都会失败。
    disableSysProxyOnExit();

    if (m_hub) m_hub->stop();

    // 退出前务必收起 TUN：否则会留下一条指到 TUN 的默认路由（等于断网）
    stopTunInternal();
}

void AppContext::disableSysProxyOnExit()
{
    if (!m_sysProxy || !m_controledConfig || !m_appConfig) return;
    if (m_sysProxy->backend() == SysProxy::Backend::None) return;

    // 本进程没成功起过内核，桌面上的代理设置就不可能是我们写的
    // （可能属于别的代理工具，或者是用户自己配的企业代理），一律不动。
    if (!m_kernelStarted) return;

    // 只认 http / https / socks 全部指向本机端口的情况。候选端口取当前配置
    // 的三个入站端口 —— 启用后改过端口时，桌面上可能还留着旧值。
    const int candidates[] = {m_controledConfig->mixedPort(),
                              m_controledConfig->socksPort(),
                              m_controledConfig->httpPort()};
    bool ours = false;
    for (int port : candidates) {
        if (port > 0 && m_sysProxy->targets(QStringLiteral("127.0.0.1"), port)) {
            ours = true;
            break;
        }
    }
    if (!ours) return;

    QString error;
    if (!m_sysProxy->disable(&error)) {
        AppLogger::instance().warn(
            tr("Failed to turn off the system proxy on exit: %1 (the desktop still points at the local port)").arg(error));
        return;
    }

    AppLogger::instance().info(
        tr("System proxy turned off before exit, so the desktop stops pointing at a stopped local port"));

    // 注意：**不清** sysProxy.enable。它是「用户希望开着」的记录，下次启动
    // 据此重新把桌面代理指回来（见 restoreStartupToggles）。桌面上的设置
    // 是运行时状态，进程退出后必须撤掉 —— 两者是两回事。
}

bool AppContext::initialize(QString *error)
{
    const bool appOk = m_appConfig->load();
    const bool controledOk = m_controledConfig->load();
    m_profileConfig->load();
    m_overrideConfig->load();

    if (!appOk || !controledOk) {
        const QString message = tr("Failed to load the config. Check that the files in %1 are writable")
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
        tr("Data directory: %1 (portable: %2)")
            .arg(paths::dataDir(), paths::isPortable() ? tr("yes")
                                                       : tr("no")));
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
        AppLogger::instance().error(tr("Config generation failed: %1").arg(result.error));
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
        AppLogger::instance().error(tr("Start failed: %1").arg(startError));
        emit toast(QStringLiteral("error"), tr("Startup failed: %1").arg(startError));
        return false;
    }

    // 退出时据此判断「桌面上的 127.0.0.1:<端口> 是不是我们写的」
    m_kernelStarted = true;
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
        if (error) *error = tr("Config not loaded yet");
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
        if (error) *error = tr("Config not loaded yet");
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
        tr("TUN enable request: enabled=%1 running=%2 starting(old)=%3")
            .arg(enabled ? 1 : 0)
            .arg(m_tunRunning ? 1 : 0)
            .arg(previousStarting ? 1 : 0));

    if (!enabled) {
        stopTunInternal();
        m_controledConfig->setTunEnabled(false);
        m_controledConfig->patch(QJsonObject{
            {QStringLiteral("tun"),
             QJsonObject{{QStringLiteral("enable"), false}}}});
        emit toast(QStringLiteral("info"), tr("TUN disabled; system routes restored"));
        emit tunStateChanged(false);
        return true;
    }

    // ---- 开启：前置条件 ----
    if (!m_tun) m_tun = std::make_unique<TunController>();
#ifndef Q_OS_WIN
    // Linux 才需要「辅助程序 + setuid 授权」这套前置检查；
    // Windows 走 UAC，没有独立辅助程序，由 start() 直接给出可操作的原因。
    if (!m_tun->helperExists()) {
        if (error) {
            *error = tr("TUN helper not found (it should sit next to the main executable): %1")
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
#endif

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
                AppLogger::instance().warn(tr("Removed the wildcard \"*\" from fake-ip-filter (it makes every domain skip fake-ip)"));
                emit toast(QStringLiteral("info"),
                           tr("Fixed the DNS config: the \"*\" in fake-ip-filter made every domain skip fake-ip, so it was removed"));
            }
        }
    }

    // 迁移：设备名沿用参照实现的 "Mihomo"，改成本程序自己的名字
    {
        QJsonObject tunObj = m_controledConfig->object().value(QStringLiteral("tun")).toObject();
        if (!tunObj.isEmpty()
            && tunObj.value(QStringLiteral("device")).toString() == QLatin1String("Mihomo")) {
            tunObj.insert(QStringLiteral("device"), QStringLiteral(CLASHQT_TUN_DEVICE));
            m_controledConfig->patch(QJsonObject{{QStringLiteral("tun"), tunObj}});
            configFixed = true;
            AppLogger::instance().info(
                tr("TUN device name changed from Mihomo to %1")
                    .arg(QStringLiteral(CLASHQT_TUN_DEVICE)));
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
            tr("TUN mode needs kernel DNS (fake-ip); DNS override was enabled automatically"));
        emit toast(QStringLiteral("info"),
                   tr("TUN mode needs kernel DNS; \"DNS override\" was enabled automatically"));
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
            if (error && error->isEmpty()) *error = tr("The kernel failed to start, so TUN cannot be enabled");
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
                                        tr("Failed to enable TUN: %1").arg(reason));
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
        AppLogger::instance().error(tr("Enabling TUN timed out; state was force-reset"));
        m_tunStarting = false;
        stopTunInternal();
        emit toast(QStringLiteral("error"), tr("Timed out enabling TUN; state was reset, please try again"));
        emit tunStateChanged(false);
    });

    emit toast(QStringLiteral("info"), tr("Enabling TUN…"));
    return true;
}

void AppContext::finishTunStart()
{
    m_tunStarting = false;
    if (!m_tun || !m_hub) return;

    const qintptr fd     = m_tun->takeDeviceFd();
    int           dnsUdp = -1;
    int           dnsTcp = -1;
    m_tun->takeDnsFds(&dnsUdp, &dnsTcp);
    QString   startError;
    if (fd < 0 || !m_hub->startTun(fd, dnsUdp, dnsTcp, &startError, m_tun->dnsBindAddress())) {
#ifndef Q_OS_WIN
        // POSIX 的设备所有权到这一步才真正转移，失败要自己关；
        // Windows 传的是适配器 LUID，句柄由 TunController 持有并在 stop() 里关闭
        if (fd >= 0) platform::closeDescriptor(int(fd));
        if (dnsUdp >= 0) platform::closeDescriptor(dnsUdp);
        if (dnsTcp >= 0) platform::closeDescriptor(dnsTcp);
#endif
        stopTunInternal();
        emit toast(QStringLiteral("error"),
                   tr("Failed to enable TUN: %1")
                       .arg(startError.isEmpty() ? tr("Could not take over the device") : startError));
        emit tunStateChanged(false);
        return;
    }

    m_tunRunning = true;
    m_controledConfig->setTunEnabled(true);
    m_controledConfig->patch(QJsonObject{
        {QStringLiteral("tun"), QJsonObject{{QStringLiteral("enable"), true}}}});

    emit toast(QStringLiteral("info"),
               m_tunAutoRoutePending
                   ? tr("TUN enabled (system routes taken over)")
                   : tr("TUN enabled (routes not taken over)"));
    emit tunStateChanged(true);
}


void AppContext::restoreStartupToggles()
{
    if (!m_hub || !m_hub->isRunning()) {
        AppLogger::instance().debug(
            tr("Kernel is not running; skipping switch-state restore"));
        return;
    }

    // ---- TUN ----
    // 退出时只停数据面、不动配置里的 tun.enable，所以这里能如实还原。
    // 未授权时会弹 pkexec 密码框，因此调用点要放在窗口显示之后。
    if (m_controledConfig && m_controledConfig->tunEnabled() && !m_tunRunning) {
        QString tunError;
        if (setTunEnabled(true, &tunError)) {
            AppLogger::instance().info(tr("Restored TUN to its previous state"));
        } else {
            AppLogger::instance().warn(
                tr("Failed to restore TUN: %1").arg(tunError));
            emit toast(QStringLiteral("warning"),
                       tr("Failed to restore TUN mode: %1").arg(tunError));
        }
    }

    // ---- 系统代理 ----
    // 桌面上的代理在退出时被撤掉了，这里按开关记录重新指回本机端口。
    if (m_appConfig && m_appConfig->sysProxyEnabled() && !isSysProxyEnabled()) {
        QString proxyError;
        if (setSysProxyEnabled(true, &proxyError)) {
            AppLogger::instance().info(tr("Restored the system proxy to its previous state"));
        } else {
            AppLogger::instance().warn(
                tr("Failed to restore the system proxy: %1").arg(proxyError));
            emit toast(QStringLiteral("warning"),
                       tr("Failed to restore the system proxy: %1").arg(proxyError));
        }
    }
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
                    tr("Start failed; system proxy not enabled: %1").arg(startError);
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
            const QString message = tr("No inbound port available; cannot set the system proxy");
            if (error) *error = message;
            emit toast(QStringLiteral("error"), message);
            return false;
        }

        // 设置前自检：该端口必须真的在监听。否则系统代理会把整个桌面
        // 指向一个空端口，表现为「代理已启用但完全无法上网」。
        if (!probeLocalPort(quint16(port), 800)) {
            const QString message =
                tr("127.0.0.1:%1 is not listening locally; refusing to enable the system proxy (the whole desktop would lose connectivity by pointing at an empty port)")
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
                       tr("Failed to set the system proxy: %1").arg(proxyError));
            return false;
        }
    } else {
        QString proxyError;
        if (!m_sysProxy->disable(&proxyError)) {
            if (error) *error = proxyError;
            emit toast(QStringLiteral("error"),
                       tr("Failed to turn off the system proxy: %1").arg(proxyError));
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
        if (error) *error = tr("Unknown outbound mode: %1").arg(mode);
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
        if (error) *error = tr("Failed to write the profile contents");
        return false;
    }

    if (!m_profileConfig->addItem(item, /*switchTo=*/true)) {
        if (error) *error = tr("Failed to write the profile list");
        return false;
    }

    emit toast(QStringLiteral("info"), tr("Added local config: %1").arg(item.name));
    return reloadKernel(error);
}

bool AppContext::replaceProfileContent(const QString &id, const QString &filePath, QString *error)
{
    const ProfileItem item = m_profileConfig->item(id);
    if (item.id.isEmpty()) {
        if (error) *error = tr("Profile does not exist: %1").arg(id);
        return false;
    }

    QString content;
    if (!readValidatedProfile(filePath, &content, error)) return false;

    if (!m_profileConfig->setProfileContent(id, content)) {
        if (error) *error = tr("Failed to write the profile contents");
        return false;
    }
    m_profileConfig->touchUpdated(id);

    emit toast(QStringLiteral("info"),
               tr("Replaced the config file of \"%1\"").arg(item.name));

    // 只有当前生效的订阅才需要重载内核
    if (m_profileConfig->current() != id) return true;
    return reloadKernel(error);
}

bool AppContext::addSubscription(const QString &url, const QString &name, QString *error)
{
    const QUrl parsedUrl(url.trimmed());
    if (!parsedUrl.isValid() || (parsedUrl.scheme() != QLatin1String("http")
                                 && parsedUrl.scheme() != QLatin1String("https"))) {
        if (error) *error = tr("Invalid profile URL: %1").arg(url);
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
        if (error) *error = tr("Profile download timed out (%1 ms)").arg(timeoutMs);
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString reason = reply->errorString();
        reply->deleteLater();
        if (error) *error = tr("Profile download failed: %1").arg(reason);
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
            *error = tr("The profile is not a valid Clash config: %1").arg(parseError);
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
        if (error) *error = tr("Failed to write the profile contents");
        return false;
    }
    if (!m_profileConfig->addItem(item, /*switchTo=*/true)) {
        if (error) *error = tr("Failed to write the profile list");
        return false;
    }

    emit toast(QStringLiteral("info"), tr("Added profile: %1").arg(item.name));
    return reloadKernel(error);
}

bool AppContext::updateSubscription(const QString &id, QString *error)
{
    const ProfileItem item = m_profileConfig->item(id);
    if (item.id.isEmpty()) {
        if (error) *error = tr("Profile does not exist: %1").arg(id);
        return false;
    }
    if (item.type != QLatin1String("remote") || item.url.isEmpty()) {
        if (error) *error = tr("Profile \"%1\" is not a remote profile; nothing to update").arg(item.name);
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
        if (error) *error = tr("Updating \"%1\" timed out").arg(item.name);
        return false;
    }
    if (reply->error() != QNetworkReply::NoError) {
        const QString reason = reply->errorString();
        reply->deleteLater();
        if (error) *error = tr("Failed to update \"%1\": %2").arg(item.name, reason);
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
            *error = tr("The update for \"%1\" returned invalid content; the old config was kept: %2")
                         .arg(item.name, parseError);
        }
        return false;
    }

    if (!m_profileConfig->setProfileContent(id, content)) {
        if (error) *error = tr("Failed to write the profile contents");
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

    emit toast(QStringLiteral("info"), tr("Profile updated: %1").arg(item.name));

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
        if (error) *error = tr("Profile does not exist: %1").arg(id);
        return false;
    }

    const ProfileItem item = m_profileConfig->item(id);
    emit toast(QStringLiteral("info"), tr("Switched to profile: %1").arg(item.name));
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
            *error = tr("Override content must be a YAML mapping: %1").arg(parseError);
        }
        return false;
    }

    OverrideItem item;
    item.id     = m_overrideConfig->generateId();
    item.name   = name.trimmed().isEmpty() ? tr("Unnamed override") : name.trimmed();
    item.type   = QStringLiteral("local");
    item.ext    = QStringLiteral("yaml");
    item.global = global;
    item.updated = QDateTime::currentSecsSinceEpoch();

    if (!m_overrideConfig->setContent(item.id, QStringLiteral("yaml"), yaml)) {
        if (error) *error = tr("Failed to write the override contents");
        return false;
    }
    if (!m_overrideConfig->addItem(item)) {
        if (error) *error = tr("Failed to write the override list");
        return false;
    }

    emit toast(QStringLiteral("info"), tr("Added override: %1").arg(item.name));
    return reloadKernel(error);
}

// ---------------------------------------------------------------------------
//  自定义规则覆写
//
//  为什么单独做一层而不是让用户直接改 running config：订阅每次更新都会
//  重写 work/config.yaml，手工改的内容必然丢失。规则覆写按订阅 id 存在
//  rules/<id>.yaml，由 Factory 在每次生成运行配置时重新叠加，订阅更新后
//  依然生效。
//
//  注意 append 的语义：它是「追加到规则列表末尾」。订阅通常以 MATCH 收尾，
//  追加项会落在 MATCH 之后从而永不匹配 —— 这一点在界面上要明确提示，
//  引导用户优先用 prepend。
// ---------------------------------------------------------------------------

AppContext::RuleOverride AppContext::ruleOverride() const
{
    RuleOverride result;
    const QString profileId = m_profileConfig ? m_profileConfig->current() : QString();
    if (profileId.isEmpty()) return result;

    bool          ok = false;
    const QString content = safefile::readText(paths::rulePath(profileId), &ok);
    if (!ok || content.trimmed().isEmpty()) return result;

    QString    parseError;
    const auto parsed = core::Yaml::parse(content, &parseError);
    if (!parsed || !parsed->isObject()) {
        AppLogger::instance().warn(
            tr("Failed to parse the rule-override file; treating it as empty: %1").arg(parseError));
        return result;
    }

    const QJsonObject root = parsed->toObject();
    const auto        readList = [&root](const QString &key) {
        QStringList list;
        for (const QJsonValue &value : root.value(key).toArray()) {
            if (value.isString()) list << value.toString();
        }
        return list;
    };

    result.prepend = readList(QStringLiteral("prepend"));
    result.append  = readList(QStringLiteral("append"));
    result.remove  = readList(QStringLiteral("delete"));
    result.disabled = readList(QStringLiteral("disabled"));
    return result;
}

bool AppContext::saveRuleOverride(const RuleOverride &value, QString *error)
{
    const QString profileId = m_profileConfig ? m_profileConfig->current() : QString();
    if (profileId.isEmpty()) {
        if (error) *error = tr("No profile selected; cannot save custom rules");
        return false;
    }

    // 逐条体检：规则是「A,B,C」形式的单行文本，空行与换行都会让内核解析
    // 出半截规则；这里拦下来并指出第几条，避免用户把错误带进运行配置。
    const auto validate = [error](const QStringList &rules, const QString &label) {
        for (int i = 0; i < rules.size(); ++i) {
            const QString rule = rules.at(i).trimmed();
            if (rule.isEmpty()) continue;
            if (rule.contains(QLatin1Char('\n'))) {
                if (error) {
                    *error = tr("%1 entry %2 contains a line break (each rule must stay on one line)")
                                 .arg(label)
                                 .arg(i + 1);
                }
                return false;
            }
            if (rule.count(QLatin1Char(',')) < 1) {
                if (error) {
                    *error = tr("%1 entry %2 is not a valid rule (expected at least \"type,target\"): %3")
                                 .arg(label)
                                 .arg(i + 1)
                                 .arg(rule);
                }
                return false;
            }
        }
        return true;
    };
    if (!validate(value.prepend, tr("Custom rules"))
        || !validate(value.append, tr("Fallback rules"))
        || !validate(value.remove, tr("Removal rules"))) {
        return false;
    }

    const auto clean = [](const QStringList &rules) {
        QStringList cleaned;
        for (const QString &rule : rules) {
            const QString trimmed = rule.trimmed();
            if (!trimmed.isEmpty()) cleaned << trimmed;
        }
        return cleaned;
    };

    QJsonObject root;
    if (const QStringList list = clean(value.prepend); !list.isEmpty()) {
        root.insert(QStringLiteral("prepend"), QJsonArray::fromStringList(list));
    }
    if (const QStringList list = clean(value.append); !list.isEmpty()) {
        root.insert(QStringLiteral("append"), QJsonArray::fromStringList(list));
    }
    if (const QStringList list = clean(value.remove); !list.isEmpty()) {
        root.insert(QStringLiteral("delete"), QJsonArray::fromStringList(list));
    }
    // 停用列表只对 prepend 有意义：规则本体仍在 prepend 里，应用时按它跳过
    if (const QStringList list = clean(value.disabled); !list.isEmpty()) {
        root.insert(QStringLiteral("disabled"), QJsonArray::fromStringList(list));
    }

    // 全空时写一个空映射：等价于「没有规则覆写」，同时保留文件以便用户
    // 继续编辑（不必区分「文件不存在」和「内容为空」两种状态）
    const QString content = core::Yaml::stringify(QJsonValue(root));
    QString       writeError;
    if (!safefile::writeAtomic(paths::rulePath(profileId), content, &writeError)) {
        if (error) *error = tr("Failed to write custom rules: %1").arg(writeError);
        return false;
    }

    AppLogger::instance().info(
        tr("Custom rules saved (profile %1): %2 total, %3 disabled, %4 fallback / %5 removal")
            .arg(profileId)
            .arg(value.prepend.size())
            .arg(value.disabled.size())
            .arg(value.append.size())
            .arg(value.remove.size()));

    if (!reloadKernel(error)) return false;

    emit toast(QStringLiteral("info"), tr("Custom rules applied"));
    return true;
}

QString AppContext::currentProfileName() const
{
    if (!m_profileConfig) return QString();
    const QString id = m_profileConfig->current();
    if (id.isEmpty()) return QString();
    return m_profileConfig->item(id).name;
}

core::RawConfig AppContext::runtimeConfig() const { return m_factory->runtimeConfig(); }

} // namespace clashqt
