#pragma once

// ---------------------------------------------------------------------------
//  WebSocket 出站流（承载在任意 OutboundStream 之上）
//
//  与 common/WebSocketClient.h 的 WebSocketStream（包装 QTcpSocket）不同，
//  本类把 WebSocket 分帧承载在抽象出站流上（如手工 TLS 1.3 流），用于
//  VLESS+TLS+WS 传输：SNI 与拨号地址分离的场景 QSslSocket 无法胜任。
//
//  行为对齐 RFC 6455 客户端语义：
//    · 拨号时完成升级握手（GET path + Sec-WebSocket-Key），校验 101；
//    · 客户端 → 服务端帧必须掩码；服务端 → 客户端帧不掩码；
//    · 自动应答 ping（pong）；收到 close 帧即关闭；
//    · 分帧对上层透明：write() 的数据按 ≤16KB 分帧为二进制帧，
//      readAll() 返回已解帧的应用数据。
// ---------------------------------------------------------------------------

#include "common/Tls13Client.h"
#include "tunnel/OutboundStream.h"

#include <QByteArray>
#include <QString>

namespace core {

class WsStream : public OutboundStream {
    Q_OBJECT

public:
    // 在 transport（已连接、可用）上完成 WebSocket 升级握手
    WsStream(std::shared_ptr<RealityTlsStream> transport, const QString &path,
             const QString &host, int timeoutMs, QString *error,
             QObject *parent = nullptr);

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

    // 阻塞等待可读业务数据（供拨号阶段按字节读取应答）
    bool waitForData(int timeoutMs);
    // 精确读取 n 字节（多出的部分内部缓冲）
    bool readExact(int n, QByteArray *out, int timeoutMs, QString *error);

    // 首次读取前懒消费「裸 VLESS 应答头」（VER + ADDONS_LEN + ADDONS）。
    // 服务端会把应答头缓冲到首次写出业务数据时才发送，拨号阶段同步等待会死锁。
    void setLazyVlessResponseHeader(bool enabled) override { m_expectVlessHeader = enabled; }

private:
    void   pumpOut();                       // 解帧状态机：transport → m_out
    qint64 sendFrame(uchar opcode, const QByteArray &payload);
    bool   ensureInput(int timeoutMs);      // 从 transport 拉取更多原始字节

    std::shared_ptr<RealityTlsStream> m_transport;
    QString m_path;
    QString m_host;

    QByteArray m_in;                        // 原始输入缓冲
    QByteArray m_out;                       // 已解帧输出缓冲
    QByteArray m_frame;                     // 当前帧已累计的负载
    int        m_frameNeed = -1;            // 当前帧还需的负载字节数（-1 = 未开始）
    uchar      m_frameOpcode = 0;
    bool       m_closed = false;
    bool       m_peerClosed = false;
    QString    m_error;

    bool       m_expectVlessHeader    = false; // 懒消费裸应答头
    int        m_vlessHeaderRemaining = -1;    // -1 = 尚未读到长度字节
    QByteArray m_hdrBuf;                       // 应答头跨帧边界缓冲

    void feedApplicationData(const QByteArray &data); // 解帧后 → m_out（含应答头剥离）
};

} // namespace core
