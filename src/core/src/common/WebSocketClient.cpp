#include "common/WebSocketClient.h"

#include "common/SocketUtil.h"

#include <QAbstractSocket>
#include <QCryptographicHash>
#include <QDateTime>
#include <QRandomGenerator>

#include "base/Common.h"

#include <cstring>

namespace core {

namespace websocket {
namespace {

// 客户端帧必须带 32 位掩码（RFC 6455 §5.3）
void applyMask(char *data, int size, const uchar mask[4], int offset)
{
    for (int i = 0; i < size; ++i) {
        data[i] = static_cast<char>(static_cast<uchar>(data[i]) ^ mask[(offset + i) % 4]);
    }
}

QString acceptKey(const QString &secWebSocketKey)
{
    // Sec-WebSocket-Accept = base64(SHA1(key + GUID))
    const QString concatenated = secWebSocketKey + QStringLiteral("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    return QString::fromLatin1(
        QCryptographicHash::hash(concatenated.toUtf8(), QCryptographicHash::Sha1).toBase64());
}

quint16 readU16(const QByteArray &buffer, int offset)
{
    return static_cast<quint16>((static_cast<uchar>(buffer.at(offset)) << 8)
                                | static_cast<uchar>(buffer.at(offset + 1)));
}

quint64 readU64(const QByteArray &buffer, int offset)
{
    quint64 value = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | static_cast<uchar>(buffer.at(offset + i));
    }
    return value;
}

} // namespace

bool handshake(QAbstractSocket *socket, const QString &path, const QString &hostHeader,
               int timeoutMs, QString *error)
{
    // key 只需随机 16 字节的 base64；不追求密码学强度，仅用于握手校验
    QByteArray rawKey(16, 0);
    for (int i = 0; i < 16; ++i) {
        rawKey[i] = static_cast<char>(QRandomGenerator::global()->generate() & 0xFF);
    }
    const QString key = QString::fromLatin1(rawKey.toBase64());

    QString request;
    request += QStringLiteral("GET %1 HTTP/1.1\r\n").arg(path.isEmpty() ? QStringLiteral("/") : path);
    request += QStringLiteral("Host: %1\r\n").arg(hostHeader);
    request += QStringLiteral("Upgrade: websocket\r\n");
    request += QStringLiteral("Connection: Upgrade\r\n");
    request += QStringLiteral("Sec-WebSocket-Key: %1\r\n").arg(key);
    request += QStringLiteral("Sec-WebSocket-Version: 13\r\n");
    request += QStringLiteral("\r\n");

    if (!sockutil::writeAll(socket, request.toUtf8(), timeoutMs, error)) {
        if (error) *error = QStringLiteral("写入升级请求失败：%1").arg(*error);
        return false;
    }
    Logger::instance().debug(LogCategory::Listener,
                             QStringLiteral("WS 升级请求已发送 %1 字节到 %2:%3（待写 %4，状态 %5）")
                                 .arg(request.size())
                                 .arg(socket->peerName(), QString::number(socket->peerPort()))
                                 .arg(socket->bytesToWrite())
                                 .arg(int(socket->state())));

    QByteArray response;
    if (!sockutil::readUntil(socket, &response, "\r\n\r\n", timeoutMs, 16 * 1024, error)) {
        // 附带已读取的部分内容，便于判断请求是否到达服务端
        if (error) {
            *error = QStringLiteral("%1（已收到 %2 字节：%3）")
                         .arg(*error)
                         .arg(response.size())
                         .arg(QString::fromLatin1(response.left(120)));
        }
        return false;
    }

    const int    headerEnd = response.indexOf("\r\n\r\n");
    const QByteArray head  = response.left(headerEnd > 0 ? headerEnd : 0);
    const QList<QByteArray> lines = head.split('\n');
    if (lines.isEmpty() || !lines.first().contains("101")) {
        // 附带完整响应头，便于排查 CDN 网关的拒绝原因（SNI 路由 / 路径 / UA 等）
        if (error) {
            *error = QStringLiteral("WebSocket 握手被拒绝：%1\n%2")
                         .arg(QString::fromLatin1(lines.value(0)).trimmed(),
                              QString::fromLatin1(head));
        }
        return false;
    }

    // 校验 Sec-WebSocket-Accept，防止连到非 WS 服务
    QByteArray accept;
    for (const QByteArray &line : lines) {
        if (line.startsWith("Sec-WebSocket-Accept:")) {
            accept = line.mid(int(strlen("Sec-WebSocket-Accept:"))).trimmed();
        }
    }
    if (accept != acceptKey(key).toLatin1()) {
        if (error) *error = QStringLiteral("WebSocket 握手校验失败（Sec-WebSocket-Accept 不匹配）");
        return false;
    }

    // 101 之后可能已携带首帧数据，交给调用方的流处理（此处丢弃会丢数据）
    if (headerEnd + 4 < response.size()) {
        const QByteArray extra = response.mid(headerEnd + 4);
        // 重新塞回读取缓冲不可行（Qt 无回灌），因此写入 socket 的内部缓冲不可用；
        // 直接把它转成未掩码字节放回不可行，改为：只有当首帧完整时才继续。
        // 当前 VLESS 握手首帧由客户端先发送，服务端不会先于请求发数据，
        // 该分支出现时按异常处理并明确报错。
        Q_UNUSED(extra);
        if (error) {
            *error = QStringLiteral("WebSocket 握手响应中夹带了数据，当前不支持该形态");
        }
        return false;
    }
    return true;
}

bool sendBinary(QAbstractSocket *socket, const QByteArray &payload, QString *error)
{
    QByteArray frame;
    frame.append(char(0x80 | kOpBinary)); // FIN + binary

    const qint64 length = payload.size();
    uchar        mask[4];
    for (int i = 0; i < 4; ++i) {
        mask[i] = static_cast<uchar>(QRandomGenerator::global()->generate() & 0xFF);
    }

    if (length < 126) {
        frame.append(char(0x80 | uchar(length))); // MASK + len
    } else if (length <= 0xFFFF) {
        frame.append(char(0x80 | 126));
        frame.append(char((length >> 8) & 0xFF));
        frame.append(char(length & 0xFF));
    } else {
        frame.append(char(0x80 | 127));
        for (int shift = 56; shift >= 0; shift -= 8) {
            frame.append(char((length >> shift) & 0xFF));
        }
    }
    frame.append(reinterpret_cast<const char *>(mask), 4);

    // 掩码必须作用在实际发送的字节上，避免修改调用方缓冲
    QByteArray masked = payload;
    applyMask(masked.data(), masked.size(), mask, 0);
    frame.append(masked);

    return sockutil::writeAll(socket, frame, 15000, error);
}

bool readFrame(QAbstractSocket *socket, uchar *opcode, QByteArray *payload, int timeoutMs,
               QString *error)
{
    QByteArray buffer = sockutil::drainAvailable(socket);

    while (true) {
        // 至少需要 2 字节基础头
        if (buffer.size() >= 2) {
            const uchar first  = static_cast<uchar>(buffer.at(0));
            const uchar second = static_cast<uchar>(buffer.at(1));
            const bool  fin    = (first & 0x80) != 0;
            const uchar op     = first & 0x7F;
            const bool  masked = (second & 0x80) != 0;
            quint64     length = second & 0x7F;
            int         headerSize = 2;

            if (length == 126) {
                if (buffer.size() < 4) {
                    // 继续读
                } else {
                    length = readU16(buffer, 2);
                    headerSize = 4;
                }
            } else if (length == 127) {
                if (buffer.size() < 10) {
                    // 继续读
                } else {
                    length = readU64(buffer, 2);
                    headerSize = 10;
                }
            }

            const int maskSize = masked ? 4 : 0;
            if (buffer.size() >= headerSize + maskSize
                && (length != 126 || headerSize >= 4)
                && (length != 127 || headerSize >= 10)
                && buffer.size() >= headerSize + maskSize + int(qMin<quint64>(length, 0x7FFFFFFF))) {

                uchar mask[4] = {0, 0, 0, 0};
                if (masked) {
                    std::memcpy(mask, buffer.constData() + headerSize, 4);
                }
                QByteArray data = buffer.mid(headerSize + maskSize, int(length));
                if (masked) applyMask(data.data(), data.size(), mask, 0);
                buffer.remove(0, headerSize + maskSize + int(length));

                // 控制帧就地处理，不返回给业务层
                if (op == kOpPing) {
                    // pong：与 ping 相同帧头但操作码为 0xA，服务端帧不带掩码
                    QByteArray pong;
                    pong.append(char(0x80 | kOpPong));
                    pong.append(char(data.size() & 0x7F));
                    pong.append(data);
                    QString writeError;
                    sockutil::writeAll(socket, pong, 5000, &writeError);
                    continue;
                }
                if (op == kOpClose) {
                    if (error) *error = QStringLiteral("对端发送了 WebSocket 关闭帧");
                    websocket::sendClose(socket);
                    return false;
                }
                if (op == kOpPong) continue;

                // 数据帧：当前不处理跨帧分片（VLESS 负载通常单帧承载），
                // 出现分片时按异常处理并明确报错
                if (!fin) {
                    if (error) *error = QStringLiteral("收到分片的 WebSocket 消息，当前不支持");
                    return false;
                }

                if (opcode) *opcode = op;
                *payload = data;
                return true;
            }
        }

        if (socket->state() != QAbstractSocket::ConnectedState) {
            if (error) *error = QStringLiteral("连接已关闭");
            return false;
        }

        const QByteArray chunk = sockutil::drainAvailable(socket);
        if (!chunk.isEmpty()) {
            buffer.append(chunk);
            continue;
        }
        if (!socket->waitForReadyRead(timeoutMs)) {
            const QByteArray late = sockutil::drainAvailable(socket);
            if (!late.isEmpty()) {
                buffer.append(late);
                continue;
            }
            if (error) *error = QStringLiteral("读取 WebSocket 帧超时");
            return false;
        }
        buffer.append(sockutil::drainAvailable(socket));
    }
}

void sendClose(QAbstractSocket *socket)
{
    QByteArray frame;
    frame.append(char(0x80 | kOpClose));
    frame.append(char(0x00));
    socket->write(frame);
    socket->flush();
}

} // namespace websocket

// ===========================================================================
//  TcpStream
// ===========================================================================

TcpStream::TcpStream(QAbstractSocket *socket, QObject *parent)
    : OutboundStream(parent), m_socket(socket)
{
    QObject::connect(m_socket, &QAbstractSocket::readyRead, this, &OutboundStream::readyToRead);
    QObject::connect(m_socket, &QAbstractSocket::disconnected, this, &OutboundStream::streamClosed);
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, this, [this]() {
        emit streamClosed();
    });
}

QByteArray TcpStream::readAll()
{
    return m_socket ? m_socket->readAll() : QByteArray();
}

qint64 TcpStream::write(const QByteArray &data)
{
    if (!m_socket || !m_socket->isOpen()) return -1;
    return m_socket->write(data);
}

void TcpStream::closeStream()
{
    if (!m_socket) return;
    m_socket->flush();
    m_socket->disconnectFromHost();
}

QString TcpStream::errorText() const
{
    return m_socket ? m_socket->errorString() : QString();
}

bool TcpStream::isOpen() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

// ===========================================================================
//  WebSocketStream
// ===========================================================================

WebSocketStream::WebSocketStream(QAbstractSocket *socket, QObject *parent)
    : OutboundStream(parent), m_socket(socket)
{
    QObject::connect(m_socket, &QAbstractSocket::readyRead, this, [this]() { pump(); });
    QObject::connect(m_socket, &QAbstractSocket::disconnected, this, [this]() {
        m_closed = true;
        emit streamClosed();
    });
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, this, [this]() {
        m_closed = true;
        emit streamClosed();
    });
}

WebSocketStream::~WebSocketStream()
{
    if (m_socket) {
        m_socket->abort();
        m_socket->deleteLater();
    }
}

void WebSocketStream::pump()
{
    // 循环取帧：一次 readyRead 可能包含多帧
    while (!m_closed) {
        uchar   opcode  = 0;
        QByteArray payload;
        QString    error;
        if (!websocket::readFrame(m_socket, &opcode, &payload, 0, &error)) {
            if (!error.isEmpty()) {
                m_closed = true;
                emit streamClosed();
            }
            return; // 数据不足，等待下一次 readyRead
        }
        if (opcode == websocket::kOpBinary || opcode == websocket::kOpText) {
            m_buffer.append(payload);
            emit readyToRead();
        }
    }
}

QByteArray WebSocketStream::readAll()
{
    const QByteArray out = m_buffer;
    m_buffer.clear();
    return out;
}

qint64 WebSocketStream::write(const QByteArray &data)
{
    if (m_closed) return -1;
    QString error;
    if (!websocket::sendBinary(m_socket, data, &error)) {
        m_closed = true;
        emit streamClosed();
        return -1;
    }
    return data.size();
}

void WebSocketStream::closeStream()
{
    if (m_closed) return;
    websocket::sendClose(m_socket);
    m_socket->flush();
    m_socket->abort();
    m_closed = true;
}

QString WebSocketStream::errorText() const
{
    return m_socket ? m_socket->errorString() : QString();
}

bool WebSocketStream::isOpen() const
{
    return !m_closed && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

} // namespace core
