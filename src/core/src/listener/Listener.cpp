#include "listener/Listener.h"
#include <QCoreApplication>

#include "base/Common.h"

#include <QElapsedTimer>
#include <QFile>
#include <QTcpServer>
#include <QStringList>
#include <QTcpSocket>
#include <QThread>

#include <memory>

#include <unistd.h>

namespace core {

Listener::Listener(QString type, QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent)
    : QObject(parent), m_tunnel(std::move(tunnel)), m_type(std::move(type)), m_name(std::move(name))
{
}

Listener::~Listener()
{
    stop();
}

bool Listener::start(const QString &bindAddress, quint16 port)
{
    if (m_server) return true;

    m_server = new QTcpServer(this);
    QObject::connect(m_server, &QTcpServer::newConnection, this, &Listener::onNewConnection);

    QHostAddress address;
    QString      normalized = bindAddress.trimmed();
    if (normalized.isEmpty() || normalized == QLatin1String("*")
        || normalized == QLatin1String("0.0.0.0")) {
        address = QHostAddress::Any;
    } else if (normalized == QLatin1String("::")) {
        address = QHostAddress::AnyIPv6;
    } else if (!address.setAddress(normalized)) {
        emit logMessage(QStringLiteral("error"),
                        QCoreApplication::translate("clashqt::core", "%1 has an invalid listen address: %2").arg(m_type, normalized));
        delete m_server;
        m_server = nullptr;
        return false;
    }

    if (!m_server->listen(address, port)) {
        emit logMessage(QStringLiteral("error"),
                        QCoreApplication::translate("clashqt::core", "%1 failed to listen on %2:%3: %4")
                            .arg(m_type, normalized)
                            .arg(port)
                            .arg(m_server->errorString()));
        delete m_server;
        m_server = nullptr;
        return false;
    }

    m_bindAddress = normalized;
    m_port        = m_server->serverPort();
    m_accepting.store(true, std::memory_order_release);

    emit logMessage(QStringLiteral("info"),
                    QCoreApplication::translate("clashqt::core", "%1 inbound started: %2:%3 (concurrency limit %4)")
                        .arg(m_type, normalized)
                        .arg(m_port)
                        .arg(effectiveWorkerLimit()));
    return true;
}

void Listener::stop()
{
    m_accepting.store(false, std::memory_order_release);

    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }

    // 等待在途连接结束（最多 3 秒），避免工作线程持有已析构的 Listener
    QElapsedTimer timer;
    timer.start();
    while (m_activeWorkers.load(std::memory_order_acquire) > 0 && timer.elapsed() < 3000) {
        QThread::msleep(20);
    }

    const int remaining = m_activeWorkers.load(std::memory_order_acquire);
    if (remaining > 0) {
        emit logMessage(QStringLiteral("warning"),
                        QCoreApplication::translate("clashqt::core", "%1 still has %2 open connection(s)").arg(m_type).arg(remaining));
    }
}

bool Listener::isRunning() const
{
    return m_server != nullptr && m_server->isListening();
}

void Listener::onNewConnection()
{
    if (!m_accepting.load(std::memory_order_acquire)) {
        // 停止过程中拒绝新连接
        while (m_server && m_server->hasPendingConnections()) {
            QTcpSocket *socket = m_server->nextPendingConnection();
            socket->abort();
            socket->deleteLater();
        }
        return;
    }

    while (m_server && m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        if (!socket) continue;

        const int workerLimit = effectiveWorkerLimit();
        if (m_activeWorkers.load(std::memory_order_acquire) >= workerLimit) {
            emit logMessage(QStringLiteral("warning"),
                            QCoreApplication::translate("clashqt::core", "Concurrency limit %1 reached; rejecting new connections").arg(workerLimit));
            socket->abort();
            socket->deleteLater();
            continue;
        }

        const QHostAddress peer       = socket->peerAddress();
        const quint16      peerPort   = socket->peerPort();
        const int          sourceFd   = int(socket->socketDescriptor());

        // 用 dup() 复制一份描述符交给工作线程：两份描述符指向同一条 TCP 连接，
        // 各自独立关闭互不影响。这样避免了「QTcpSocket 析构是否会关闭
        // 原描述符」的所有权歧义，比 setSocketDescriptor(-1) 更可靠。
        const int workerFd = sourceFd >= 0 ? ::dup(sourceFd) : -1;

        socket->abort();
        socket->deleteLater();

        if (workerFd < 0) {
            emit logMessage(QStringLiteral("warning"),
                            QCoreApplication::translate("clashqt::core", "Failed to duplicate the connection descriptor; connection dropped"));
            continue;
        }

        spawnWorker(qintptr(workerFd), peer, peerPort);
    }
}

void Listener::spawnWorker(qintptr descriptor, const QHostAddress &peerAddress, quint16 peerPort)
{
    m_activeWorkers.fetch_add(1, std::memory_order_acq_rel);

    // 必须使用 QThread 而不是 std::thread：
    // QAbstractSocket 依赖 QSocketNotifier，而 QSocketNotifier 要求所在线程
    // 由 QThread 创建（QThread 会为该线程创建事件分发器）。在 std::thread
    // 中创建的 QTcpSocket 无法正常工作，连接会静默失败。
    QThread *thread = QThread::create([this, descriptor, peerAddress, peerPort]() {
        // 工作线程内重建套接字；lambda 结束时 socket 析构即关闭描述符
        QTcpSocket socket;
        if (socket.setSocketDescriptor(descriptor)) {
            socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

            Logger::instance().debug(
                LogCategory::Listener,
                QCoreApplication::translate("clashqt::core", "%1 accepted connection %2:%3").arg(m_type, peerAddress.toString()).arg(peerPort));

            if (m_accepting.load(std::memory_order_acquire)) {
                handleConnection(&socket);
            }
        } else {
            Logger::instance().warn(
                LogCategory::Listener,
                QCoreApplication::translate("clashqt::core", "Could not rebuild a socket from descriptor %1").arg(qint64(descriptor)));
        }

        socket.abort();
        m_activeWorkers.fetch_sub(1, std::memory_order_acq_rel);
    });

    // 线程结束后自释放，避免每条连接泄漏一个 QThread 对象
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

int Listener::effectiveWorkerLimit()
{
    // 每条在途连接的开销：客户端 socket 1 + 出站 socket 1 + 工作线程事件
    // 分发器的 eventfd 1，另需为 DNS 查询、日志写入等临时 fd 留余量。
    // 固定 512 会让 fd 需求（约 2000）超过常见的 1024 软上限 —— 连接突发时
    // 必然以「打开的文件过多」崩溃收尾。这里按软上限推算，并保留 200 个
    // fd 给非连接用途。
    static const int limit = [] {
        int cap   = 512; // 非 Linux 或读取失败时的兜底
        int soft  = 0;
        QFile file(QStringLiteral("/proc/self/limits"));
        if (file.open(QIODevice::ReadOnly)) {
            for (const QByteArray &line : file.readAll().split('\n')) {
                if (!line.startsWith("Max open files")) continue;
                QStringList fields;
                for (const QByteArray &part : line.split(' ')) {
                    if (!part.trimmed().isEmpty()) fields << QString::fromLatin1(part.trimmed());
                }
                soft = fields.value(3, QStringLiteral("0")).toInt();
                break;
            }
        }
        if (soft > 0) {
            const int derived = (soft - 200) / 4;
            cap = qBound(32, derived, 512);
        }
        return cap;
    }();
    return limit;
}

} // namespace core
