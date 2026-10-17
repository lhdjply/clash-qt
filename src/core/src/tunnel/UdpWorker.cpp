#include "tunnel/UdpWorker.h"

#include "tunnel/UdpChannel.h"

namespace core {

UdpWorker::UdpWorker(DialFn dial, DialContext context, QObject *parent)
    : QObject(parent), m_dial(std::move(dial)), m_context(std::move(context))
{
}

UdpWorker::~UdpWorker()
{
    if (m_channel) {
        m_channel->closeChannel();
        m_channel.reset();
    }
}

void UdpWorker::start()
{
    if (!m_dial) {
        emit failed(QStringLiteral("没有可用的拨号实现"));
        return;
    }

    DialResult result = m_dial(m_context);
    if (!result.udpChannel) {
        emit failed(result.error.isEmpty() ? QStringLiteral("UDP 拨号失败") : result.error);
        return;
    }

    m_channel = result.udpChannel;

    QObject::connect(m_channel.get(), &UdpChannel::datagramReceived, this,
                     [this](const QByteArray &data) {
                         if (!m_closing) emit datagram(data);
                     });
    QObject::connect(m_channel.get(), &UdpChannel::channelClosed, this, [this]() {
        if (m_closing) return;
        m_closing = true;
        emit closed();
    });

    emit connected();
}

void UdpWorker::sendDatagram(const QByteArray &data)
{
    if (m_closing || !m_channel || !m_channel->isOpen()) return;
    // 发送失败即丢弃：UDP 本身不保证送达，重传是上层（QUIC/STUN）的事
    m_channel->sendDatagram(data);
}

void UdpWorker::closeChannel()
{
    m_closing = true;
    if (m_channel) {
        m_channel->closeChannel();
        m_channel.reset();
    }
}

} // namespace core
