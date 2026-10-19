#include "hub/Hub.h"

#include "tunnel/TunDevice.h"

#include "adapter/Adapters.h"
#include "base/Common.h"
#include "common/JsonUtil.h"
#include "listener/HttpListener.h"
#include "listener/MixedListener.h"
#include "listener/Socks5Listener.h"
#include "rules/RuleParser.h"

#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QPointer>
#include <QSet>
#include <QThread>

#ifdef Q_OS_UNIX
#include <QSocketNotifier>
#include <QThread>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#endif
#include <QTimer>

#include <algorithm>

namespace core {
namespace {

// 便于第三方面板按版本判断可用接口。

} // namespace

Hub::Hub(QObject *parent) : QObject(parent), m_tunnel(std::make_shared<Tunnel>())
{
    m_registry = std::make_shared<AdapterRegistry>();
    m_resolver = std::make_shared<Resolver>();
    m_dnsServer = std::make_shared<DnsServer>(m_resolver);

    // 内核日志汇聚到统一的 Logger，再经信号对外广播
    QObject::connect(m_resolver.get(), &Resolver::logMessage, this,
                     [this](const QString &level, const QString &message) {
                         emit logMessage(level, message);
                     });
    QObject::connect(m_dnsServer.get(), &DnsServer::logMessage, this,
                     [this](const QString &level, const QString &message) {
                         emit logMessage(level, message);
                     });
    QObject::connect(m_tunnel.get(), &Tunnel::coreLog, this,
                     [this](const QString &level, const QString &message) {
                         emit logMessage(level, message);
                     });

    m_statsTimer = new QTimer(this);
    m_statsTimer->setInterval(1000);
    QObject::connect(m_statsTimer, &QTimer::timeout, this, &Hub::onStatisticsTick);
}

Hub::~Hub()
{
    stop();
}


RawConfig Hub::config() const { return m_config; }

QStringList Hub::unsupportedNodes() const
{
    return m_registry ? m_registry->unsupportedNodes() : QStringList();
}

// ---------------------------------------------------------------------------
//  配置应用
// ---------------------------------------------------------------------------

bool Hub::applyConfig(const RawConfig &config, QString *error)
{
    const bool wasRunning = m_running;
    stopListeners();
    m_running = false;

    m_config = config;
    m_mode   = config.mode();

    // ---- DNS --------------------------------------------------------------
    if (m_dnsEnabled) {
        m_resolver->applyDnsConfig(config.dns());

        // 代理节点的服务器域名必须走真实解析：TUN 下若被分配假 IP，
        // 连节点就会连回本机 TUN，所有代理流量全断。
        {
            QStringList serverHosts;
            const QJsonArray proxies = config.proxies();
            for (const QJsonValue &value : proxies) {
                if (!value.isObject()) continue;
                const QString server =
                    value.toObject().value(QStringLiteral("server")).toString().trimmed();
                if (!server.isEmpty() && !server.at(0).isDigit()) serverHosts.append(server);
            }
            m_resolver->setServerHosts(serverHosts);
        }
        // 应答侧：按需开 dns.listen（TUN 的 53 劫持不依赖它，直接调 handleQuery）
        m_dnsServer->applyDnsConfig(config.dns());
    } else {
        // DNS 受控关闭：清空自定义 DNS，仅保留 hosts 语义所需的直连解析
        m_resolver->applyDnsConfig(QJsonObject());
        m_dnsServer->applyDnsConfig(QJsonObject());
        Logger::instance().info(LogCategory::Config,
                                QStringLiteral("DNS 覆写已关闭，使用系统解析"));
    }

    // ---- 适配器 -----------------------------------------------------------
    // 每次都建一份**新的**注册表再挂到快照上，而不是原地 loadFromConfig。
    // 原因：loadFromConfig 内部先 clear() 再重建（std::map / vector），而旧快照
    // 里的注册表指针仍被正在拨号的连接线程使用（dispatch → resolvePolicy）。
    // 原地重建 = 那些线程遍历到被清空的容器，随机时刻闪退。换新对象后，
    // 老注册表随最后一个 shared_ptr 自然销毁，正在用它的人不受影响。
    auto registry = std::make_shared<AdapterRegistry>();
    registry->loadFromConfig(config);
    m_registry = registry;

    // ---- 规则集与规则 ------------------------------------------------------
    RuleSetRegistry ruleSets;
    const QJsonObject ruleProviders = config.ruleProviders();
    for (auto it = ruleProviders.constBegin(); it != ruleProviders.constEnd(); ++it) {
        if (!it.value().isObject()) continue;
        const QJsonObject provider = it.value().toObject();

        const QString behaviorText = jsonutil::stringValue(provider, QStringLiteral("behavior"));
        const auto    behavior     = RuleParser::parseBehavior(behaviorText);
        if (!behavior) {
            Logger::instance().warn(
                LogCategory::Rule,
                QStringLiteral("规则集 %1 的 behavior 非法：%2").arg(it.key(), behaviorText));
            continue;
        }

        const QString type = jsonutil::stringValue(provider, QStringLiteral("type"));
        const QString path = jsonutil::stringValue(provider, QStringLiteral("path"));

        // Phase 1 只支持内联 payload 与本地 file 两种来源；
        // http 远程规则集需要下载与缓存机制（后续阶段）。
        QStringList warnings;
        std::shared_ptr<RuleSet> set;
        if (type.compare(QLatin1String("inline"), Qt::CaseInsensitive) == 0) {
            set = RuleParser::parseRuleSet(it.key(), *behavior,
                                           provider.value(QStringLiteral("payload")).toArray(),
                                           &warnings);
        } else if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                const QString content = QString::fromUtf8(file.readAll());
                QJsonArray  lines;
                for (const QString &line : content.split(QLatin1Char('\n'))) {
                    const QString trimmed = line.trimmed();
                    if (!trimmed.isEmpty()) lines.append(trimmed);
                }
                set = RuleParser::parseRuleSet(it.key(), *behavior, lines, &warnings);
            } else {
                warnings << QStringLiteral("无法读取规则集文件：%1").arg(path);
            }
        } else {
            warnings << QStringLiteral("远程规则集（type=http）尚未实现，已跳过");
        }

        for (const QString &warning : warnings) {
            Logger::instance().warn(LogCategory::Rule, warning);
        }
        if (set) ruleSets.registerSet(it.key(), set);
    }

    QStringList ruleWarnings;
    m_rules = RuleParser::parseAll(config.rules(), &ruleSets, &ruleWarnings);
    for (const QString &warning : ruleWarnings) {
        Logger::instance().warn(LogCategory::Rule, warning);
    }

    // ---- 快照 -------------------------------------------------------------
    auto snapshot          = std::make_shared<TunnelSnapshot>();
    snapshot->registry     = m_registry;
    snapshot->resolver     = m_resolver;
    snapshot->rules        = m_rules;
    snapshot->mode         = m_mode;
    snapshot->snifferEnabled = m_snifferEnabled;
    m_tunnel->setSnapshot(snapshot);

    if (wasRunning) {
        Logger::instance().info(LogCategory::Config,
                                QStringLiteral("配置已热更新（不重启入站监听）"));
    }

    emit configChanged();
    if (error) error->clear();
    return true;
}

// ---------------------------------------------------------------------------
//  入站监听
// ---------------------------------------------------------------------------

bool Hub::startListeners(QString *error)
{
    const QString bindAddress = m_config.bindAddress();

    const InboundPorts ports{m_config.mixedPort(), m_config.socksPort(), m_config.httpPort(),
                             m_config.redirPort(), m_config.tproxyPort()};

    // 端口占用检测：同一端口不能同时作为 mixed 与 socks/http 监听
    QSet<int> used;
    const auto claim = [&used](int port, const QString &label) -> bool {
        if (port <= 0) return true;
        if (used.contains(port)) return false;
        used.insert(port);
        Q_UNUSED(label);
        return true;
    };

    // mixed 端口同时提供 HTTP 与 SOCKS5
    if (ports.mixed > 0) {
        claim(ports.mixed, QStringLiteral("mixed"));
        auto listener = std::make_shared<MixedListener>(
            QStringLiteral("MIXED"), m_tunnel);
        QObject::connect(listener.get(), &Listener::logMessage, this,
                         [this](const QString &level, const QString &message) {
                             emit logMessage(level, message);
                         });
        QString listenError;
        if (!listener->start(bindAddress, quint16(ports.mixed))) {
            if (error) {
                *error = QStringLiteral("混合端口 %1 启动失败").arg(ports.mixed);
            }
            return false;
        }
        m_listeners.push_back(listener);
    }

    if (ports.socks > 0 && !used.contains(ports.socks)) {
        claim(ports.socks, QStringLiteral("socks"));
        auto listener = std::make_shared<Socks5Listener>(
            QStringLiteral("SOCKS"), m_tunnel);
        listener->setCredentials(m_config.authentication());
        QObject::connect(listener.get(), &Listener::logMessage, this,
                         [this](const QString &level, const QString &message) {
                             emit logMessage(level, message);
                         });
        if (!listener->start(bindAddress, quint16(ports.socks))) {
            if (error) *error = QStringLiteral("SOCKS5 端口 %1 启动失败").arg(ports.socks);
            return false;
        }
        m_listeners.push_back(listener);
    }

    if (ports.http > 0 && !used.contains(ports.http)) {
        claim(ports.http, QStringLiteral("http"));
        auto listener = std::make_shared<HttpListener>(
            QStringLiteral("HTTP"), m_tunnel);
        QObject::connect(listener.get(), &Listener::logMessage, this,
                         [this](const QString &level, const QString &message) {
                             emit logMessage(level, message);
                         });
        if (!listener->start(bindAddress, quint16(ports.http))) {
            if (error) *error = QStringLiteral("HTTP 端口 %1 启动失败").arg(ports.http);
            return false;
        }
        m_listeners.push_back(listener);
    }

    if (ports.redir > 0 || ports.tproxy > 0) {
        // redir / tproxy 透明代理需要 SO_ORIGINAL_DST / IP_TRANSPARENT 等
        // 内核特性与提权，属于后续阶段；这里明确告警而不是静默忽略。
        emit logMessage(QStringLiteral("warning"),
                        QStringLiteral("redir-port / tproxy-port 透明代理尚未实现（Phase 2），"
                                       "对应端口未监听"));
    }

    if (m_listeners.empty()) {
        if (error) {
            *error = QStringLiteral("配置中没有任何启用的入站端口"
                                    "（mixed-port / socks-port / port 均为 0）");
        }
        return false;
    }
    return true;
}

void Hub::stopListeners()
{
    for (auto &listener : m_listeners) {
        if (listener) listener->stop();
    }
    m_listeners.clear();
}

bool Hub::start(QString *error)
{
    if (m_running) return true;
    if (m_config.isEmpty()) {
        if (error) *error = QStringLiteral("尚未应用任何配置");
        return false;
    }

    if (!startListeners(error)) {
        stopListeners();
        return false;
    }

    m_lastUpload   = m_tunnel->uploadedBytes();
    m_lastDownload = m_tunnel->downloadedBytes();
    m_statsTimer->start();

    m_running = true;
    Logger::instance().info(LogCategory::Core, QStringLiteral("已启动"));
    emit logMessage(QStringLiteral("info"), QStringLiteral("已启动"));
    return true;
}

void Hub::stop()
{
    if (!m_running && m_listeners.empty()) return;

    m_statsTimer->stop();
    stopListeners();

    if (m_tunnel) {
        m_tunnel->closeConnectionsByChain(QString());
        if (m_tunnel->tracker()) m_tunnel->tracker()->closeAll();
    }

    m_running = false;
    Logger::instance().info(LogCategory::Core, QStringLiteral("已停止"));
}

// ---------------------------------------------------------------------------
//  运行期配置
// ---------------------------------------------------------------------------

void Hub::setMode(OutboundMode mode)
{
    m_mode = mode;
    m_config.setMode(mode);

    if (const auto snapshot = m_tunnel->snapshot()) {
        // 快照是不可变的，切换模式需要替换一份新快照
        auto next  = std::make_shared<TunnelSnapshot>(*snapshot);
        next->mode = mode;
        m_tunnel->setSnapshot(next);
    }
    Logger::instance().info(LogCategory::Core,
                            QStringLiteral("出站模式切换为 %1").arg(toString(mode)));
    emit configChanged();
}

// ---------------------------------------------------------------------------
//  TUN 接管：数据面跑在独立工作线程
//
//  fd 由特权辅助程序（build/bin/clash-qt-tun）建好 TUN 后经 SCM_RIGHTS 传来，
//  主程序无需 root 即可读写。这里把它交给 TunDevice，并把它 move 到一个
//  QThread 里：
//
//    工作线程: fd 可读 → TunIpStack::onPackets → packetsReady → 写回 fd
//    工作线程: DNS 应答、规则派发、adapter->dial（这两者内部有局部事件循环）
//    GUI 线程: 只负责开关与日志转发
//
//  早期版本把这一切放在 GUI 线程，结果是界面卡死 + 嵌套事件循环导致
//  onPackets 重入、段错误闪退。线程边界见 TunDevice 的头文件说明。
// ---------------------------------------------------------------------------
#ifdef Q_OS_UNIX
bool Hub::startTun(int deviceFd, int dnsUdpFd, int dnsTcpFd, QString *error)
{
    if (deviceFd < 0) {
        if (error) *error = QStringLiteral("无效的 TUN 设备 fd");
        return false;
    }
    if (m_tunDevice) {
        if (error) *error = QStringLiteral("TUN 已在运行");
        return false;
    }

    // 拨号：在**每个连接自己的线程**里执行（adapter->dial 内部有阻塞握手）
    auto alive  = std::make_shared<bool>(true);
    m_tunAlive  = alive;
    auto dialFn = [this, alive](const DialContext &context) {
        DialResult aborted;
        if (!*alive) {
            aborted.error = QStringLiteral("TUN 已关闭");
            return aborted;
        }
        DialContext working = context;

        DispatchOutcome outcome = m_tunnel->dispatch(working.metadata);
        if (!outcome.adapter) {
            DialResult failure;
            failure.error = outcome.error.isEmpty()
                                ? QStringLiteral("没有匹配到可用出站")
                                : outcome.error;
            return failure;
        }

        // 只落到 MATCH 兜底时，用真实 IP 再匹配一次：这样 GEOIP / 不带 no-resolve
        // 的 IP-CIDR 规则也能生效（TUN 下伪装地址没有判断意义，域名先走域名规则，
        // 命中就不多花这次解析）。
        if (working.dstIp.isEmpty() && !working.host.isEmpty()
            && outcome.ruleType == QStringLiteral("MATCH")) {
            const QList<QHostAddress> resolved =
                m_resolver ? m_resolver->resolveReal(working.host, 5000)
                           : QList<QHostAddress>{};
            if (!resolved.isEmpty()) {
                working.dstIp          = resolved.first().toString();
                working.metadata.dstIp = working.dstIp;
                const DispatchOutcome retry = m_tunnel->dispatch(working.metadata);
                if (retry.adapter) outcome = retry;
            }
        }

        // DIRECT 类出站需要真实地址。这里必须用**内核解析器**：
        // TUN 模式下系统 DNS 已指向我们自己，交给 Qt/系统解析会拿到 fake-ip，
        // 于是连回本机 TUN —— 自环。resolveReal 明确不走 fake-ip。
        if (outcome.adapter->needsLocalResolution() && working.dstIp.isEmpty()
            && !working.host.isEmpty()) {
            const QList<QHostAddress> resolved =
                m_resolver ? m_resolver->resolveReal(working.host, 5000)
                           : QList<QHostAddress>{};
            if (resolved.isEmpty()) {
                DialResult failure;
                failure.error = QStringLiteral("域名解析失败：%1").arg(working.host);
                return failure;
            }
            working.dstIp           = resolved.first().toString();
            working.metadata.dstIp  = working.dstIp;
        }

        // 5 秒足够（真实代理握手 300~800ms）；原来 8 秒会把数据面堵更久
        return outcome.adapter->dial(working, 5000);
    };

    m_tunThread = new QThread(this);
    m_tunThread->setObjectName(QStringLiteral(CLASHQT_TUN_HELPER_NAME));
    m_tunDevice = new TunDevice(dialFn, m_resolver, m_dnsServer); // 无 parent，随后 moveToThread

    // TUN 的字节由协议栈自己收发（FlowWorker 直接拨号），不经过
    // Tunnel::handleStream 的 StreamRelay，所以统计钩子不会触发。这里把
    // 数据面的计数接进 Tunnel 的全局累计，界面速度才会有数。
    // 捕获 shared_ptr 而不是 this：数据面线程可能比 Hub 收尾得更晚。
    m_tunDevice->setTrafficSink([tunnel = m_tunnel](qint64 upload, qint64 download) {
        tunnel->addTraffic(upload, download);
    });

    m_tunDevice->moveToThread(m_tunThread);

    QObject::connect(m_tunDevice, &TunDevice::logMessage, this,
                     [this](const QString &level, const QString &message) {
                         emit logMessage(level, message);
                     });
    QObject::connect(m_tunDevice, &TunDevice::failed, this, [this](const QString &reason) {
        emit logMessage(QStringLiteral("warning"),
                        QStringLiteral("TUN 数据面异常（%1），已停止 TUN").arg(reason));
        // 在 GUI 线程收尾，避免在设备线程里析构自己
        QMetaObject::invokeMethod(this, [this]() { stopTun(); }, Qt::QueuedConnection);
    });
    QObject::connect(m_tunThread, &QThread::finished, m_tunDevice, &QObject::deleteLater);

    m_tunThread->start();

    TunDevice *device = m_tunDevice;
    QMetaObject::invokeMethod(device, [device, deviceFd, dnsUdpFd, dnsTcpFd]() {
        device->start(deviceFd, dnsUdpFd, dnsTcpFd);
    }, Qt::QueuedConnection);

    m_tunFd = -1; // 所有权已交给 TunDevice（由它关闭）
    Logger::instance().info(LogCategory::Core,
                            QStringLiteral("TUN 已接管（fd=%1），数据面运行在工作线程")
                                .arg(deviceFd));
    emit configChanged();
    return true;
}

void Hub::stopTun()
{
    if (!m_tunDevice) {
        if (m_tunFd >= 0) {
            ::close(m_tunFd);
            m_tunFd = -1;
        }
        return;
    }

    TunDevice *device = m_tunDevice;
    m_tunDevice       = nullptr;

    if (m_tunAlive) *m_tunAlive = false; // 让还在拨号的工作线程立刻退出
    m_tunAlive.reset();

    QThread *thread = m_tunThread;
    m_tunThread     = nullptr;

    if (thread && thread->isRunning()) {
        // 用**排队**调用而不是 BlockingQueued：后者在数据面线程正忙于
        // DNS 解析（内部有局部事件循环）时会把 GUI 线程一起卡住，
        // 那样用户连开关都点不动。TunDevice::shutdown 内部会避开「协议栈
        // 正在执行」的情况，不会在它自己的栈上把自己删掉。
        QMetaObject::invokeMethod(device, &TunDevice::shutdown, Qt::QueuedConnection);
        thread->quit();

        if (thread->wait(800)) {
            thread->deleteLater();
        } else {
            // 线程还卡在阻塞拨号里：绝不能 deleteLater —— 销毁一个仍在运行的
            // QThread 会让 Qt 直接 abort（"Destroyed while thread is still
            // running"），这正是「关闭 TUN 闪退」的第二个原因。
            // 交给它自己退出后回收。
            emit logMessage(QStringLiteral("warning"),
                            QStringLiteral("TUN 数据面线程仍在收尾，退出后自动释放"));
            QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        }
    } else if (thread) {
        thread->deleteLater();
    }

    Logger::instance().info(LogCategory::Core, QStringLiteral("TUN 已关闭"));
    emit configChanged();
}
#else
bool Hub::startTun(int, int, int, QString *error)
{
    if (error) *error = QStringLiteral("当前平台尚未实现 TUN");
    return false;
}
void Hub::stopTun() {}
#endif

void Hub::setDnsEnabled(bool enabled)
{
    if (m_dnsEnabled == enabled) return;
    m_dnsEnabled = enabled;
    // 重新应用 DNS 段：开启时装载配置，关闭时退回系统解析
    const QJsonObject dnsConfig = enabled ? m_config.dns() : QJsonObject();
    m_resolver->applyDnsConfig(dnsConfig);
    m_dnsServer->applyDnsConfig(dnsConfig);
    emit configChanged();
}

void Hub::setSnifferEnabled(bool enabled)
{
    if (m_snifferEnabled == enabled) return;
    m_snifferEnabled = enabled;
    if (const auto snapshot = m_tunnel->snapshot()) {
        auto next = std::make_shared<TunnelSnapshot>(*snapshot);
        next->snifferEnabled = enabled;
        m_tunnel->setSnapshot(next);
    }
    emit configChanged();
}

// ---------------------------------------------------------------------------
//  /proxies
// ---------------------------------------------------------------------------

QJsonObject Hub::proxiesStatus() const
{
    return m_registry ? m_registry->allProxiesStatus() : QJsonObject();
}

bool Hub::selectProxy(const QString &group, const QString &proxy, QString *error)
{
    const AdapterPtr adapter = m_registry->find(group);
    if (!adapter) {
        if (error) *error = QStringLiteral("组 %1 不存在").arg(group);
        return false;
    }
    auto proxyGroup = std::dynamic_pointer_cast<ProxyGroup>(adapter);
    if (!proxyGroup) {
        if (error) *error = QStringLiteral("%1 不是代理组").arg(group);
        return false;
    }
    if (!proxyGroup->select(proxy)) {
        if (error) *error = QStringLiteral("组 %1 中不存在成员 %2").arg(group, proxy);
        return false;
    }
    emit configChanged();
    return true;
}

QJsonObject Hub::selectedProxies() const
{
    QJsonObject result;
    if (!m_registry) return result;

    for (const auto &group : m_registry->groups()) {
        if (!group) continue;
        const QString member = group->userSelectedMember();
        if (!member.isEmpty()) result.insert(group->name(), member);
    }
    return result;
}

int Hub::restoreSelectedProxies(const QJsonObject &selections)
{
    if (!m_registry || selections.isEmpty()) return 0;

    int restored = 0;
    for (const auto &group : m_registry->groups()) {
        if (!group) continue;

        const QString member = jsonutil::stringValue(selections, group->name());
        if (member.isEmpty()) continue; // 该组没有保存过选择
        if (member == group->userSelectedMember()) continue;

        if (!group->select(member)) {
            // 换过订阅后成员可能已不存在：忽略该组并提示，不阻断其他组的恢复
            Logger::instance().warn(
                LogCategory::Adapter,
                QStringLiteral("组 %1 保存的成员 %2 已不存在，已忽略")
                    .arg(group->name(), member));
            continue;
        }
        ++restored;
    }

    if (restored > 0) {
        Logger::instance().info(
            LogCategory::Adapter,
            QStringLiteral("已恢复 %1 个代理组的手动选择").arg(restored));
    }
    return restored;
}

bool Hub::unfixProxy(const QString &group, QString *error)
{
    const AdapterPtr adapter = m_registry->find(group);
    if (!adapter) {
        if (error) *error = QStringLiteral("组 %1 不存在").arg(group);
        return false;
    }
    auto proxyGroup = std::dynamic_pointer_cast<ProxyGroup>(adapter);
    if (!proxyGroup) {
        if (error) *error = QStringLiteral("%1 不是代理组").arg(group);
        return false;
    }
    proxyGroup->unfix();
    emit configChanged();
    return true;
}

QJsonObject Hub::testDelay(const QString &name, const QString &url, int timeoutMs)
{
    QJsonObject result;

    const QString testUrl = url.isEmpty() ? QStringLiteral("https://www.gstatic.com/generate_204")
                                          : url;
    const int effectiveTimeout = timeoutMs > 0 ? timeoutMs : 5000;

    const AdapterPtr adapter = m_registry->resolvePolicy(name);
    if (!adapter) {
        result.insert(QStringLiteral("message"), QStringLiteral("节点不存在：%1").arg(name));
        return result;
    }

    // 通过节点访问测试地址，测量到首个响应字节的耗时
    DialContext context;
    context.host    = testUrl.section(QLatin1Char('/'), 2).section(QLatin1Char(':'), 0, 0);
    context.dstPort = 80;
    context.network = QStringLiteral("tcp");

    const qint64 startMs = QDateTime::currentMSecsSinceEpoch();
    const DialResult dialResult = adapter->dial(context, effectiveTimeout);
    const int elapsed = int(QDateTime::currentMSecsSinceEpoch() - startMs);

    if (!dialResult.ok()) {
        adapter->setAlive(false);
        result.insert(QStringLiteral("message"), dialResult.error);
        return result;
    }

    adapter->recordDelay(elapsed);
    result.insert(QStringLiteral("delay"), elapsed);
    return result;
}

bool Hub::testProxyDelayAsync(const QString &name, const QString &url, int timeoutMs)
{
    // 先用一次同步查找把「节点是否存在」这种参数错误挡在界面线程里，
    // 否则用户点了不存在的节点会毫无反馈
    if (!m_registry || !m_registry->resolvePolicy(name)) return false;

    QPointer<Hub> self(this);

    QThread *thread = QThread::create([self, name, url, timeoutMs]() {
        // 线程启动与对象销毁之间只差一个事件循环，这里再确认一次
        if (!self) return;

        const QJsonObject result = self->testDelay(name, url, timeoutMs);
        const int         delay  = result.value(QStringLiteral("delay")).toInt(-1);
        const QString     error  = result.value(QStringLiteral("message")).toString();

        // 结果必须回到 Hub 所在线程再发：订阅者是界面控件，不能被数据面
        //（这里是测速线程）直接回调
        QMetaObject::invokeMethod(
            self,
            [self, name, delay, error]() {
                if (self) emit self->delayTested(name, delay, error);
            },
            Qt::QueuedConnection);
    });

    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
    return true;
}

void Hub::refreshGroupSelections()
{
    if (!m_registry) return;

    bool any = false;
    for (const auto &group : m_registry->groups()) {
        if (!group) continue;
        group->selectByDelay(); // 内部只对 URLTest / Fallback 且未手动固定的组生效
        any = true;
    }

    // 让界面与托盘重新拉取：节点标记（当前节点）会随选择变化
    if (any) emit configChanged();
}

QJsonObject Hub::testGroupDelay(const QString &group, const QString &url, int timeoutMs)
{
    QJsonObject result;

    const AdapterPtr adapter = m_registry->find(group);
    auto proxyGroup = std::dynamic_pointer_cast<ProxyGroup>(adapter);
    if (!proxyGroup) {
        result.insert(QStringLiteral("message"), QStringLiteral("%1 不是代理组").arg(group));
        return result;
    }

    for (const AdapterPtr &member : proxyGroup->members()) {
        if (!member) continue;
        const QJsonObject single = testDelay(member->name(), url, timeoutMs);
        if (single.contains(QStringLiteral("delay"))) {
            result.insert(member->name(), single.value(QStringLiteral("delay")));
        }
    }
    proxyGroup->selectByDelay();
    emit configChanged();
    return result;
}

// ---------------------------------------------------------------------------
//  /rules
// ---------------------------------------------------------------------------

QJsonObject Hub::rulesStatus() const
{
    QJsonArray rules;
    int        index = 0;

    for (const RulePtr &rule : m_rules) {
        if (!rule) continue;

        QJsonObject item;
        item.insert(QStringLiteral("type"), toString(rule->type()));
        item.insert(QStringLiteral("payload"), rule->payload());
        item.insert(QStringLiteral("proxy"), rule->target());
        item.insert(QStringLiteral("size"), -1);
        item.insert(QStringLiteral("index"), index++);

        QJsonObject extra;
        extra.insert(QStringLiteral("disabled"), rule->disabled());
        extra.insert(QStringLiteral("hitCount"), 0);
        extra.insert(QStringLiteral("hitAt"), QString());
        extra.insert(QStringLiteral("missCount"), 0);
        extra.insert(QStringLiteral("missAt"), QString());
        item.insert(QStringLiteral("extra"), extra);

        rules.append(item);
    }

    QJsonObject result;
    result.insert(QStringLiteral("rules"), rules);
    return result;
}

bool Hub::setRuleDisabled(const QString &ruleText, bool disabled)
{
    for (const RulePtr &rule : m_rules) {
        if (!rule) continue;
        if (rule->description() == ruleText) {
            rule->setDisabled(disabled);
            emit configChanged();
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
//  /connections
// ---------------------------------------------------------------------------

QJsonObject Hub::connectionsStatus() const
{
    QJsonArray connections;
    const auto snapshot = m_tunnel->tracker()->snapshot();
    for (const ConnectionInfo &info : snapshot) {
        connections.append(connectionToJson(info));
    }

    QJsonObject result;
    result.insert(QStringLiteral("downloadTotal"), m_tunnel->tracker()->totalDownload());
    result.insert(QStringLiteral("uploadTotal"), m_tunnel->tracker()->totalUpload());
    result.insert(QStringLiteral("connections"), connections);
    result.insert(QStringLiteral("memory"), memoryInUse());
    return result;
}

bool Hub::closeConnection(const QString &id)
{
    m_tunnel->tracker()->closeConnection(id);
    emit connectionsChanged();
    return true;
}

int Hub::closeAllConnections()
{
    const int count = m_tunnel->tracker()->activeCount();
    m_tunnel->tracker()->closeAll();
    emit connectionsChanged();
    return count;
}

int Hub::closeConnectionsByChain(const QString &chain)
{
    const int count = m_tunnel->closeConnectionsByChain(chain);
    emit connectionsChanged();
    return count;
}

// ---------------------------------------------------------------------------
//  统计
// ---------------------------------------------------------------------------

qint64 Hub::memoryInUse() const
{
    // 内嵌内核与应用同进程，读取自身驻留集作为「内核内存占用」
    QFile file(QStringLiteral("/proc/self/statm"));
    if (!file.open(QIODevice::ReadOnly)) return 0;

    const QList<QByteArray> fields = file.readAll().simplified().split(' ');
    if (fields.size() < 2) return 0;

    bool ok = false;
    const qint64 pages = fields.at(1).toLongLong(&ok);
    if (!ok) return 0;
    return pages * 4096; // 页大小按 Linux 常见值 4KB 计
}

qint64 Hub::uploadBytes() const { return m_tunnel->uploadedBytes(); }
qint64 Hub::downloadBytes() const { return m_tunnel->downloadedBytes(); }

QJsonObject Hub::statistics() const
{
    QJsonObject object;
    object.insert(QStringLiteral("connectionCount"), m_tunnel->tracker()->activeCount());
    object.insert(QStringLiteral("uploadTotal"), m_tunnel->uploadedBytes());
    object.insert(QStringLiteral("downloadTotal"), m_tunnel->downloadedBytes());
    object.insert(QStringLiteral("memoryInUse"), memoryInUse());
    object.insert(QStringLiteral("unsupportedNodeCount"), unsupportedNodes().size());
    return object;
}

void Hub::onStatisticsTick()
{
    const qint64 upload   = m_tunnel->uploadedBytes();
    const qint64 download = m_tunnel->downloadedBytes();

    const qint64 uploadSpeed   = std::max<qint64>(0, upload - m_lastUpload);
    const qint64 downloadSpeed = std::max<qint64>(0, download - m_lastDownload);

    m_lastUpload   = upload;
    m_lastDownload = download;

    emit trafficUpdated(uploadSpeed, downloadSpeed);
    emit memoryUpdated(memoryInUse(), 0);

    if (m_tunnel->tracker()->activeCount() > 0 || uploadSpeed > 0 || downloadSpeed > 0) {
        emit connectionsChanged();
    }
}

} // namespace core
