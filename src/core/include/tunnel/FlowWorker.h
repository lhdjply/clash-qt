#pragma once

// ---------------------------------------------------------------------------
//  TUN 连接的工作线程执行体（每个连接一个）
//
//  为什么要独立线程
//  ---------------------------------------------------------------------------
//  拨号是**同步阻塞**的：TLS 握手、WebSocket 升级、VLESS/Hysteria2 握手，
//  加上海外节点 RTT，通常是 300~800ms，失败时可达数秒。若把它放在 TUN 数据面
//  线程里，这段时间内**所有其他连接一起排队** —— 用户感受就是「网页很卡」，
//  页面里并发几十个请求时尤其明显。
//
//  所以这里把「拨号 + 上游读写」整体搬到独立线程：数据面只做 TCP 状态机
//  （收包、序号、窗口、回包），永不阻塞。这与参照实现的「每连接一个线程」一致。
//
//  线程边界
//  ---------------------------------------------------------------------------
//  · 该线程独占：OutboundStream / 底层 socket、写缓冲
//  · 与数据面之间只通过队列信号交换：connected / failed / dataReady / closed
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

#include <QByteArray>
#include <QObject>
#include <QString>

#include <functional>
#include <memory>

namespace core {

class OutboundStream;

class FlowWorker : public QObject {
    Q_OBJECT

public:
    using DialFn = std::function<DialResult(const DialContext &)>;

    FlowWorker(DialFn dial, DialContext context, QObject *parent = nullptr);
    ~FlowWorker() override;

public slots:
    // 在所属线程里执行拨号（阻塞本线程，不影响数据面）
    void start();
    // 数据面把客户端数据投递过来（可能被多次调用）
    void writeData(const QByteArray &data);
    // 关闭上游
    void closeStream();

signals:
    void connected();
    void failed(const QString &reason);
    void dataReady(const QByteArray &data);
    void closed();

private:
    void flushWrites();

    DialFn                          m_dial;
    DialContext                     m_context;
    std::shared_ptr<OutboundStream> m_stream;
    QByteArray                      m_pending; // 写入未完成的部分
    bool                            m_closing = false;
};

} // namespace core
