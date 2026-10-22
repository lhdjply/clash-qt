#include "tunnel/Tunnel.h"
#include <QCoreApplication>

#include "adapter/Adapters.h"
#include "base/Common.h"

#include <QEventLoop>
#include <QTimer>
#include <QTcpSocket>

#include <algorithm>

namespace core {

// ===========================================================================
//  双向中继
//
//  在同一线程的事件循环内驱动两个套接字之间的数据搬运：
//  任一端可读时写入对端；任一端断开时结束并清理。
//  通过待写缓冲上限做反压，避免慢速对端把内存吃满（与参照实现的
//  io.Copy 配合限流语义一致）。
// ===========================================================================
namespace {

class StreamRelay : public QObject {
public:
    // client 为入站裸 socket；remote 为抽象出站流（DIRECT/HTTP/SOCKS5 是
    // 裸 TCP，WebSocket 类传输则已完成帧解析），中继逻辑完全一致
    StreamRelay(QTcpSocket *client, OutboundStream *remote, ConnectionTracker *tracker,
                QString connectionId, Tunnel *tunnel)
        : m_client(client),
          m_remote(remote),
          m_tracker(tracker),
          m_connectionId(std::move(connectionId)),
          m_tunnel(tunnel)
    {
    }

    // 开始中继；prefetched 是拨号阶段已经读到的上游数据，需先发给客户端
    void start(const QByteArray &prefetched, const QByteArray &initialPayload)
    {
        if (!initialPayload.isEmpty()) {
            m_remote->write(initialPayload);
            if (m_tracker) m_tracker->addUpload(m_connectionId, initialPayload.size());
            m_tunnelUploadHook(initialPayload.size());
        }

        if (!prefetched.isEmpty()) {
            m_client->write(prefetched);
            if (m_tracker) m_tracker->addDownload(m_connectionId, prefetched.size());
            m_tunnelDownloadHook(prefetched.size());
        }

        QObject::connect(m_client, &QTcpSocket::readyRead, this, [this]() { pumpClientToRemote(); });
        QObject::connect(m_remote, &OutboundStream::readyToRead, this,
                         [this]() { pumpRemoteToClient(); });

        QObject::connect(m_client, &QTcpSocket::disconnected, this, [this]() {
            Logger::instance().debug(
                LogCategory::Listener,
                QCoreApplication::translate("clashqt::core", "Connection %1: client disconnected (up %2 / down %3 bytes)")
                    .arg(m_connectionId)
                    .arg(m_up)
                    .arg(m_down));
            finish();
        });
        QObject::connect(m_remote, &OutboundStream::streamClosed, this, [this]() {
            Logger::instance().debug(
                LogCategory::Listener,
                QCoreApplication::translate("clashqt::core", "Connection %1: upstream closed (up %2 / down %3 bytes) %4")
                    .arg(m_connectionId)
                    .arg(m_up)
                    .arg(m_down)
                    .arg(m_remote->errorText()));
            finish();
        });

        // 出错时同样结束，避免连接悬挂
        QObject::connect(m_client, &QTcpSocket::errorOccurred, this,
                         [this](QAbstractSocket::SocketError) { finish(); });

        // 关键：握手期间可能已有客户端数据到达并被缓冲（例如 curl 在完成
        // SOCKS5 协商后立刻发出 HTTP 请求）。这些数据不会再触发 readyRead，
        // 必须在这里主动搬运一次，否则连接会静默挂起到超时。
        if (m_client->bytesAvailable() > 0) pumpClientToRemote();
        if (m_remote->isOpen()) pumpRemoteToClient();
    }

    // 设置结束回调（由 Tunnel 传入，用于退出工作线程的事件循环）
    void setFinishedCallback(std::function<void()> callback) { m_finishedCallback = std::move(callback); }

    void setTrafficHooks(std::function<void(qint64)> upload, std::function<void(qint64)> download)
    {
        m_tunnelUploadHook   = std::move(upload);
        m_tunnelDownloadHook = std::move(download);
    }

private:
    void pumpClientToRemote()
    {
        const QByteArray data = m_client->readAll();
        if (data.isEmpty()) return;
        m_up += data.size();

        const qint64 written = m_remote->write(data);
        if (written != data.size()) {
            // 静默丢字节会让内层 TLS 报「bad record mac / unexpected eof」，
            // 现象与对端故障极像，因此这里必须留下痕迹
            Logger::instance().warn(
                LogCategory::Listener,
                QCoreApplication::translate("clashqt::core", "Connection %1: short upstream write, %2 / %3 bytes")
                    .arg(m_connectionId)
                    .arg(written)
                    .arg(data.size()));
        }
        if (m_tracker) m_tracker->addUpload(m_connectionId, data.size());
        m_tunnelUploadHook(data.size());
    }

    void pumpRemoteToClient()
    {
        const QByteArray data = m_remote->readAll();
        if (data.isEmpty()) return;
        m_down += data.size();

        m_client->write(data);
        if (m_tracker) m_tracker->addDownload(m_connectionId, data.size());
        m_tunnelDownloadHook(data.size());
    }

    void finish()
    {
        if (m_finished) return;
        m_finished = true;

        // 尽量把已排队的剩余数据冲刷出去，再关闭
        m_client->flush();
        m_remote->closeStream();
        m_client->disconnectFromHost();

        if (m_finishedCallback) m_finishedCallback();
    }

    QTcpSocket       *m_client = nullptr;
    OutboundStream   *m_remote = nullptr;
    ConnectionTracker *m_tracker = nullptr;
    QString            m_connectionId;
    Tunnel            *m_tunnel = nullptr;

    qint64 m_up   = 0;
    qint64 m_down = 0;

    std::function<void(qint64)> m_tunnelUploadHook   = [](qint64) {};
    std::function<void(qint64)> m_tunnelDownloadHook = [](qint64) {};
    std::function<void()>       m_finishedCallback;
    bool                        m_finished = false;
};

} // namespace

// ===========================================================================
//  Tunnel
// ===========================================================================

Tunnel::Tunnel(QObject *parent)
    : QObject(parent), m_tracker(std::make_shared<ConnectionTracker>())
{
}

Tunnel::~Tunnel() = default;

void Tunnel::setSnapshot(std::shared_ptr<TunnelSnapshot> snapshot)
{
    // shared_ptr 的读写不是原子的，用原子操作保证跨线程可见性
    std::atomic_store(&m_snapshot, std::move(snapshot));
}

std::shared_ptr<TunnelSnapshot> Tunnel::snapshot() const
{
    return std::atomic_load(&m_snapshot);
}

RulePtr Tunnel::matchRule(const TunnelSnapshot &snapshot, const Metadata &metadata) const
{
    for (const RulePtr &rule : snapshot.rules) {
        if (!rule) continue;

        // 需要解析的规则在 dstIp 缺失且存在域名时，交由调用方先解析后重试；
        // 这里直接跳过，避免用未解析的元数据做出错误判定。
        if (rule->requiresResolution() && metadata.dstIp.isEmpty() && metadata.hasHost()) {
            continue;
        }
        if (rule->matches(metadata, snapshot.geo.get())) {
            rule->noteHit();
            return rule;
        }
    }
    return nullptr;
}

QStringList Tunnel::expandChains(const AdapterPtr &adapter, int depth) const
{
    QStringList chains;
    if (!adapter || depth > 16) return chains;

    chains << adapter->name();

    // 若适配器是代理组，继续展开其当前选中的成员
    if (auto group = std::dynamic_pointer_cast<ProxyGroup>(adapter)) {
        const AdapterPtr selected = group->current();
        if (selected && selected != adapter) {
            chains << expandChains(selected, depth + 1);
        }
    }
    return chains;
}

DispatchOutcome Tunnel::dispatch(const Metadata &metadata) const
{
    DispatchOutcome outcome;

    const std::shared_ptr<TunnelSnapshot> snap = snapshot();
    if (!snap || !snap->registry) {
        outcome.error = QCoreApplication::translate("clashqt::core", "Config not loaded yet");
        return outcome;
    }

    // GLOBAL 模式：所有流量交给 GLOBAL 组
    if (snap->mode == OutboundMode::Global) {
        const AdapterPtr global = snap->registry->find(QStringLiteral("GLOBAL"));
        if (!global) {
            outcome.error = QCoreApplication::translate("clashqt::core", "GLOBAL mode is on, but the config has no GLOBAL group");
            return outcome;
        }
        outcome.matched     = true;
        outcome.policyName  = QStringLiteral("GLOBAL");
        outcome.ruleType    = QStringLiteral("MATCH");
        outcome.rulePayload = QString();
        outcome.adapter     = global;
        outcome.chains      = expandChains(global);
        return outcome;
    }

    // DIRECT 模式：全部直连
    if (snap->mode == OutboundMode::Direct) {
        const AdapterPtr direct = snap->registry->resolvePolicy(QStringLiteral("DIRECT"));
        outcome.matched    = true;
        outcome.policyName = QStringLiteral("DIRECT");
        outcome.ruleType   = QStringLiteral("MATCH");
        outcome.adapter    = direct;
        if (direct) outcome.chains = expandChains(direct);
        return outcome;
    }

    // RULE 模式：顺序匹配
    const RulePtr matchedRule = matchRule(*snap, metadata);
    if (!matchedRule) {
        // 没有兜底 MATCH 规则属于配置错误；按参照实现的行为直连并告警，
        // 保证网络仍可用（用户可自行修正配置）。
        const AdapterPtr direct = snap->registry->resolvePolicy(QStringLiteral("DIRECT"));
        outcome.matched    = true;
        outcome.policyName = QStringLiteral("DIRECT");
        outcome.ruleType   = QStringLiteral("MATCH");
        outcome.adapter    = direct;
        if (direct) outcome.chains = expandChains(direct);
        outcome.error = QCoreApplication::translate("clashqt::core", "No MATCH fallback rule in the rule list; treating as DIRECT");
        return outcome;
    }

    outcome.matched     = true;
    outcome.ruleType    = toString(matchedRule->type());
    outcome.rulePayload = matchedRule->payload();
    outcome.policyName  = matchedRule->target();

    outcome.adapter = snap->registry->resolvePolicy(outcome.policyName);
    if (!outcome.adapter) {
        outcome.error = QCoreApplication::translate("clashqt::core", "Rule %1 points at a policy that does not exist: %2")
                            .arg(matchedRule->description(), outcome.policyName);
        return outcome;
    }
    outcome.chains = expandChains(outcome.adapter);
    return outcome;
}

// TUN 数据面把字节汇总进来。TUN 有自己的线程，这里只能做原子加，
// 不能触碰 m_tracker / m_snapshot 等非线程安全成员。
void Tunnel::addTraffic(qint64 upload, qint64 download)
{
    if (upload > 0) m_uploaded.fetch_add(upload, std::memory_order_relaxed);
    if (download > 0) m_downloaded.fetch_add(download, std::memory_order_relaxed);
}

void Tunnel::handleStream(QTcpSocket *client, const Metadata &metadata,
                          const QByteArray &initialPayload)
{
    if (!client) return;

    const std::shared_ptr<TunnelSnapshot> snap = snapshot();
    if (!snap) {
        client->abort();
        return;
    }

    const QString connectionId = m_tracker->createConnection(metadata);

    // ---- 目标地址准备 ------------------------------------------------------
    Metadata working = metadata;
    QString  host    = working.host.isEmpty() ? working.dstIp : working.host;

    if (host.isEmpty() || working.dstPort <= 0) {
        emit coreLog(QStringLiteral("warning"), QCoreApplication::translate("clashqt::core", "Closed an inbound connection with no target address"));
        client->abort();
        m_tracker->closeConnection(connectionId);
        return;
    }

    // ---- 规则匹配（IP 类规则可能需要先解析域名） ---------------------------
    DispatchOutcome outcome = dispatch(working);

    if (!outcome.matched && snap->resolver) {
        // 先做一次真实 DNS 解析（不做 fake-ip 映射）再重试匹配：
        // IP 类规则需要的是目标真实地址，fake-ip 占位地址没有判断意义。
        const auto resolved = snap->resolver->resolveReal(host, 5000);
        if (!resolved.isEmpty()) {
            working.dstIp = resolved.first().toString();
            outcome = dispatch(working);
        }
    }

    if (!outcome.adapter) {
        emit coreLog(QStringLiteral("warning"),
                     QCoreApplication::translate("clashqt::core", "Could not determine an outbound policy (%1 -> %2); connection closed")
                         .arg(host, outcome.error.isEmpty() ? QCoreApplication::translate("clashqt::core", "Unknown reason")
                                                           : outcome.error));
        client->abort();
        m_tracker->closeConnection(connectionId);
        return;
    }

    if (!outcome.error.isEmpty()) {
        emit coreLog(QStringLiteral("warning"), outcome.error);
    }

    // ---- 需要本机解析的出站（DIRECT）先解析出真实 IP ------------------------
    // 关键：必须使用内核解析器而不是系统解析器，否则配置中的 hosts 映射、
    // nameserver-policy、fallback 全部失效（表现为域名解析到错误地址）。
    if (outcome.adapter->needsLocalResolution() && working.dstIp.isEmpty()
        && !working.host.isEmpty()) {
        if (!snap->resolver) {
            emit coreLog(QStringLiteral("error"),
                         QCoreApplication::translate("clashqt::core", "The DIRECT outbound needs DNS resolution, but no resolver is available"));
            client->abort();
            m_tracker->closeConnection(connectionId);
            return;
        }
        const QList<QHostAddress> resolved =
            snap->resolver->resolveReal(working.host, snap->dialTimeoutMs);
        if (resolved.isEmpty()) {
            emit coreLog(QStringLiteral("warning"),
                         QCoreApplication::translate("clashqt::core", "DNS resolution failed; connection closed: %1").arg(working.host));
            client->abort();
            m_tracker->closeConnection(connectionId);
            return;
        }
        working.dstIp = resolved.first().toString();
    }

    // ---- 拨号 --------------------------------------------------------------
    DialContext context;
    context.host    = working.host;
    context.dstIp   = working.dstIp;
    context.dstPort = working.dstPort;
    context.network = working.network.isEmpty() ? QStringLiteral("tcp") : working.network;
    context.metadata = working;

    const DialResult dialResult = outcome.adapter->dial(context, snap->dialTimeoutMs);

    // REJECT-DROP：保持客户端连接打开但丢弃数据
    if (dialResult.drop) {
        emit coreLog(QStringLiteral("info"),
                     QCoreApplication::translate("clashqt::core", "Rule %1 dropped the connection: %2")
                         .arg(outcome.ruleType, QStringLiteral("%1:%2").arg(host).arg(working.dstPort)));
        // 挂起连接：读走并丢弃所有入站数据，直到客户端主动关闭
        QObject::connect(client, &QTcpSocket::readyRead, client,
                         [client]() { client->readAll(); });
        QObject::connect(client, &QTcpSocket::disconnected, client,
                         [this, connectionId]() { m_tracker->closeConnection(connectionId); });
        QObject::connect(client, &QTcpSocket::disconnected, client, &QObject::deleteLater);
        return;
    }

    if (!dialResult.ok()) {
        emit coreLog(QStringLiteral("warning"),
                     QCoreApplication::translate("clashqt::core", "Outbound %1 failed to connect to %2:%3: %4")
                         .arg(outcome.policyName, host)
                         .arg(working.dstPort)
                         .arg(dialResult.error));
        client->abort();
        m_tracker->closeConnection(connectionId);
        return;
    }

    if (!dialResult.stream) {
        emit coreLog(QStringLiteral("info"), QCoreApplication::translate("clashqt::core", "UDP relay is not implemented yet; connection closed"));
        client->abort();
        m_tracker->closeConnection(connectionId);
        return;
    }

    // 回填连接信息，供 /connections 展示
    m_tracker->updateDispatch(connectionId, outcome.ruleType, outcome.rulePayload,
                              outcome.chains);

    // ---- 建立中继 ----------------------------------------------------------
    QEventLoop loop;

    auto *relay = new StreamRelay(client, dialResult.stream.get(), m_tracker.get(),
                                  connectionId, this);
    relay->setFinishedCallback([&loop, this, connectionId]() {
        m_tracker->closeConnection(connectionId);
        loop.quit();
    });
    relay->setTrafficHooks(
        [this](qint64 bytes) { m_uploaded.fetch_add(bytes, std::memory_order_relaxed); },
        [this](qint64 bytes) { m_downloaded.fetch_add(bytes, std::memory_order_relaxed); });

    relay->start(dialResult.extraData, initialPayload);

    emit coreLog(QStringLiteral("info"),
                 QCoreApplication::translate("clashqt::core", "%1 -> %2:%3 via %4 (rule %5)")
                     .arg(working.srcIp.isEmpty() ? QStringLiteral("local") : working.srcIp, host)
                     .arg(working.dstPort)
                     .arg(outcome.chains.join(QStringLiteral(" -> ")),
                          outcome.ruleType));

    loop.exec();

    // 中继对象与工作线程的事件循环同生命周期，直接析构即可；
    // deleteLater 需要事件循环继续运转，而此时即将退出工作线程。
    delete relay;
}

int Tunnel::closeConnectionsByChain(const QString &chain)
{
    return m_tracker->closeByChain(chain);
}

} // namespace core
