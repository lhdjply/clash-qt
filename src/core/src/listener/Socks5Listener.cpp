#include "listener/Socks5Listener.h"

#include "base/Common.h"
#include "common/SocketUtil.h"

#include <QDateTime>
#include <QTcpSocket>

namespace core {
namespace {

constexpr int kHandshakeTimeoutMs = 8000;

// SOCKS5 应答码（RFC 1928 §6）
constexpr uchar kSucceeded             = 0x00;
constexpr uchar kGeneralFailure        = 0x01;
constexpr uchar kConnectionNotAllowed  = 0x02;
constexpr uchar kHostUnreachable       = 0x04;
constexpr uchar kCommandNotSupported   = 0x07;
constexpr uchar kAddressNotSupported   = 0x08;

} // namespace

Socks5Listener::Socks5Listener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent)
    : Listener(QString::fromLatin1(kType), std::move(name), std::move(tunnel), parent)
{
}

void Socks5Listener::setCredentials(const QStringList &credentials)
{
    m_credentials = credentials;
}

bool Socks5Listener::writeAll(QTcpSocket *socket, const QByteArray &payload, int timeoutMs)
{
    return sockutil::writeAll(socket, payload, timeoutMs);
}

bool Socks5Listener::verifyCredentials(const QString &username, const QString &password) const
{
    const QString pair = username + QLatin1Char(':') + password;
    for (const QString &credential : m_credentials) {
        if (credential == pair) return true;
    }
    return false;
}

bool Socks5Listener::negotiate(QTcpSocket *client, QString *username, QString *error)
{
    // ---- 方法协商 ---------------------------------------------------------
    // 先消费已缓冲的数据，再等待新数据：curl 等客户端可能把版本协商与
    // 请求连包发出，若直接 waitForReadyRead 会误判为无数据而阻塞。
    QByteArray head = sockutil::drainAvailable(client);
    if (!sockutil::readAtLeast(client, &head, 2, kHandshakeTimeoutMs, error)) return false;

    if (static_cast<uchar>(head.at(0)) != 0x05) {
        if (error) *error = QStringLiteral("不支持的 SOCKS 版本：%1").arg(int(head.at(0)));
        return false;
    }

    const int methodCount = static_cast<uchar>(head.at(1));
    if (!sockutil::readAtLeast(client, &head, 2 + methodCount, kHandshakeTimeoutMs, error)) {
        return false;
    }

    const QByteArray methods = head.mid(2, methodCount);
    const bool requireAuth  = !m_credentials.isEmpty();
    const bool supportsAuth = methods.contains(char(0x02));
    const bool supportsNone = methods.contains(char(0x00));

    if (requireAuth && !supportsAuth) {
        static const QByteArray kNoAcceptable = QByteArray::fromHex("05FF");
        writeAll(client, kNoAcceptable, kHandshakeTimeoutMs);
        if (error) *error = QStringLiteral("客户端不支持用户名/密码认证");
        return false;
    }
    if (!requireAuth && !supportsNone && !supportsAuth) {
        static const QByteArray kNoAcceptable = QByteArray::fromHex("05FF");
        writeAll(client, kNoAcceptable, kHandshakeTimeoutMs);
        if (error) *error = QStringLiteral("客户端未提供可接受的认证方式");
        return false;
    }

    QByteArray selection;
    selection.append(char(0x05));
    selection.append(requireAuth ? char(0x02) : char(0x00));
    if (!writeAll(client, selection, kHandshakeTimeoutMs)) {
        if (error) *error = QStringLiteral("写入 SOCKS5 方法选择失败");
        return false;
    }

    // ---- 用户名/密码认证（RFC 1929） --------------------------------------
    if (!requireAuth) return true;

    QByteArray auth = sockutil::drainAvailable(client);
    if (!sockutil::readAtLeast(client, &auth, 2, kHandshakeTimeoutMs, error)) return false;

    if (static_cast<uchar>(auth.at(0)) != 0x01) {
        if (error) *error = QStringLiteral("不支持的用户名/密码认证版本");
        return false;
    }

    const int userNameLength = static_cast<uchar>(auth.at(1));
    if (!sockutil::readAtLeast(client, &auth, 2 + userNameLength + 1, kHandshakeTimeoutMs,
                               error)) {
        return false;
    }

    const QString name       = QString::fromUtf8(auth.mid(2, userNameLength));
    const int     passLength = static_cast<uchar>(auth.at(2 + userNameLength));
    if (!sockutil::readAtLeast(client, &auth, 2 + userNameLength + 1 + passLength,
                               kHandshakeTimeoutMs, error)) {
        return false;
    }
    const QString password = QString::fromUtf8(auth.mid(2 + userNameLength + 1, passLength));

    const bool granted = verifyCredentials(name, password);
    QByteArray response;
    response.append(char(0x01));
    response.append(granted ? char(0x00) : char(0x01));
    writeAll(client, response, kHandshakeTimeoutMs);

    if (!granted) {
        if (error) *error = QStringLiteral("SOCKS5 认证失败（用户名 %1）").arg(name);
        return false;
    }
    if (username) *username = name;
    return true;
}

bool Socks5Listener::readRequest(QTcpSocket *client, Metadata *metadata, uchar *replyCode,
                                 QString *error)
{
    *replyCode = kGeneralFailure;

    QByteArray head = sockutil::drainAvailable(client);
    // 固定部分：VER(1) CMD(1) RSV(1) ATYP(1)
    if (!sockutil::readAtLeast(client, &head, 4, kHandshakeTimeoutMs, error)) return false;

    if (static_cast<uchar>(head.at(0)) != 0x05) {
        if (error) *error = QStringLiteral("SOCKS5 请求版本错误");
        return false;
    }

    const uchar command     = static_cast<uchar>(head.at(1));
    const uchar addressType = static_cast<uchar>(head.at(3));

    if (command != 0x01) {
        *replyCode = kCommandNotSupported;
        if (error) {
            *error = QStringLiteral("不支持的 SOCKS5 命令 0x%1").arg(QString::number(command, 16));
        }
        return false;
    }

    if (addressType == 0x03) {
        // 域名：1 字节长度 + 域名 + 2 字节端口
        if (!sockutil::readAtLeast(client, &head, 5, kHandshakeTimeoutMs, error)) return false;
        const int addressLength = static_cast<uchar>(head.at(4));
        if (!sockutil::readAtLeast(client, &head, 5 + addressLength + 2, kHandshakeTimeoutMs,
                                   error)) {
            return false;
        }
        metadata->host = QString::fromUtf8(head.mid(5, addressLength));
        metadata->dstPort = (static_cast<uchar>(head.at(5 + addressLength)) << 8)
                            | static_cast<uchar>(head.at(5 + addressLength + 1));
        *replyCode = kSucceeded;
        return true;
    }

    int addressLength = 0;
    if (addressType == 0x01) {
        addressLength = 4;
    } else if (addressType == 0x04) {
        addressLength = 16;
    } else {
        *replyCode = kAddressNotSupported;
        if (error) {
            *error = QStringLiteral("SOCKS5 不支持的地址类型 0x%1")
                         .arg(QString::number(addressType, 16));
        }
        return false;
    }

    if (!sockutil::readAtLeast(client, &head, 4 + addressLength + 2, kHandshakeTimeoutMs, error)) {
        return false;
    }

    if (addressType == 0x01) {
        const quint32 v4 = (quint32(static_cast<uchar>(head.at(4))) << 24)
                           | (quint32(static_cast<uchar>(head.at(5))) << 16)
                           | (quint32(static_cast<uchar>(head.at(6))) << 8)
                           | quint32(static_cast<uchar>(head.at(7)));
        QHostAddress ip;
        ip.setAddress(v4);
        metadata->dstIp = ip.toString();
    } else {
        Q_IPV6ADDR v6{};
        for (int i = 0; i < 16; ++i) v6.c[i] = static_cast<uchar>(head.at(4 + i));
        QHostAddress ip;
        ip.setAddress(v6);
        metadata->dstIp = ip.toString();
    }

    metadata->dstPort = (static_cast<uchar>(head.at(4 + addressLength)) << 8)
                        | static_cast<uchar>(head.at(4 + addressLength + 1));
    *replyCode = kSucceeded;
    return true;
}

void Socks5Listener::handleConnection(QTcpSocket *clientSocket)
{
    QString error;
    QString username;

    if (!negotiate(clientSocket, &username, &error)) {
        Logger::instance().warn(LogCategory::Listener,
                                QStringLiteral("SOCKS5 协商失败：%1").arg(error));
        clientSocket->abort();
        return;
    }

    Metadata metadata;
    metadata.network     = QStringLiteral("tcp");
    metadata.inboundType = QStringLiteral("SOCKS5");
    metadata.inboundName = name();
    metadata.inboundPort = port();
    metadata.inboundUser = username;
    metadata.srcIp       = clientSocket->peerAddress().toString();
    metadata.srcPort     = clientSocket->peerPort();

    uchar replyCode = kGeneralFailure;
    if (!readRequest(clientSocket, &metadata, &replyCode, &error)
        || metadata.dstPort <= 0
        || (metadata.host.isEmpty() && metadata.dstIp.isEmpty())) {
        // 按协议回明确的失败码，避免客户端一直等待
        QByteArray response;
        response.append(char(0x05));
        response.append(char(replyCode));
        response.append(char(0x00));
        response.append(char(0x01));
        response.append(QByteArray::fromHex("00000000")); // BND.ADDR 0.0.0.0
        response.append(char(0x00));
        response.append(char(0x00));                      // BND.PORT 0
        writeAll(clientSocket, response, kHandshakeTimeoutMs);

        Logger::instance().debug(
            LogCategory::Listener,
            QStringLiteral("SOCKS5 请求失败：%1").arg(error.isEmpty() ? QStringLiteral("目标无效")
                                                                     : error));
        clientSocket->abort();
        return;
    }

    // 成功应答：BND.ADDR/BND.PORT 用 0 表示由系统分配
    QByteArray response;
    response.append(char(0x05));
    response.append(char(kSucceeded));
    response.append(char(0x00));
    response.append(char(0x01));
    response.append(QByteArray::fromHex("00000000"));
    response.append(char(0x00));
    response.append(char(0x00));
    if (!writeAll(clientSocket, response, kHandshakeTimeoutMs)) {
        clientSocket->abort();
        return;
    }

    if (tunnel()) tunnel()->handleStream(clientSocket, metadata);
}

} // namespace core
