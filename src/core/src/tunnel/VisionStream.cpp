#include "tunnel/VisionStream.h"

#include "base/Common.h"

#include <QDateTime>
#include <QRandomGenerator>

namespace core {

namespace {

QByteArray be16(int v)
{
    QByteArray out(2, 0);
    out[0] = char((v >> 8) & 0xFF);
    out[1] = char(v & 0xFF);
    return out;
}

int readBe16(const QByteArray &d, int off)
{
    if (off + 2 > d.size()) return -1;
    return (uchar(d.at(off)) << 8) | uchar(d.at(off + 1));
}

QByteArray randomBytes(int len)
{
    QByteArray out(len, 0);
    for (int i = 0; i < len; ++i) {
        out[i] = char(QRandomGenerator::global()->bounded(256));
    }
    return out;
}

// 对应 vision 包的嗅探特征
const QByteArray kTlsServerHandshakeStart("\x16\x03\x03", 3);
const QByteArray kTlsClientHandshakeStart("\x16\x03", 2);
const QByteArray kTlsApplicationDataStart("\x17\x03\x03", 3);
const QByteArray kTls13SupportedVersions("\x00\x2b\x00\x02\x03\x04", 6);

} // namespace

VisionStream::VisionStream(std::shared_ptr<RealityTlsStream> tls, const QByteArray &uuid,
                           QObject *parent)
    : OutboundStream(parent), m_tls(std::move(tls)), m_uuid(uuid)
{
    m_writeOnceUuid = m_uuid; // 首写携带 UUID

    // 告诉记录层：切换点之后的字节不是 TLS 记录，解不动时保留而不是判死
    m_tls->setAllowRawTail(true);

    QObject::connect(m_tls.get(), &OutboundStream::readyToRead, this, [this]() {
        pumpOut();
        if (!m_out.isEmpty()) emit readyToRead();
    });
    QObject::connect(m_tls.get(), &OutboundStream::streamClosed, this, [this]() {
        m_closed = true;
        emit streamClosed();
    });
}

// ---------------------------------------------------------------------------
//  ApplyPadding：构造一帧填充数据
//  帧结构：[UUID?][command(1)][contentLen(2)][paddingLen(2)][content][padding]
// ---------------------------------------------------------------------------

void VisionStream::applyPadding(QByteArray *out, uchar command, const QByteArray &content,
                                bool paddingTls)
{
    const int contentLen = content.size();
    int paddingLen = 0;
    if (contentLen < 900) {
        paddingLen = paddingTls
                         ? QRandomGenerator::global()->bounded(500) + 900 - contentLen
                         : QRandomGenerator::global()->bounded(256);
        if (paddingLen < 0) paddingLen = 0;
    }

    if (!m_writeOnceUuid.isEmpty()) {
        out->append(m_writeOnceUuid);
        m_writeOnceUuid.clear();
    }
    out->append(char(command));
    out->append(be16(contentLen));
    out->append(be16(paddingLen));
    out->append(content);
    if (paddingLen > 0) out->append(randomBytes(paddingLen));
}

// ---------------------------------------------------------------------------
//  ReshapeBuffer：大块写出按 8192 切分（优先在内层应用数据记录边界切开）
// ---------------------------------------------------------------------------

QList<QByteArray> VisionStream::reshape(const QByteArray &buffer)
{
    constexpr int kBufSize = 8192;
    const int     bufferLimit = kBufSize - kPaddingHeaderLen;
    if (buffer.size() < bufferLimit) return {buffer};

    QList<QByteArray> out;
    QByteArray rest = buffer;
    while (rest.size() >= bufferLimit) {
        int cutAt = rest.lastIndexOf(kTlsApplicationDataStart);
        if (cutAt < kPaddingHeaderLen || cutAt > bufferLimit) cutAt = kBufSize / 2;
        out.append(rest.left(cutAt));
        rest.remove(0, cutAt);
    }
    out.append(rest);
    return out;
}

// ---------------------------------------------------------------------------
//  FilterTLS：嗅探内层连接的 TLS 特征（写侧找 ClientHello，读侧找
//  ServerHello 与 supported_versions 扩展以确认 TLS 1.3）
// ---------------------------------------------------------------------------

int VisionStream::filterTls(const QByteArray &buffer)
{
    if (m_packetsToFilter <= 0) return 0;
    const int lenP = buffer.size();
    m_packetsToFilter--;

    int index = buffer.indexOf(kTlsServerHandshakeStart);
    if (index != -1) {
        if (lenP > index + 5) {
            if (uchar(buffer.at(0)) == 22 && uchar(buffer.at(1)) == 3
                && uchar(buffer.at(2)) == 3) {
                m_isTls = true;
                if (uchar(buffer.at(index + 5)) == 0x02) { // ServerHello
                    m_remainingServerHello = readBe16(buffer, index + 3) + 5;
                    m_isTls12orAbove = true;
                    if (lenP - index >= 79 && m_remainingServerHello >= 79) {
                        const int sessionIDLen = uchar(buffer.at(index + 43));
                        const int cipherOffset = index + 43 + sessionIDLen + 1;
                        if (cipherOffset + 2 <= lenP) {
                            m_cipher = quint16(readBe16(buffer, cipherOffset));
                        }
                    }
                }
            }
        }
    } else if ((index = buffer.indexOf(kTlsClientHandshakeStart)) != -1) {
        if (lenP > index + 5 && uchar(buffer.at(index + 5)) == 0x01) {
            m_isTls = true; // 内层 ClientHello
        }
    }

    if (m_remainingServerHello > 0) {
        int end = m_remainingServerHello;
        int i   = index < 0 ? 0 : index;
        if (i + end > lenP) {
            end = lenP;
            m_remainingServerHello -= (end - i);
        } else {
            m_remainingServerHello -= end;
            end += i;
        }
        const QByteArray window = buffer.mid(i, std::max(0, end - i));
        if (window.contains(kTls13SupportedVersions)) {
            // 内层为 TLS 1.3：启用 XTLS 直接拼接（CCM_8 套件除外）
            switch (m_cipher) {
            case 0x1301:
            case 0x1302:
            case 0x1303:
            case 0x1304:
                m_enableXtls = true;
                break;
            default:
                break;
            }
            m_packetsToFilter = 0;
            return index;
        }
        if (m_remainingServerHello <= 0) {
            m_packetsToFilter = 0;
            return index;
        }
    }
    return index;
}

// ---------------------------------------------------------------------------
//  写方向
// ---------------------------------------------------------------------------

qint64 VisionStream::write(const QByteArray &data)
{
    if (m_closed || !m_tls->isOpen()) return -1;

    if (m_writeFilterAppData) {
        if (data.isEmpty()) {
            // 空写：长填充隐藏 VLESS 头（paddingTLS=true）
            QByteArray frame;
            applyPadding(&frame, PaddingContinue, QByteArray(), true);
            return m_tls->write(frame) >= 0 ? 0 : -1;
        }

        filterTls(data);
        const QList<QByteArray> buffers = reshape(data);
        for (const QByteArray &buffer : buffers) {
            if (!m_writeFilterAppData) {
                // 过滤阶段已结束：Direct 之后裸写，End 之后仍走普通写（无 Vision 帧头）
                if (m_writeDirect) m_tls->writeRaw(buffer);
                else m_tls->write(buffer);
                continue;
            }

            uchar command = PaddingContinue;
            if (m_isTls && buffer.size() > 6 && buffer.left(3) == kTlsApplicationDataStart) {
                // 内层应用数据开始：结束填充；内层为 TLS 1.3 且套件合适时用 Direct
                // 通知服务端切换到「直接拼接」
                command = PaddingEnd;
                if (m_enableXtls) {
                    command       = PaddingDirect;
                    m_writeDirect = true;
                }
                m_writeFilterAppData = false;
            } else if (!m_isTls12orAbove && m_packetsToFilter <= 1) {
                // 内层不是 TLS：过滤窗口耗尽，结束填充
                command = PaddingEnd;
                m_writeFilterAppData = false;
            }

            // 关键：即便本帧携带的是 End / Direct 命令，也**必须**正常组帧 ——
            // 切换命令本身就是靠这一帧的 command 字段传达给服务端的。
            // 参考实现里 ApplyPadding 在置位 applyPadding=false 之后仍会被调用，
            // 裸写只从「再下一帧」开始。若在此处跳过组帧直接裸写，服务端仍在按
            // Vision 帧解析后续字节，数据流会被破坏：表现为内层 HTTPS 的首个
            // TLS 记录损坏（bad record mac / unexpected eof），而明文 HTTP 正常
            // （明文不进入本分支，因此掩盖了问题）。
            QByteArray frame;
            applyPadding(&frame, command, buffer, m_isTls);
            m_tls->write(frame);
        }
        return data.size();
    }

    if (m_writeDirect) return m_tls->writeRaw(data);
    return m_tls->write(data);
}

// ---------------------------------------------------------------------------
//  读方向状态机
// ---------------------------------------------------------------------------

void VisionStream::pumpOut()
{
    for (;;) {
        // 懒消费裸 VLESS 应答头（VER(1) + ADDONS_LEN(1) + ADDONS(n)）
        if (m_expectVlessHeader) {
            if (m_tlsIn.isEmpty()) {
                m_tlsIn += m_tls->readAll();
                if (m_tlsIn.isEmpty()) return;
            }
            if (m_vlessHeaderRemaining < 0) {
                if (m_tlsIn.size() < 2) return;
                m_vlessHeaderRemaining = 2 + uchar(m_tlsIn.at(1));
            }
            if (m_tlsIn.size() < m_vlessHeaderRemaining) return;
            m_tlsIn.remove(0, m_vlessHeaderRemaining);
            m_vlessHeaderRemaining = -1;
            m_expectVlessHeader    = false;
            continue;
        }
        if (m_readRaw) {
            // 直接拼接模式：原始 socket 字节直通
            m_out += m_tls->takeRaw();
            return;
        }
        // 补充输入
        if (m_tlsIn.isEmpty()) m_tlsIn += m_tls->readAll();

        if (!m_readProcess) {
            m_out += m_tlsIn;
            m_tlsIn.clear();
            return;
        }

        if (m_readRemainingContent > 0) {
            if (m_tlsIn.isEmpty()) return;
            const int n = std::min<qsizetype>(m_readRemainingContent, m_tlsIn.size());
            const QByteArray chunk = m_tlsIn.left(n);
            m_tlsIn.remove(0, n);
            m_out += chunk;
            filterTls(chunk);
            m_readRemainingContent -= n;
            continue;
        }

        if (m_readRemainingPadding > 0) {
            if (m_tlsIn.isEmpty()) return;
            const int n = std::min<qsizetype>(m_readRemainingPadding, m_tlsIn.size());
            m_tlsIn.remove(0, n);
            m_readRemainingPadding -= n;
            continue;
        }

        switch (m_readLastCommand) {
        case PaddingContinue: {
            const int need = m_readFilterUuid ? kPaddingHeaderLen
                                              : kPaddingHeaderLen - 16;
            if (m_tlsIn.size() < need) return;
            QByteArray header = m_tlsIn.left(need);
            m_tlsIn.remove(0, need);
            if (m_readFilterUuid) {
                m_readFilterUuid = false;
                if (!header.startsWith(m_uuid)) {
                    m_error = QStringLiteral("Vision 服务端回显了未知 UUID");
                    m_closed = true;
                    emit streamClosed();
                    return;
                }
                header.remove(0, 16);
            }
            m_readLastCommand      = uchar(header.at(0));
            m_readRemainingContent = readBe16(header, 1);
            m_readRemainingPadding = readBe16(header, 3);
            continue;
        }
        case PaddingEnd:
            m_readProcess = false;
            continue;
        case PaddingDirect: {
            // 取净记录层残余明文，再切原始直读。
            // 注意顺序：先把记录层已解出的明文取完，再 setRawMode；之后缓冲里
            // 解不动的尾部（内层 TLS 密文）由 takeRaw() 原样交出。
            m_out += m_tlsIn;
            m_tlsIn.clear();
            m_out += m_tls->readAll();
            m_tls->setRawMode();
            m_out += m_tls->takeRaw();
            m_readRaw     = true;
            m_readProcess = false;
            continue;
        }
        default:
            m_error  = QStringLiteral("Vision 读取到未知命令 %1").arg(m_readLastCommand);
            m_closed = true;
            emit streamClosed();
            return;
        }
    }
}

QByteArray VisionStream::readAll()
{
    pumpOut();
    const QByteArray out = m_out;
    m_out.clear();
    return out;
}

void VisionStream::closeStream()
{
    if (m_closed) return;
    m_closed = true;
    m_tls->closeStream();
    emit streamClosed();
}

QString VisionStream::errorText() const
{
    return m_error.isEmpty() ? m_tls->errorText() : m_error;
}

bool VisionStream::isOpen() const { return !m_closed && m_tls->isOpen(); }

bool VisionStream::waitForData(int timeoutMs)
{
    pumpOut();
    if (!m_out.isEmpty()) return true;
    if (!m_tls->waitForData(timeoutMs)) return false;
    pumpOut();
    return !m_out.isEmpty();
}

bool VisionStream::readExact(int n, QByteArray *out, int timeoutMs, QString *error)
{
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + std::max(1000, timeoutMs);
    while (m_out.size() < n) {
        pumpOut();
        if (m_out.size() >= n) break;
        if (m_closed || !m_tls->isOpen()) {
            if (error) *error = QStringLiteral("连接在读取应答时关闭");
            return false;
        }
        const qint64 remaining = deadline - QDateTime::currentMSecsSinceEpoch();
        if (remaining <= 0) {
            if (error) *error = QStringLiteral("读取应答超时");
            return false;
        }
        m_tls->waitForData(int(remaining));
    }
    *out = m_out.left(n);
    m_out.remove(0, n);
    return true;
}

} // namespace core
