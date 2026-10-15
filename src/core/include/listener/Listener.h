#pragma once

// ---------------------------------------------------------------------------
//  入站监听基类
//
//  对应参照实现 listener/listener.go 的 Listener 接口与各协议监听器。
//
//  线程模型：
//    QTcpServer 在本类所属线程（主线程）接受连接，随后把「套接字描述符」
//    交给一个独立工作线程；子类在工作线程内用 setSocketDescriptor 重建
//    QTcpSocket 并完成协议握手，然后调用 Tunnel::handleStream 建立中继。
//    跨线程只传递描述符（qintptr），不传递 QObject，规避 Qt 的
//    线程亲和性限制。
//
//    每条连接一个线程在 Phase 1 是可行且易于验证的方案；后续阶段会替换为
//    「若干事件循环线程 + 异步拨号」的 reactor 模型以降低线程开销。
// ---------------------------------------------------------------------------

#include "tunnel/Tunnel.h"

#include <QHostAddress>
#include <QObject>
#include <QString>

#include <atomic>
#include <memory>

class QTcpServer;
class QTcpSocket;

namespace core {

class Listener : public QObject {
    Q_OBJECT

public:
    Listener(QString type, QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent = nullptr);
    ~Listener() override;

    // 启动监听；bindAddress 为空或 "*" 表示监听全部网卡
    bool start(const QString &bindAddress, quint16 port);

    // 停止监听并等待在途连接结束
    void stop();

    bool    isRunning() const;
    QString type() const { return m_type; }
    QString name() const { return m_name; }
    quint16 port() const { return m_port; }

    void setTunnel(std::shared_ptr<Tunnel> tunnel) { m_tunnel = std::move(tunnel); }

    // 供 MIXED 监听复用协议处理逻辑时同步真实入站端口
    void setInboundPort(quint16 port) { m_port = port; }

signals:
    void logMessage(const QString &level, const QString &message);

protected:
    // 协议处理主入口，在工作线程中执行。
    // 实现负责完成握手、调用 tunnel()->handleStream()、并关闭/释放 clientSocket。
    virtual void handleConnection(QTcpSocket *clientSocket) = 0;

    const std::shared_ptr<Tunnel> &tunnel() const { return m_tunnel; }

    // 单条连接允许的最长空闲时间（毫秒）；0 表示不限制
    int idleTimeoutMs() const { return m_idleTimeoutMs; }
    void setIdleTimeoutMs(int timeoutMs) { m_idleTimeoutMs = timeoutMs; }

    // 并发连接上限。每条在途连接大致要花掉「客户端 socket + 出站 socket +
    // 工作线程事件分发器的 eventfd」，约 3~4 个 fd；固定 512 会让 fd 需求
    // （约 2000）超过常见的 1024 软上限，连接突发时便以「打开的文件过多」
    // 收尾。改为按进程 fd 软上限推算。
    static int effectiveWorkerLimit();

private slots:
    void onNewConnection();

private:
    // 启动工作线程处理一条连接
    void spawnWorker(qintptr descriptor, const QHostAddress &peerAddress, quint16 peerPort);

    std::shared_ptr<Tunnel> m_tunnel;

    QTcpServer *m_server = nullptr;
    QString     m_type;
    QString     m_name;
    QString     m_bindAddress;
    quint16     m_port = 0;
    int         m_idleTimeoutMs = 0;

    std::atomic<int>  m_activeWorkers{0};
    std::atomic<bool> m_accepting{false};
};

} // namespace core
