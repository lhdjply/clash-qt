#include "common/SocketUtil.h"
#include <QCoreApplication>

#include <QDateTime>
#include <QAbstractSocket>

#include <algorithm>

namespace core {
namespace sockutil {

QByteArray drainAvailable(QAbstractSocket *socket)
{
    if (!socket) return QByteArray();
    if (socket->bytesAvailable() <= 0) return QByteArray();
    return socket->readAll();
}

bool readUntil(QAbstractSocket *socket, QByteArray *out, const QByteArray &delimiter,
               int timeoutMs, int maxBytes, QString *error)
{
    if (!socket || !out) return false;

    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);

    while (!out->contains(delimiter)) {
        if (maxBytes > 0 && out->size() > maxBytes) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Read more than %1 bytes without finding the delimiter").arg(maxBytes);
            }
            return false;
        }

        // 关键：优先消费已缓冲的数据，否则会在无新数据时被 waitForReadyRead 卡住
        const QByteArray buffered = drainAvailable(socket);
        if (!buffered.isEmpty()) {
            out->append(buffered);
            continue;
        }

        if (socket->state() != QAbstractSocket::ConnectedState) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Connection closed");
            return false;
        }

        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Read timed out");
            return false;
        }

        const qint64 before = socket->bytesAvailable();
        if (!socket->waitForReadyRead(int(remaining))) {
            // 等待失败时再检查一次缓冲：可能恰好在返回前到达
            const QByteArray late = drainAvailable(socket);
            if (!late.isEmpty()) {
                out->append(late);
                continue;
            }
            if (before > 0) continue;
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Read failed: %1").arg(socket->errorString());
            }
            return false;
        }

        const QByteArray chunk = drainAvailable(socket);
        if (!chunk.isEmpty()) out->append(chunk);
    }
    return true;
}

bool readAtLeast(QAbstractSocket *socket, QByteArray *out, int count, int timeoutMs, QString *error)
{
    if (!socket || !out) return false;

    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);

    while (out->size() < count) {
        const QByteArray buffered = drainAvailable(socket);
        if (!buffered.isEmpty()) {
            out->append(buffered);
            continue;
        }

        if (socket->state() != QAbstractSocket::ConnectedState) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Connection closed");
            return false;
        }

        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Read timed out");
            return false;
        }

        if (!socket->waitForReadyRead(int(remaining))) {
            const QByteArray late = drainAvailable(socket);
            if (!late.isEmpty()) {
                out->append(late);
                continue;
            }
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Read failed: %1").arg(socket->errorString());
            }
            return false;
        }

        const QByteArray chunk = drainAvailable(socket);
        if (!chunk.isEmpty()) out->append(chunk);
    }
    return true;
}

bool writeAll(QAbstractSocket *socket, const QByteArray &payload, int timeoutMs, QString *error)
{
    if (!socket) return false;


    qint64       offset = 0;
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);

    while (offset < payload.size()) {
        const qint64 written = socket->write(payload.constData() + offset, payload.size() - offset);
        if (written < 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Write failed: %1").arg(socket->errorString());
            return false;
        }
        offset += written;

        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Write timed out");
            return false;
        }
        socket->waitForBytesWritten(int(remaining));
    }

    // QSslSocket 的加密输出不保证被 waitForBytesWritten 全部送出
    // （其内部会先把明文收入缓冲，再在事件循环中加密发送），必须显式 flush
    socket->flush();

    if (socket->error() == QAbstractSocket::RemoteHostClosedError
        && socket->state() != QAbstractSocket::ConnectedState) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Connection closed by the peer");
        return false;
    }
    return true;
}

} // namespace sockutil
} // namespace core
