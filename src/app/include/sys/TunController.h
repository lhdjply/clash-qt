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
//  授权方式与参照实现一致（见其 main/core/permissions.ts）：
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
    QString device  = QStringLiteral(CLASHQT_TUN_DEVICE);
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

    // 是否已建立到辅助程序/设备的连接（Linux：辅助程序 socket 已建立；
    // Windows：Wintun 适配器与路由已就绪）
    bool isRunning() const;
    // 拿到的 TUN 设备标识（POSIX 是 fd，Windows 是适配器 LUID；未打开时为 -1 / 0）
    qintptr deviceId() const { return m_deviceFd; }
    // 辅助程序在 TUN 地址上接管的 DNS socket（未接管时都是 -1）
    int  dnsUdpFd() const { return int(m_dnsUdpFd); }
    int  dnsTcpFd() const { return int(m_dnsTcpFd); }
    void takeDnsFds(int *udp, int *tcp)
    {
        if (udp) *udp = int(m_dnsUdpFd);
        if (tcp) *tcp = int(m_dnsTcpFd);
        m_dnsUdpFd = -1;
        m_dnsTcpFd = -1;
    }

    // 交出设备的所有权（交给内核 Hub::startTun）：调用后本类不再释放它
    qintptr takeDeviceFd()
    {
        const qintptr fd = m_deviceFd;
        m_deviceFd       = -1;
        return fd;
    }
    // Windows：内核要在 TUN 地址上自己监听 53（POSIX 由辅助程序递回 fd，返回空）
    QString dnsBindAddress() const { return m_dnsAddress; }
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
    // Windows 没有辅助程序 socket，运行状态单独记一份；Linux 由 m_socket 推导
    bool             m_running  = false;

    int     m_listenSocket = -1;
    int     m_socket       = -1; // 与辅助程序的连接
    qintptr m_deviceFd     = -1; // TUN 设备（POSIX: fd；Windows: 适配器 LUID）
    qintptr m_dnsUdpFd     = -1; // TUN 地址上的 DNS(UDP)
    qintptr m_dnsTcpFd     = -1; // TUN 地址上的 DNS(TCP)
    QString m_socketPath;
    QString m_deviceName;
    QString m_pendingHeader;
    bool    m_dryRun = false;
    // Windows：TUN 地址（内核要在它上面监听 53）
    QString m_dnsAddress;
#ifdef Q_OS_WIN
    // 本次加过的路由：关闭时逐条删掉（不删的话重启后仍在，会把流量继续引向 TUN）
    struct RouteEntry {
        quint64 luid    = 0;
        quint32 network = 0;
        int     prefix  = 0;
        quint32 nextHop = 0;   // 删除时也要一致，否则 DeleteIpForwardEntry2 匹配不到
    };
    quintptr m_adapter    = 0;   // Wintun 适配器句柄（本类持有，stop 时关闭）
    quint64  m_luid       = 0;   // 适配器 LUID（配地址/路由用）
    QString  m_adapterName;
    QList<RouteEntry> m_addedRoutes;
#endif
};

} // namespace clashqt
