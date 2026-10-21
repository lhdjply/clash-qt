#include "tunnel/TunDevice.h"
#include <QCoreApplication>

#include "component/dns/DnsServer.h"
#include "component/dns/Resolver.h"

#include <QSocketNotifier>
#include <QThread>

#ifdef Q_OS_WIN
#include "tunnel/Wintun.h"

#include <QUdpSocket>

// 顺序有讲究：winsock2 / ws2ipdef 必须早于 windows.h，netioapi 依赖它们的类型
#include <winsock2.h>
#include <ws2ipdef.h>

#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>

#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#else
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cerrno>
#include <cstring>
#include <unistd.h>
#endif

namespace core {

#ifdef Q_OS_WIN

// ---------------------------------------------------------------------------
//  Windows 数据面
//
//  Wintun 没有可被 QSocketNotifier 监听的描述符，只有「等事件 → 收包」这一种
//  用法，所以用一个专职读线程收包，攒成一批投递回本对象所在线程 —— 协议栈
//  的 onPackets 本来就吃「一批连续 IP 包」，与 POSIX 侧读到 EAGAIN 为止等价。
//
//  一批投递同时解决了重入：协议栈执行期间（拨号 / DNS 里有局部事件循环）
//  再进来的批次直接丢弃，和 Linux 侧关掉 notifier 的行为一致。
// ---------------------------------------------------------------------------
struct TunDevice::WinBackend {
    quintptr  adapter   = 0;   // Wintun 适配器（由本对象打开并关闭）
    quintptr  session   = 0;   // Wintun 会话
    HANDLE    readEvent = nullptr;
    std::thread reader;
    std::atomic<bool> stopping{false};
    std::atomic<bool> busy{false};   // 协议栈正在处理
    bool              dnsBusy = false;

    TunDevice *owner = nullptr;
    std::function<void(const QByteArray &)> deliver;
    std::function<void(const QString &)>    reportError;   // 致命：会话中断
    std::function<void(const QString &)>    reportWarning; // 非致命：DNS 未接管

    QUdpSocket *dns = nullptr;   // 在 TUN 地址上应答 53

    bool start(qintptr luid, TunDevice *device, std::function<void(const QByteArray &)> sink,
               std::function<void(const QString &)> onError,
               std::function<void(const QString &)> onWarning, QString *error);
    bool send(const QByteArray &packet);
    void startDns(const QString &address, const std::shared_ptr<DnsServer> &server);
    void serveDns(const std::shared_ptr<DnsServer> &server);
    void stop();

private:
    void runReader();
    void dispatch(const QByteArray &batch);
};

bool TunDevice::WinBackend::start(qintptr luid, TunDevice *device,
                                  std::function<void(const QByteArray &)> sink,
                                  std::function<void(const QString &)> onError,
                                  std::function<void(const QString &)> onWarning, QString *error)
{
    owner         = device;
    deliver       = std::move(sink);
    reportError   = std::move(onError);
    reportWarning = std::move(onWarning);

    // LUID → 别名 → 打开适配器。句柄归本对象：TunController 那边还有自己的一份，
    // 各自关闭互不影响（适配器本身由系统持有，关闭句柄不会删掉它）。
    NET_LUID netLuid{};
    netLuid.Value = LONGLONG(luid);
    wchar_t alias[256] = {0};
    if (::ConvertInterfaceLuidToAlias(&netLuid, alias, 256) != NO_ERROR) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::core",
                                                 "Could not resolve the TUN adapter from its LUID");
        }
        return false;
    }

    adapter = wintun::openAdapter(QString::fromWCharArray(alias), error);
    if (!adapter) return false;

    session = wintun::startSession(adapter, error);
    if (!session) {
        wintun::closeAdapter(adapter);
        adapter = 0;
        return false;
    }

    readEvent = reinterpret_cast<HANDLE>(wintun::readWaitEvent(session));
    if (!readEvent) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::core",
                                                 "Could not get the Wintun read event");
        }
        wintun::endSession(session);
        session = 0;
        wintun::closeAdapter(adapter);
        adapter = 0;
        return false;
    }

    stopping = false;
    reader   = std::thread([this]() { runReader(); });
    return true;
}

void TunDevice::WinBackend::runReader()
{
    std::vector<char> buffer(65536);

    while (!stopping) {
        // 100ms 超时：不依赖唤醒事件，关闭时最多等这么久
        ::WaitForSingleObject(readEvent, 100);

        QByteArray batch;
        bool       broken = false;
        for (;;) {
            const int size = wintun::receivePacket(session, buffer.data(), int(buffer.size()));
            if (size > 0) {
                batch.append(buffer.data(), size);
                continue;
            }
            if (size == 0) break;       // 这一轮收完了
            if (!stopping) broken = true; // 会话异常中断
            break;
        }

        if (!batch.isEmpty()) {
            const QByteArray payload = batch;
            QMetaObject::invokeMethod(owner, [this, payload]() { dispatch(payload); },
                                      Qt::QueuedConnection);
        }
        if (broken) {
            QMetaObject::invokeMethod(owner,
                                      [this]() {
                                          if (reportError) {
                                              reportError(QCoreApplication::translate(
                                                  "clashqt::core", "The Wintun session was closed"));
                                          }
                                      },
                                      Qt::QueuedConnection);
            return;
        }
    }
}

void TunDevice::WinBackend::dispatch(const QByteArray &batch)
{
    if (!deliver) return;
    // 重入（协议栈里跑了局部事件循环）：丢掉这一批，TCP 会重传、DNS 会重问
    if (busy.exchange(true)) return;
    deliver(batch);
    busy = false;
}

bool TunDevice::WinBackend::send(const QByteArray &packet)
{
    if (!session || packet.isEmpty()) return false;
    return wintun::sendPacket(session, packet.constData(), int(packet.size()));
}

void TunDevice::WinBackend::startDns(const QString &address, const std::shared_ptr<DnsServer> &server)
{
    if (address.isEmpty() || !server) return;

    // TUN 地址是本机地址，发往它的包不会进入 Wintun 会话，所以在该地址上
    // 直接监听 53（等价于 Linux 辅助程序 bind 到 TUN 地址并递回 fd）
    dns = new QUdpSocket(owner);
    if (!dns->bind(QHostAddress(address), 53)) {
        const QString reason = dns->errorString();
        delete dns;
        dns = nullptr;
        // 与 Linux 侧一致：DNS 没接管不致命，默认路由已接管时查询仍会进 TUN
        if (reportWarning) {
            reportWarning(QCoreApplication::translate(
                              "clashqt::core", "DNS is not taken over on the TUN address (%1): %2")
                              .arg(address, reason));
        }
        return;
    }
    QObject::connect(dns, &QUdpSocket::readyRead, owner, [this, server]() { serveDns(server); });
}

void TunDevice::WinBackend::serveDns(const std::shared_ptr<DnsServer> &server)
{
    if (!dns || dnsBusy) return;
    dnsBusy = true;

    while (dns->hasPendingDatagrams()) {
        QByteArray query;
        query.resize(int(dns->pendingDatagramSize()));

        QHostAddress peer;
        quint16      peerPort = 0;
        if (dns->readDatagram(query.data(), query.size(), &peer, &peerPort) < 0) break;

        const QByteArray reply = server->handleQuery(query, peer);
        if (!reply.isEmpty()) dns->writeDatagram(reply, peer, peerPort);
    }

    dnsBusy = false;
}

void TunDevice::WinBackend::stop()
{
    stopping = true;
    if (reader.joinable()) reader.join();

    if (dns) {
        dns->close();
        dns->deleteLater();
        dns = nullptr;
    }
    if (session) {
        wintun::endSession(session);
        session = 0;
    }
    if (adapter) {
        wintun::closeAdapter(adapter);
        adapter = 0;
    }
    readEvent     = nullptr;
    deliver       = nullptr;
    reportError   = nullptr;
    reportWarning = nullptr;
}

#endif // Q_OS_WIN

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

void TunDevice::start(qintptr device, int dnsUdpFd, int dnsTcpFd, const QString &dnsBindAddress)
{
    if (m_stack) return;
#ifdef Q_OS_WIN
    if (device == 0) {
        emit failed(QCoreApplication::translate("clashqt::core", "Invalid TUN adapter"));
        return;
    }
#else
    if (device < 0) {
        emit failed(QCoreApplication::translate("clashqt::core", "Invalid TUN device fd"));
        return;
    }
#endif

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
#ifdef Q_OS_WIN
                         if (!m_win) return;
                         if (!m_win->send(packet)) {
                             emit logMessage(QStringLiteral("warning"),
                                             QCoreApplication::translate("clashqt::core", "Failed to send a packet to the TUN adapter"));
                         }
#else
                         if (m_fd < 0) return;
                         const ssize_t written =
                             ::write(int(m_fd), packet.constData(), size_t(packet.size()));
                         if (written < 0 && errno != EAGAIN && errno != EINTR) {
                             emit logMessage(QStringLiteral("warning"),
                                             QCoreApplication::translate("clashqt::core", "Failed to write to the TUN device: %1")
                                                 .arg(QString::fromLocal8Bit(
                                                     std::strerror(errno))));
                         }
#endif
                     });

    m_fd = device;

#ifdef Q_OS_WIN
    m_win = std::make_unique<WinBackend>();
    QString error;
    const bool ok = m_win->start(
        device, this,
        [this](const QByteArray &batch) {
            if (m_stack) m_stack->onPackets(batch);
        },
        [this](const QString &reason) {
            emit logMessage(QStringLiteral("warning"), reason);
            emit failed(reason);
        },
        [this](const QString &reason) { emit logMessage(QStringLiteral("warning"), reason); },
        &error);
    if (!ok) {
        m_win.reset();
        m_stack.reset();
        m_fd = -1;
        emit failed(error.isEmpty()
                        ? QCoreApplication::translate("clashqt::core", "Could not open the TUN adapter")
                        : error);
        return;
    }
    m_win->startDns(dnsBindAddress, m_dns);

    emit logMessage(QStringLiteral("info"),
                    QCoreApplication::translate("clashqt::core", "TUN data plane ready (worker thread %1, Wintun %2)")
                        .arg(quintptr(QThread::currentThreadId()), 0, 16)
                        .arg(wintun::versionText()));
#else
    Q_UNUSED(dnsBindAddress);

    // 读循环靠 EAGAIN 结束：fd 必须是非阻塞的。特权辅助程序建 TUN 时已设过，
    // 但 fd 可能来自别处（测试 / 其它平台实现），这里再兜一次 ——
    // 否则一次 read 就会把整个数据面线程永久阻塞。
    const int flags = ::fcntl(int(m_fd), F_GETFL, 0);
    if (flags >= 0) ::fcntl(int(m_fd), F_SETFL, flags | O_NONBLOCK);

    m_notifier = new QSocketNotifier(int(m_fd), QSocketNotifier::Read, this);
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
#endif
}

void TunDevice::readAvailable()
{
#ifdef Q_OS_WIN
    // Windows 由读包线程直接投递批次，没有可监听的描述符
#else
    if (m_fd < 0 || !m_stack) return;

    // 处理期间关掉 notifier：下面可能走到 resolveReal / dial，它们内部会跑
    // 局部事件循环，若不关就会重入本函数、同时改写流的表 —— 之前的段错误
    // 就是这么来的。丢掉的包由 TCP 重传 / DNS 重问补回。
    if (m_notifier) m_notifier->setEnabled(false);

    QByteArray buffer;
    buffer.resize(65536);
    for (;;) {
        const ssize_t received = ::read(int(m_fd), buffer.data(), size_t(buffer.size()));
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
#ifdef Q_OS_WIN
    // Windows 侧由 WinBackend 的 QUdpSocket 应答
#else
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
#ifdef Q_OS_WIN
    // Windows 侧不做 53/TCP：stub resolver 走 UDP，截断时才回落 TCP
#else
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
#ifdef Q_OS_WIN
    // 只停不销毁：读线程可能还有若干排队中的投递，投递里带着后端的指针，
    // 提前析构就是 use-after-free。stop() 已经把 deliver 置空，剩下的投递
    // 会直接返回；对象本身随本对象一起回收（析构里再调一次 stop() 也是幂等的）。
    if (m_win) m_win->stop();
    m_fd = -1;
#else
    if (m_fd >= 0) {
        ::close(int(m_fd));
        m_fd = -1;
    }
    if (m_dnsUdpFd >= 0) {
        ::close(m_dnsUdpFd);
        m_dnsUdpFd = -1;
    }
    if (m_dnsTcpFd >= 0) {
        ::close(m_dnsTcpFd);
        m_dnsTcpFd = -1;
    }
#endif
}

} // namespace core
