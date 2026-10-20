#include "tunnel/WsStream.h"
#include <QCoreApplication>

#include "common/Tls13Client.h"

#include "base/Common.h"

#include <QDateTime>
#include <QRandomGenerator>

#include <algorithm>

namespace core {

namespace {

constexpr uchar kOpcodeBinary = 0x2;
constexpr uchar kOpcodeClose = 0x8;
constexpr uchar kOpcodePing = 0x9;
constexpr uchar kOpcodePong = 0xA;

QByteArray be16Bytes(int v)
{
    QByteArray out(2, 0);
    out[0] = char((v >> 8) & 0xFF);
    out[1] = char(v & 0xFF);
    return out;
}

// 客户端帧：FIN + 二进制/ping/pong，掩码位必须为 1
QByteArray encodeClientFrame(uchar opcode, const QByteArray &payload)
{
    QByteArray frame;
    frame.append(char(0x80 | opcode));
    const int len = payload.size();
    if (len < 126) {
        frame.append(char(0x80 | len));
    } else if (len <= 0xFFFF) {
        frame.append(char(0x80 | 126));
        frame.append(be16Bytes(len));
    } else {
        frame.append(char(0x80 | 127));
        QByteArray ext(8, 0);
        quint64 v = quint64(len);
        for (int i = 7; i >= 0; --i) {
            ext[i] = char(v & 0xFF);
            v >>= 8;
        }
        frame.append(ext);
    }
    QByteArray mask(4, 0);
    for (int i = 0; i < 4; ++i) {
        mask[i] = char(QRandomGenerator::global()->bounded(256));
    }
    frame.append(mask);
    frame.append(payload);
    for (int i = 0; i < len; ++i) {
        frame[frame.size() - len + i] = char(frame.at(frame.size() - len + i) ^ mask[i % 4]);
    }
    return frame;
}

} // namespace

WsStream::WsStream(std::shared_ptr<RealityTlsStream> transport, const QString &path,
                   const QString &host, int timeoutMs, QString *error, QObject *parent)
    : OutboundStream(parent), m_transport(std::move(transport)), m_path(path), m_host(host)
{
    // ---- 升级握手 ---------------------------------------------------------
    const QString key = QString::fromLatin1(
        []() {
            QByteArray raw(16, 0);
            for (int i = 0; i < 16; ++i) {
                raw[i] = char(QRandomGenerator::global()->bounded(256));
            }
            return raw.toBase64();
        }());

    QByteArray request;
    request.append("GET " + m_path.toUtf8() + " HTTP/1.1\r\n");
    request.append("Host: " + m_host.toUtf8() + "\r\n");
    request.append("Upgrade: websocket\r\n");
    request.append("Connection: Upgrade\r\n");
    request.append("Sec-WebSocket-Key: " + key.toLatin1() + "\r\n");
    request.append("Sec-WebSocket-Version: 13\r\n\r\n");

    if (m_transport->write(request) < 0) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to send the WebSocket upgrade request");
        m_closed = true;
        return;
    }

    QByteArray head;
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);
    while (!head.contains("\r\n\r\n")) {
        // 已有缓冲则先消费（连接可能一次性把响应都发来）
        const QByteArray chunk = m_transport->readAll();
        if (!chunk.isEmpty()) {
            head += chunk;
            continue;
        }
        if (!m_transport->isOpen()) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::core", "Connection closed during the WebSocket upgrade (received %1 bytes: %2)")
                             .arg(head.size())
                             .arg(QString::fromLatin1(head.left(120)));
            }
            m_closed = true;
            return;
        }
        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "WebSocket upgrade handshake timed out");
            m_closed = true;
            return;
        }
        if (!m_transport->waitForData(int(remaining))) continue;
        head += m_transport->readAll();
    }
    const int headerEnd = head.indexOf("\r\n\r\n");
    m_in = head.mid(headerEnd + 4);
    head.truncate(headerEnd + 4);

    const QByteArray statusLine = head.left(head.indexOf("\r\n"));
    if (!statusLine.contains("101")) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::core", "WebSocket handshake rejected: %1")
                         .arg(QString::fromLatin1(statusLine));
        }
        m_closed = true;
        return;
    }

    QObject::connect(m_transport.get(), &OutboundStream::readyToRead, this, [this]() {
        pumpOut();
        if (!m_out.isEmpty()) emit readyToRead();
    });
    QObject::connect(m_transport.get(), &OutboundStream::streamClosed, this, [this]() {
        m_closed = true;
        emit streamClosed();
    });
}

void WsStream::pumpOut()
{
    for (;;) {
        if (m_closed || m_peerClosed) return;
        if (m_frameNeed < 0) {
            // 需要新的帧头：至少 2 字节
            if (m_in.size() < 2 && !ensureInput(0)) return;
            const uchar b0 = uchar(m_in.at(0));
            const uchar b1 = uchar(m_in.at(1));
            if (b1 & 0x80) { // 服务端帧不允许掩码
                m_error  = QCoreApplication::translate("clashqt::core", "Server-sent WebSocket frames must not be masked");
                m_closed = true;
                emit streamClosed();
                return;
            }
            m_frameOpcode = b0 & 0x0F;
            const bool fin = (b0 & 0x80) != 0;
            Q_UNUSED(fin);
            const int len7 = b1 & 0x7F;
            int headerLen = 2;
            qint64 payloadLen = len7;
            if (len7 == 126) {
                headerLen = 4;
            } else if (len7 == 127) {
                headerLen = 10;
            }
            if (m_in.size() < headerLen && !ensureInput(0)) return;
            if (len7 == 126) {
                payloadLen = (uchar(m_in.at(2)) << 8) | uchar(m_in.at(3));
            } else if (len7 == 127) {
                payloadLen = 0;
                for (int i = 2; i < 10; ++i) {
                    payloadLen = (payloadLen << 8) | uchar(m_in.at(i));
                }
                if (payloadLen > 32 * 1024 * 1024) {
                    m_error  = QCoreApplication::translate("clashqt::core", "WebSocket frame too long");
                    m_closed = true;
                    emit streamClosed();
                    return;
                }
            }
            m_in.remove(0, headerLen);
            m_frameNeed = int(payloadLen);
            m_frame.clear();

            // 控制帧就地处理（ping/pong/close）
            if (m_frameOpcode == kOpcodePing || m_frameOpcode == kOpcodeClose) {
                if (m_frameNeed > 0 && m_in.size() < m_frameNeed && !ensureInput(0)) return;
                if (m_frameOpcode == kOpcodePing) {
                    sendFrame(kOpcodePong, m_in.left(m_frameNeed));
                } else {
                    m_peerClosed = true;
                    m_closed = true;
                    emit streamClosed();
                    return;
                }
                m_in.remove(0, m_frameNeed);
                m_frameNeed = -1;
                continue;
            }
            continue;
        }

        // 已有帧头：继续累计负载
        if (m_frameNeed > 0) {
            if (m_in.isEmpty() && !ensureInput(0)) return;
            const int n = std::min<qsizetype>(m_frameNeed, m_in.size());
            if (n <= 0) return;
            m_frame += m_in.left(n);
            m_in.remove(0, n);
            m_frameNeed -= n;
            if (m_frameNeed > 0) return;
        }

        feedApplicationData(m_frame);
        m_frame.clear();
        m_frameNeed = -1;
    }
}

void WsStream::feedApplicationData(const QByteArray &data)
{
    if (!m_expectVlessHeader) {
        m_out += data;
        return;
    }
    QByteArray buf = m_hdrBuf + data;
    m_hdrBuf.clear();
    if (m_vlessHeaderRemaining < 0) {
        if (buf.size() < 2) {
            m_hdrBuf = buf;
            return;
        }
        m_vlessHeaderRemaining = 2 + uchar(buf.at(1));
    }
    if (buf.size() < m_vlessHeaderRemaining) {
        m_hdrBuf = buf;
        return;
    }
    buf.remove(0, m_vlessHeaderRemaining);
    m_vlessHeaderRemaining = -1;
    m_expectVlessHeader    = false;
    m_out += buf;
}

bool WsStream::ensureInput(int timeoutMs)
{
    if (!m_transport->isOpen()) return false;
    const QByteArray data = m_transport->readAll();
    if (!data.isEmpty()) {
        m_in += data;
        return true;
    }
    if (timeoutMs > 0) {
        return m_transport->waitForData(timeoutMs);
    }
    return false;
}

QByteArray WsStream::readAll()
{
    pumpOut();
    const QByteArray out = m_out;
    m_out.clear();
    return out;
}

qint64 WsStream::sendFrame(uchar opcode, const QByteArray &payload)
{
    if (m_closed || !m_transport->isOpen()) return -1;
    return m_transport->write(encodeClientFrame(opcode, payload));
}

qint64 WsStream::write(const QByteArray &data)
{
    if (data.isEmpty()) return 0;
    // 按 16KB 分帧；首帧 FIN 即可（数据一次性给出）
    qint64 total = 0;
    int offset = 0;
    while (offset < data.size()) {
        const int chunk = std::min(16384, int(data.size() - offset));
        if (sendFrame(kOpcodeBinary, data.mid(offset, chunk)) < 0) return -1;
        offset += chunk;
        total += chunk;
    }
    return total;
}

void WsStream::closeStream()
{
    if (m_closed) return;
    sendFrame(kOpcodeClose, QByteArray());
    m_transport->closeStream();
    m_closed = true;
    emit streamClosed();
}

QString WsStream::errorText() const
{
    return m_error.isEmpty() ? m_transport->errorText() : m_error;
}

bool WsStream::isOpen() const { return !m_closed && m_transport->isOpen(); }

bool WsStream::waitForData(int timeoutMs)
{
    pumpOut();
    if (!m_out.isEmpty()) return true;
    if (!m_transport->waitForData(timeoutMs)) return false;
    pumpOut();
    return !m_out.isEmpty();
}

bool WsStream::readExact(int n, QByteArray *out, int timeoutMs, QString *error)
{
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);
    while (m_out.size() < n) {
        pumpOut();
        if (m_out.size() >= n) break;
        if (m_closed || !m_transport->isOpen()) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Connection closed while reading");
            return false;
        }
        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QCoreApplication::translate("clashqt::core", "Read timed out");
            return false;
        }
        m_transport->waitForData(int(remaining));
    }
    *out = m_out.left(n);
    m_out.remove(0, n);
    return true;
}

} // namespace core
