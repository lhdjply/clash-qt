#include "adapter/Hysteria2Adapter.h"
#include <QCoreApplication>

#include "base/Common.h"
#include "common/Hysteria2Client.h"
#include "common/JsonUtil.h"

#include <QDateTime>

#include <algorithm>

namespace core {
namespace {

// 目标地址字符串：hysteria2 使用 "host:port" 形式，IPv6 需加方括号
QString formatTarget(const QString &host, int port)
{
    if (host.contains(QLatin1Char(':')) && !host.startsWith(QLatin1Char('['))) {
        return QStringLiteral("[%1]:%2").arg(host).arg(port);
    }
    return QStringLiteral("%1:%2").arg(host).arg(port);
}

} // namespace

Hysteria2Adapter::Hysteria2Adapter(ProxyConfig config) : m_config(std::move(config))
{
    const QJsonObject &extra = m_config.extra;

    // 密码字段：参照实现用 password，部分订阅写作 auth
    m_password = jsonutil::stringValue(extra, QStringLiteral("password"));
    if (m_password.isEmpty()) {
        m_password = jsonutil::stringValue(extra, QStringLiteral("auth"));
    }

    m_serverName = jsonutil::stringValue(extra, QStringLiteral("sni"));
    if (m_serverName.isEmpty()) m_serverName = m_config.server;

    m_portRange = jsonutil::stringValue(extra, QStringLiteral("ports"));

    m_skipVerify =
        jsonutil::boolValue(extra, QStringLiteral("skip-cert-verify"), false);

    // fingerprint 为证书 SHA-256 的十六进制（64 字符）
    const QString fingerprint =
        jsonutil::stringValue(extra, QStringLiteral("fingerprint"));
    if (!fingerprint.isEmpty()) {
        const QByteArray raw = QByteArray::fromHex(fingerprint.toUtf8());
        if (raw.size() == 32) {
            m_fingerprint = raw;
        } else {
            m_unsupported = QCoreApplication::translate("clashqt::core", "fingerprint is not a valid SHA-256 hex string");
        }
    }

    // Salamander 混淆与 realm 需要额外的报文层 / 服务端协商，尚未实现
    const QString obfs = jsonutil::stringValue(extra, QStringLiteral("obfs"));
    if (!obfs.isEmpty() && m_unsupported.isEmpty()) {
        m_unsupported = QCoreApplication::translate("clashqt::core", "obfs=%1 is set (Salamander obfuscation is not implemented yet)").arg(obfs);
    }
    if (!extra.value(QStringLiteral("realm-opts")).toObject().isEmpty()
        && m_unsupported.isEmpty()) {
        m_unsupported = QCoreApplication::translate("clashqt::core", "realm-opts is set (realm is not implemented yet)");
    }

    if (m_password.isEmpty() && m_unsupported.isEmpty()) {
        m_unsupported = QCoreApplication::translate("clashqt::core", "Missing password");
    }
}

DialResult Hysteria2Adapter::dial(const DialContext &context, int timeoutMs)
{
    DialResult result;

    if (context.network.compare(QLatin1String("udp"), Qt::CaseInsensitive) == 0) {
        // Hysteria2 的 UDP 中继走 QUIC DATAGRAM，而当前 OpenSSL 3.2 未提供该接口。
        // 明确失败，绝不静默降级成 TCP（那会让应用的 UDP 流量变成一条死连接）。
        result.error = QCoreApplication::translate("clashqt::core", "Node %1 does not support UDP relay yet (Hysteria2 needs QUIC DATAGRAM, which OpenSSL 3.2 does not provide)")
                           .arg(m_config.name);
        return result;
    }

    if (!m_unsupported.isEmpty()) {
        result.error = QCoreApplication::translate("clashqt::core", "Node %1 is not supported yet: %2")
                           .arg(m_config.name, m_unsupported);
        return result;
    }

    const QString host =
        context.host.isEmpty() ? context.dstIp : context.host;
    if (host.isEmpty() || context.dstPort <= 0) {
        result.error = QCoreApplication::translate("clashqt::core", "Node %1 has an invalid target address").arg(m_config.name);
        return result;
    }

    Hysteria2Config config;
    config.server      = m_config.server;
    config.port        = quint16(m_config.port);
    config.portRange   = m_portRange;
    config.serverName   = m_serverName;
    config.password    = m_password;
    config.skipVerify  = m_skipVerify;
    config.fingerprint = m_fingerprint;
    config.udp         = m_config.udp;

    // ---- 建立会话（含有限重试）--------------------------------------------
    // hysteria2 走 UDP：链路抖动或丢包会直接让握手/鉴权超时，端口跳跃区间内
    // 也可能个别端口不通。原生客户端遇到这类失败同样会换端口重试，这里在总
    // 超时预算内最多尝试 3 次（每次重新随机选端口）。鉴权被拒、配置缺失这类
    // 确定性失败不重试——重试只会掩盖真正的原因。
    constexpr int kMaxAttempts = 3;
    const qint64  deadline     = QDateTime::currentMSecsSinceEpoch() + timeoutMs;

    std::shared_ptr<Hysteria2Session> session;
    QString                           sessionError;

    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) break;

        // 单次尝试预算：够一次握手 + 鉴权，又不至于让失败一直等到总超时
        const qint64 budget =
            std::min<qint64>(remaining, std::clamp<qint64>(timeoutMs / 3, 2500, 5000));

        bool retryable = false;
        session = Hysteria2Session::create(config, int(budget), &sessionError, &retryable);
        if (session) break;

        if (!retryable || attempt == kMaxAttempts) break;

        Logger::instance().info(
            LogCategory::Adapter,
            QCoreApplication::translate("clashqt::core", "Node %1 failed to connect (attempt %2); retrying on another port: %3")
                .arg(m_config.name)
                .arg(attempt)
                .arg(sessionError));
    }

    if (!session) {
        result.error = QCoreApplication::translate("clashqt::core", "Node %1 failed to connect: %2")
                           .arg(m_config.name, sessionError);
        return result;
    }

    QString streamError;
    auto    stream =
        session->openTcp(formatTarget(host, context.dstPort), timeoutMs, &streamError);
    if (!stream) {
        result.error = QCoreApplication::translate("clashqt::core", "Node %1 failed to establish the tunnel: %2")
                           .arg(m_config.name, streamError);
        return result;
    }

    if (m_config.udp && !session->serverUdpEnabled()) {
        // 仅记录：TCP 不受影响，UDP 请求由中继层按「不支持」处理
        Logger::instance().debug(
            LogCategory::Adapter,
            QCoreApplication::translate("clashqt::core", "Node %1: the server has UDP disabled; UDP traffic will not work")
                .arg(m_config.name));
    }

    result.stream = stream;
    m_alive       = true;
    return result;
}

QJsonObject Hysteria2Adapter::status() const
{
    QJsonObject object = Adapter::status();
    object.insert(QStringLiteral("server"), m_config.server);
    object.insert(QStringLiteral("port"), m_config.port);
    object.insert(QStringLiteral("sni"), m_serverName);
    if (!m_portRange.isEmpty()) {
        object.insert(QStringLiteral("ports"), m_portRange);
    }
    object.insert(QStringLiteral("udp"), false); // QUIC DATAGRAM 未实现
    if (!m_unsupported.isEmpty()) {
        object.insert(QStringLiteral("unsupported"), m_unsupported);
    }
    return object;
}

} // namespace core
