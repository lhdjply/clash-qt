#pragma once

// ---------------------------------------------------------------------------
//  UDP 数据报通道
//
//  TUN 的 UDP 中继需要两种完全不同的承载方式，这里用统一抽象抹平差异：
//
//    SocketUdpChannel —— 已连接的 QUdpSocket（DIRECT 直发、SOCKS5 UDP 关联）。
//                        每条 UDP 会话一个 socket，回包靠 readyRead。
//    StreamUdpChannel —— 把 OutboundStream 当成「数据报流」，用 2 字节大端
//                        长度前缀分帧（VLESS CMD=0x02 的 UDP 承载方式）。
//
//  统一语义：sendDatagram 发一个数据报，datagramReceived 收一个数据报。
//  UDP 有丢包语义，所以发送失败直接丢弃即可，不做重传。
//
//  线程模型：通道由拨号线程创建并持有，所有方法只能在该线程调用；
//  数据面通过 UdpWorker 的队列信号跨线程访问（见 tunnel/UdpWorker.h）。
// ---------------------------------------------------------------------------

#include <QByteArray>
#include <QObject>
#include <QString>

#include <memory>

class QUdpSocket;

namespace core {

class OutboundStream;

class UdpChannel : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;
    ~UdpChannel() override = default;

    // 发送一个数据报；<0 表示失败（UDP 允许丢弃，调用方可直接放弃这个包）
    virtual qint64 sendDatagram(const QByteArray &data) = 0;

    // 关闭通道并释放底层资源
    virtual void closeChannel() = 0;

    virtual bool    isOpen() const = 0;
    virtual QString errorText() const { return QString(); }

signals:
    void datagramReceived(const QByteArray &data);
    void channelClosed();
};

// ---------------------------------------------------------------------------
//  已连接的 QUdpSocket：DIRECT 直发 / SOCKS5 UDP 关联
// ---------------------------------------------------------------------------
class SocketUdpChannel : public UdpChannel {
    Q_OBJECT

public:
    explicit SocketUdpChannel(std::shared_ptr<QUdpSocket> socket, QObject *parent = nullptr);

    qint64  sendDatagram(const QByteArray &data) override;
    void    closeChannel() override;
    bool    isOpen() const override;
    QString errorText() const override;

private:
    void drain();

    std::shared_ptr<QUdpSocket> m_socket;
    bool                        m_closed = false;
};

// ---------------------------------------------------------------------------
//  隧道内数据报流：VLESS UDP（CMD=0x02）
//
//  线格式：每条数据报前面加 2 字节大端长度（0 ~ 65535）。
//  底层 OutboundStream 已经负责 VLESS 应答头的懒消费，这里只处理分帧。
// ---------------------------------------------------------------------------
class StreamUdpChannel : public UdpChannel {
    Q_OBJECT

public:
    explicit StreamUdpChannel(std::shared_ptr<OutboundStream> stream, QObject *parent = nullptr);

    qint64  sendDatagram(const QByteArray &data) override;
    void    closeChannel() override;
    bool    isOpen() const override;
    QString errorText() const override;

private:
    void drain();

    std::shared_ptr<OutboundStream> m_stream;
    QByteArray                      m_buffer; // 半包累积
    bool                            m_closed = false;
};

} // namespace core
