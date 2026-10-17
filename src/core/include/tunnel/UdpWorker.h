#pragma once

// ---------------------------------------------------------------------------
//  UDP 连接的工作线程执行体（每条 UDP 会话一个）
//
//  与 FlowWorker 同构，但承载的是数据报：
//    · 拨号（TLS 握手 / QUIC 建连 / UDP 关联）是同步阻塞的，不能放在数据面
//    · 通道建立后，收包（QUIC 回程 / STUN 响应）走 readyRead，天然异步
//
//  线程边界：
//    · 该线程独占 UdpChannel（底层 socket 或隧道流）
//    · 与数据面只通过队列信号交换：connected / failed / datagram / closed
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

#include <QByteArray>
#include <QObject>
#include <QString>

#include <functional>
#include <memory>

namespace core {

class UdpChannel;

class UdpWorker : public QObject {
    Q_OBJECT

public:
    using DialFn = std::function<DialResult(const DialContext &)>;

    UdpWorker(DialFn dial, DialContext context, QObject *parent = nullptr);
    ~UdpWorker() override;

public slots:
    // 在所属线程里完成拨号（阻塞本线程，不影响数据面）
    void start();
    // 数据面投递一个数据报（拨号完成后才会被调用）
    void sendDatagram(const QByteArray &data);
    // 关闭通道
    void closeChannel();

signals:
    void connected();
    void failed(const QString &reason);
    void datagram(const QByteArray &data);
    void closed();

private:
    DialFn                       m_dial;
    DialContext                  m_context;
    std::shared_ptr<UdpChannel>  m_channel;
    bool                         m_closing = false;
};

} // namespace core
