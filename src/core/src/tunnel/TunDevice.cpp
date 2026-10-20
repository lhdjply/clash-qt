#include "tunnel/TunDevice.h"
#include <QCoreApplication>

#include "component/dns/DnsServer.h"
#include "component/dns/Resolver.h"

#include <QSocketNotifier>
#include <QThread>

#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef Q_OS_UNIX
#include <cerrno>
#include <cstring>
#include <unistd.h>
#endif

namespace core {

TunDevice::TunDevice(TunIpStack::DialFn dialFn, std::shared_ptr<Resolver> resolver,
                     std::shared_ptr<DnsServer> dnsServer, QObject *parent)
    : QObject(parent),
      m_dial(std::move(dialFn)),
      m_resolver(std::move(resolver)),
      m_dns(std::move(dnsServer))
{
}

TunDevice::~TunDevice() { shutdown(); }

void TunDevice::setTrafficSink(TunIpStack::TrafficSink sink)
{
    m_trafficSink = std::move(sink);
    if (m_stack) m_stack->setTrafficSink(m_trafficSink);
}

void TunDevice::setConnectionTracker(std::shared_ptr<ConnectionTracker> tracker)
{
    m_tracker = std::move(tracker);
    if (m_stack) m_stack->setConnectionTracker(m_tracker);
}

void TunDevice::start(int fd, int dnsUdpFd, int dnsTcpFd)
{
#ifdef Q_OS_UNIX
    if (fd < 0) {
        emit failed(QCoreApplication::translate("clashqt::core", "Invalid TUN device fd"));
        return;
    }
    if (m_stack) return;

    // 协议栈在本线程构造：它的 QTimer、信号连接都归属本线程
    m_stack = std::make_unique<TunIpStack>(m_dial, m_resolver, m_dns);
    if (m_trafficSink) m_stack->setTrafficSink(m_trafficSink);
    if (m_tracker) m_stack->setConnectionTracker(m_tracker);
    QObject::connect(m_stack.get(), &TunIpStack::logMessage, this,
                     [this](const QString &level, const QString &message) {
                         emit logMessage(level, message);
                     });
    QObject::connect(m_stack.get(), &TunIpStack::packetsReady, this,
                     [this](const QByteArray &packet) {
                         if (m_fd < 0) return;
                         const ssize_t written =
                             ::write(m_fd, packet.constData(), size_t(packet.size()));
                         if (written < 0 && errno != EAGAIN && errno != EINTR) {
                             emit logMessage(QStringLiteral("warning"),
                                             QCoreApplication::translate("clashqt::core", "Failed to write to the TUN device: %1")
                                                 .arg(QString::fromLocal8Bit(
                                                     std::strerror(errno))));
                         }
                     });

    m_fd = fd;

    // 读循环靠 EAGAIN 结束：fd 必须是非阻塞的。特权辅助程序建 TUN 时已设过，
    // 但 fd 可能来自别处（测试 / 其它平台实现），这里再兜一次 ——
    // 否则一次 read 就会把整个数据面线程永久阻塞。
    const int flags = ::fcntl(m_fd, F_GETFL, 0);
    if (flags >= 0) ::fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);

    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    QObject::connect(m_notifier, &QSocketNotifier::activated, this,
                     [this]() { readAvailable(); });

    // ---- TUN 地址上的 53：系统 DNS 指向这里，查询就在本线程应答 ----
    // 直接读 fd，不经 QUdpSocket：这样与 TUN 包走同一条线程，DNS 里的
    // 真实解析（内部有局部事件循环）不会阻塞界面。
    if (dnsUdpFd >= 0) {
        m_dnsUdpFd = dnsUdpFd;
        m_dnsUdpNotifier = new QSocketNotifier(m_dnsUdpFd, QSocketNotifier::Read, this);
        QObject::connect(m_dnsUdpNotifier, &QSocketNotifier::activated, this,
                         [this]() { serveDnsUdp(); });
    }
    if (dnsTcpFd >= 0) {
        m_dnsTcpFd = dnsTcpFd;
        m_dnsTcpNotifier = new QSocketNotifier(m_dnsTcpFd, QSocketNotifier::Read, this);
        QObject::connect(m_dnsTcpNotifier, &QSocketNotifier::activated, this,
                         [this]() { serveDnsTcp(); });
    }

    emit logMessage(QStringLiteral("info"),
                    QCoreApplication::translate("clashqt::core", "TUN data plane ready (worker thread %1, DNS 53 %2)")
                        .arg(quintptr(QThread::currentThreadId()), 0, 16)
                        .arg(dnsUdpFd >= 0 ? QCoreApplication::translate("clashqt::core", "Taken over") : QCoreApplication::translate("clashqt::core", "Not taken over")));
#else
    Q_UNUSED(fd);
    emit failed(QCoreApplication::translate("clashqt::core", "The TUN data plane is not implemented on this platform yet"));
#endif
}

void TunDevice::readAvailable()
{
#ifdef Q_OS_UNIX
    if (m_fd < 0 || !m_stack) return;

    // 处理期间关掉 notifier：下面可能走到 resolveReal / dial，它们内部会跑
    // 局部事件循环，若不关就会重入本函数、同时改写流的表 —— 之前的段错误
    // 就是这么来的。丢掉的包由 TCP 重传 / DNS 重问补回。
    if (m_notifier) m_notifier->setEnabled(false);

    QByteArray buffer;
    buffer.resize(65536);
    for (;;) {
        const ssize_t received = ::read(m_fd, buffer.data(), size_t(buffer.size()));
        if (received > 0) {
            m_stack->onPackets(QByteArray(buffer.constData(), int(received)));
            continue;
        }
        if (received == 0) break;
        if (errno == EAGAIN || errno == EINTR) break;
        if (m_closing) break; // fd 已在 shutdown 里关闭，属正常收尾
        emit logMessage(QStringLiteral("warning"),
                        QCoreApplication::translate("clashqt::core", "Failed to read the TUN device: %1")
                            .arg(QString::fromLocal8Bit(std::strerror(errno))));
        emit failed(QCoreApplication::translate("clashqt::core", "Failed to read the TUN device"));
        break;
    }

    if (m_notifier) m_notifier->setEnabled(true);
#endif
}

void TunDevice::serveDnsUdp()
{
#ifdef Q_OS_UNIX
    if (m_dnsUdpFd < 0 || !m_dns) return;
    // handleQuery 里可能跑局部事件循环（真实解析），期间关掉 notifier 防重入
    if (m_dnsUdpNotifier) m_dnsUdpNotifier->setEnabled(false);

    for (;;) {
        char                 buffer[4096];
        struct sockaddr_in   peer {};
        socklen_t            peerLength = sizeof(peer);
        const ssize_t        received   = ::recvfrom(m_dnsUdpFd, buffer, sizeof(buffer), 0,
                                                     reinterpret_cast<struct sockaddr *>(&peer),
                                                     &peerLength);
        if (received <= 0) break; // EAGAIN 或错误

        const QByteArray query(buffer, int(received));
        const QByteArray reply = m_dns->handleQuery(
            query, QHostAddress(quint32(ntohl(peer.sin_addr.s_addr))));
        if (reply.isEmpty()) continue;

        ::sendto(m_dnsUdpFd, reply.constData(), size_t(reply.size()), 0,
                 reinterpret_cast<struct sockaddr *>(&peer), peerLength);
    }

    if (m_dnsUdpNotifier) m_dnsUdpNotifier->setEnabled(true);
#endif
}

void TunDevice::serveDnsTcp()
{
#ifdef Q_OS_UNIX
    if (m_dnsTcpFd < 0 || !m_dns) return;

    // DNS over TCP：2 字节长度前缀。这里按「一次读完一个查询」处理，
    // 足够覆盖系统解析器因截断而改用 TCP 的场景。
    while (true) {
        // 关键：接出来的连接也要 SOCK_NONBLOCK。否则 recv 会一直等数据，
        // 把整个 TUN 工作线程（数据面 + DNS）卡死 —— 表现为开关点不动、TUN 不通。
        const int client = ::accept4(m_dnsTcpFd, nullptr, nullptr, SOCK_CLOEXEC | SOCK_NONBLOCK);
        if (client < 0) break;

        char          buffer[4096];
        const ssize_t received = ::recv(client, buffer, sizeof(buffer), 0);
        if (received > 2) {
            const int length = (quint8(buffer[0]) << 8) | quint8(buffer[1]);
            if (length > 0 && length <= received - 2) {
                const QByteArray reply =
                    m_dns->handleQuery(QByteArray(buffer + 2, length), QHostAddress());
                if (!reply.isEmpty()) {
                    QByteArray framed;
                    framed.append(char((reply.size() >> 8) & 0xFF));
                    framed.append(char(reply.size() & 0xFF));
                    framed.append(reply);
                    ::send(client, framed.constData(), size_t(framed.size()), 0);
                }
            }
        }
        ::close(client);
    }
#endif
}

void TunDevice::shutdown()
{
    m_closing = true;
    if (m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    if (m_dnsUdpNotifier) {
        m_dnsUdpNotifier->setEnabled(false);
        m_dnsUdpNotifier->deleteLater();
        m_dnsUdpNotifier = nullptr;
    }
    if (m_dnsTcpNotifier) {
        m_dnsTcpNotifier->setEnabled(false);
        m_dnsTcpNotifier->deleteLater();
        m_dnsTcpNotifier = nullptr;
    }
    // 关键：协议栈可能正在自己的调用栈里执行（拨号 / DNS 解析里有局部事件循环，
    // 排队的 shutdown() 会在那个嵌套循环里被调用）。这时绝不能 closeAll()/reset()
    // —— 那是把正在执行的对象删掉，返回时就是 use-after-free（关闭 TUN 闪退）。
    // 交给析构函数（~TunDevice 里再调一次 shutdown）回收即可。
    if (m_stack && !m_stack->busy()) {
        m_stack->closeAll();
        m_stack.reset();
    }
#ifdef Q_OS_UNIX
    for (int *slot : {&m_fd, &m_dnsUdpFd, &m_dnsTcpFd}) {
        if (*slot >= 0) {
            ::close(*slot);
            *slot = -1;
        }
    }
#endif
}

} // namespace core
