#pragma once

// ---------------------------------------------------------------------------
//  TUN 设备工作对象（跑在独立工作线程里）
//
//  为什么必须单独一个线程
//  ---------------------------------------------------------------------------
//  TUN 的数据面里有两处**会跑局部事件循环的阻塞调用**：
//    · DnsServer 处理非 fake-ip 域名时走 Resolver::resolveReal()（内部 QEventLoop）
//    · 每个新连接要 Adapter::dial()（内部同样等握手）
//  这两者放在 GUI 线程会同时踩两个坑：
//    1) 界面被阻塞 —— 用户看到的就是「开一会就卡死」；
//    2) 嵌套事件循环期间 TUN 的 QSocketNotifier 会再次触发，onPackets 重入，
//       流的表与 fake-ip 池被同时改写 —— 表现为「过一会段错误闪退」。
//  项目里现有入站监听也是「每条连接一个 QThread」（见 listener/Listener.cpp），
//  这里沿用同一条规则：TunIpStack 的构造、收发包、拨号、DNS 解析全部在本线程。
//
//  另外还有两道防重入：
//    · 处理包期间临时关闭 notifier；
//    · TunIpStack 内部再有 m_inPacket 标志兜底。
//  宁可让内核丢掉少量包（TCP 会重传、DNS 会重问），也不能重入。
// ---------------------------------------------------------------------------

#include "tunnel/TunIpStack.h"

#include <QObject>
#include <QString>

#include <memory>

class QSocketNotifier;
class QThread;

namespace core {

class DnsServer;
class Resolver;
class ConnectionTracker;

class TunDevice : public QObject {
    Q_OBJECT

public:
    TunDevice(TunIpStack::DialFn dialFn, std::shared_ptr<Resolver> resolver,
              std::shared_ptr<DnsServer> dnsServer, QObject *parent = nullptr);
    ~TunDevice() override;

    // 流量出口（TUN 的字节不经过 Tunnel::handleStream，需显式汇总）。
    // 必须在 start() 之前、且在本对象尚未进入工作线程时设置。
    void setTrafficSink(TunIpStack::TrafficSink sink);

    // 连接跟踪器（/connections 的数据源）。同样必须在 start() 之前设置：
    // 协议栈在 start() 里才创建，之后设置就赶不上已经建立的连接。
    void setConnectionTracker(std::shared_ptr<ConnectionTracker> tracker);

    // 在工作线程中调用：接管设备并开始收发
    //
    // device：POSIX 是 TUN 设备 fd；Windows 是 Wintun 适配器 LUID
    // dnsUdpFd / dnsTcpFd：辅助程序在 TUN 地址上绑好的 53 端口（POSIX）
    // dnsBindAddress：Windows 上 TUN 地址是本机地址，发往它不进入 Wintun 会话，
    //                 所以要在该地址上自己监听 53（POSIX 忽略此参数）
    void start(qintptr device, int dnsUdpFd, int dnsTcpFd,
               const QString &dnsBindAddress = QString());
    // 在工作线程中调用：停止并释放设备
    void shutdown();

signals:
    void logMessage(const QString &level, const QString &message);
    void failed(const QString &reason);

private:
    void readAvailable();
    // TUN 地址上的 53 端口：把查询交给 DnsServer，答复写回
    void serveDnsUdp();
    void serveDnsTcp();

    // Windows 数据面（Wintun 会话 + 读包线程）全部收在这个不透明结构里，
    // 免得头文件被 windows.h 拖进来
    struct WinBackend;

    TunIpStack::DialFn         m_dial;
    std::shared_ptr<Resolver>  m_resolver;
    std::shared_ptr<DnsServer> m_dns;
    std::shared_ptr<ConnectionTracker> m_tracker;

    std::unique_ptr<TunIpStack> m_stack;
    TunIpStack::TrafficSink     m_trafficSink;
    QSocketNotifier            *m_notifier = nullptr;
    qintptr                     m_fd       = -1;
    // 已进入关闭流程：读循环此时报 EBADF 属正常，不再刷警告
    bool                        m_closing  = false;
    int                         m_dnsUdpFd = -1;
    int                         m_dnsTcpFd = -1;
    QSocketNotifier            *m_dnsUdpNotifier = nullptr;
    QSocketNotifier            *m_dnsTcpNotifier = nullptr;
#ifdef Q_OS_WIN
    std::unique_ptr<WinBackend> m_win;
#endif
};

} // namespace core
