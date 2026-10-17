#include "tunnel/UdpChannel.h"

#include "tunnel/OutboundStream.h"

#include <QAbstractSocket>
#include <QUdpSocket>

namespace core {
namespace {

// UDP 数据报最大载荷：去掉 IP(20)+UDP(8) 头后，标准 MTU 下约 1472 字节；
// 但 QUIC/STUN 的数据报一般不超过 1400。这里只做协议上限校验（65535）。
constexpr int kMaxDatagram = 65535;

} // namespace

// ===========================================================================
//  SocketUdpChannel
// ===========================================================================

SocketUdpChannel::SocketUdpChannel(std::shared_ptr<QUdpSocket> socket, QObject *parent)
    : UdpChannel(parent), m_socket(std::move(socket))
{
    if (!m_socket) {
        m_closed = true;
        return;
    }
    // connected 的 UDP socket：readyRead 表示有回包到达
    QObject::connect(m_socket.get(), &QUdpSocket::readyRead, this, [this]() { drain(); });
}

void SocketUdpChannel::drain()
{
    if (!m_socket || m_closed) return;
    while (m_socket->hasPendingDatagrams()) {
        QByteArray payload;
        payload.resize(int(m_socket->pendingDatagramSize()));
        const qint64 read = m_socket->readDatagram(payload.data(), payload.size());
        if (read < 0) break;
        payload.resize(int(read));
        emit datagramReceived(payload);
    }
}

qint64 SocketUdpChannel::sendDatagram(const QByteArray &data)
{
    if (!m_socket || m_closed || data.size() > kMaxDatagram) return -1;
    // 已 connectToHost 的 socket 直接用 write()，等价于 writeDatagram 到对端
    return m_socket->write(data);
}

void SocketUdpChannel::closeChannel()
{
    if (m_closed) return;
    m_closed = true;
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->close();
        m_socket.reset();
    }
}

bool SocketUdpChannel::isOpen() const
{
    return !m_closed && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

QString SocketUdpChannel::errorText() const
{
    return m_socket ? m_socket->errorString() : QStringLiteral("UDP 套接字已关闭");
}

// ===========================================================================
//  StreamUdpChannel（VLESS UDP：2 字节大端长度前缀）
// ===========================================================================

StreamUdpChannel::StreamUdpChannel(std::shared_ptr<OutboundStream> stream, QObject *parent)
    : UdpChannel(parent), m_stream(std::move(stream))
{
    if (!m_stream) {
        m_closed = true;
        return;
    }
    QObject::connect(m_stream.get(), &OutboundStream::readyToRead, this, [this]() { drain(); });
    QObject::connect(m_stream.get(), &OutboundStream::streamClosed, this, [this]() {
        if (m_closed) return;
        m_closed = true;
        emit channelClosed();
    });
    // 拨号阶段可能已经有数据到达（服务端的首个回包）
    drain();
}

void StreamUdpChannel::drain()
{
    if (!m_stream || m_closed) return;

    m_buffer.append(m_stream->readAll());
    for (;;) {
        if (m_buffer.size() < 2) return;
        const int length = (quint8(m_buffer.at(0)) << 8) | quint8(m_buffer.at(1));
        if (length <= 0) {
            // 长度 0 在 VLESS UDP 里没有意义：视为协议错误，避免死循环
            closeChannel();
            return;
        }
        if (m_buffer.size() < 2 + length) return; // 半包：等下一批

        const QByteArray payload = m_buffer.mid(2, length);
        m_buffer.remove(0, 2 + length);
        emit datagramReceived(payload);
    }
}

qint64 StreamUdpChannel::sendDatagram(const QByteArray &data)
{
    if (!m_stream || m_closed || !m_stream->isOpen()) return -1;
    if (data.size() <= 0 || data.size() > kMaxDatagram) return -1;

    QByteArray framed;
    framed.reserve(data.size() + 2);
    framed.append(char((data.size() >> 8) & 0xFF));
    framed.append(char(data.size() & 0xFF));
    framed.append(data);

    // OutboundStream::write 可能只写一部分（TLS 记录边界）。UDP 允许丢包，
    // 但不能把半截帧留在流里（会让后续所有帧错位），所以这里必须写完整，
    // 写不完就关闭通道，让客户端重新建会话。
    qint64 written = 0;
    while (written < framed.size()) {
        const qint64 n = m_stream->write(framed.mid(int(written)));
        if (n <= 0) {
            closeChannel();
            return -1;
        }
        written += n;
    }
    return data.size();
}

void StreamUdpChannel::closeChannel()
{
    if (m_closed) return;
    m_closed = true;
    m_buffer.clear();
    if (m_stream) {
        m_stream->closeStream();
        m_stream.reset();
    }
}

bool StreamUdpChannel::isOpen() const
{
    return !m_closed && m_stream && m_stream->isOpen();
}

QString StreamUdpChannel::errorText() const
{
    return m_stream ? m_stream->errorText() : QStringLiteral("UDP 隧道已关闭");
}

} // namespace core
