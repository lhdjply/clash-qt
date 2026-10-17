#pragma once

// ---------------------------------------------------------------------------
//  TUN 控制器（主程序侧）
//
//  与特权辅助程序 clash-qt-tun 的分工（见 src/tunhelper/main.cpp）：
//    辅助程序（必要时 setuid root）负责建 TUN、配地址、日后接管路由；
//    本类负责授权检查 / 申请、拉起辅助程序、**通过 unix socket + SCM_RIGHTS
//    收下 TUN 的 fd**，并持有它供用户态协议栈读写。
//
//  为什么要 fd 传递而不是让辅助程序转发数据：
//    数据面（协议栈、规则匹配、出站拨号）都在主程序里，辅助程序只做特权动作、
//    完全不碰流量 —— 攻击面小、每包没有 IPC 开销。
//
//  授权方式与参照实现一致（clash-party/src/main/core/permissions.ts）：
//    Linux 用 pkexec 执行 chown root:root + chmod +sx，授权一次以后不再弹密码；
//    权限检查 = 辅助程序带 setuid 位且属主为 root。
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>
#include <QStringList>

class QProcess;
class QSocketNotifier;
class QTimer;

namespace clashqt {

// 一次 open 的参数
struct TunOptions {
    QString device  = QStringLiteral("clash-qt-tun");
    QString address = QStringLiteral("172.19.0.1/30");
    int     mtu     = 1500;
    // 是否接管默认路由（须由特权辅助程序执行）
    bool    autoRoute = true;
    // 不接管路由的地址（通常是代理服务器 IP）：出站流量必须绕开 TUN，否则成环
    QStringList routeExcludes;
    // 假 IP 网段（如 198.18.0.0/15）：给它单独加一条路由，避免默认路由被
    // NetworkManager 抢回后应用流量进不了 TUN
    QString     fakeIpCidr = QStringLiteral("198.18.0.0/15");
    // 不碰真实网络：用 socketpair 冒充 TUN，仅用于在无 root 环境下验证
    // 「fd 传递 + 生命周期」这条链路（见 tools 里的自检程序）
    bool    dryRun  = false;
};

class TunController : public QObject {
    Q_OBJECT

public:
    explicit TunController(QObject *parent = nullptr);
    ~TunController() override;

    // 辅助程序路径（与主程序同目录）与是否存在
    QString helperPath() const;
    bool    helperExists() const;

    // 是否已被授权（setuid 位 + 属主 root），与参照实现同款判定
    bool hasPermission() const;
    // 弹系统授权窗口（pkexec）完成 chown/chmod +sx
    bool grantPermission(QString *error = nullptr);

    bool isRunning() const { return m_socket >= 0; }
    // 收到的 TUN 设备 fd（未打开时为 -1）。
    int  deviceFd() const { return m_deviceFd; }
    // 辅助程序在 TUN 地址上接管的 DNS socket（未接管时都是 -1）
    int  dnsUdpFd() const { return m_dnsUdpFd; }
    int  dnsTcpFd() const { return m_dnsTcpFd; }
    void takeDnsFds(int *udp, int *tcp)
    {
        if (udp) *udp = m_dnsUdpFd;
        if (tcp) *tcp = m_dnsTcpFd;
        m_dnsUdpFd = -1;
        m_dnsTcpFd = -1;
    }

    // 交出 fd 的所有权（交给内核 Hub::startTun）：调用后本类不再关闭它
    int  takeDeviceFd()
    {
        const int fd = m_deviceFd;
        m_deviceFd   = -1;
        return fd;
    }
    // 辅助程序在握手里报告的信息（设备名 / 地址 / 是否 dry-run）
    QString deviceName() const { return m_deviceName; }
    bool    isDryRun() const { return m_dryRun; }

    // 打开 TUN：建 socket、拉起辅助程序。**异步**——立即返回，fd 到达后
    // 发 ready()，失败发 failed()。
    //
    // 为什么必须异步：辅助程序要建设备、配地址、（可能要提权），耗时不定。
    // 早期版本在 start() 里用 processEvents() 空转等 fd，结果自己的
    // onReadable() 在 start() 返回前就重入，可能把 m_process/m_socket 释放掉，
    // start() 回来继续用就段错误；同时整个过程把 GUI 线程占住 →「死机」。
    bool start(const TunOptions &options, QString *error = nullptr);
    // 是否还在等 fd（已调用 start 但尚未就绪）
    bool isPending() const { return m_pending; }
    // 关闭：通知辅助程序清理（删接口）并关闭 fd
    void stop();

signals:
    void stateChanged();
    // fd 已到手，可以 takeDeviceFd() 交给内核
    void ready();
    // 启用失败（辅助程序报错、权限不足、超时）
    void failed(const QString &reason);

private:
    void onNewConnection();
    void onReadable();
    // 失败收尾：清资源并通知，不依赖 start() 的栈帧
    void failAndCleanup(const QString &reason);
    void closeFd();
    void cleanupSocketFile();

    QProcess        *m_process = nullptr;
    QSocketNotifier *m_notifier = nullptr;
    QTimer          *m_timeout  = nullptr;
    bool             m_pending  = false;
    bool             m_cleaning = false;

    int     m_listenSocket = -1;
    int     m_socket       = -1; // 与辅助程序的连接
    int     m_deviceFd     = -1; // TUN 设备
    int     m_dnsUdpFd     = -1; // TUN 地址上的 DNS(UDP)
    int     m_dnsTcpFd     = -1; // TUN 地址上的 DNS(TCP)
    QString m_socketPath;
    QString m_deviceName;
    QString m_pendingHeader;
    bool    m_dryRun = false;
};

} // namespace clashqt
