#pragma once

// ---------------------------------------------------------------------------
//  XTLS Vision 流控（flow = xtls-rprx-vision）
//
//  对应参照实现 transport/vless/vision/（conn.go / padding.go / filter.go，
//  与 Xray-core proxy.go 同源）。语义：
//
//  写方向：
//    · 首写携带 21 字节填充头（UUID + command + contentLen + paddingLen），
//      隐藏 VLESS 请求头；之后按命令逐帧填充；
//    · FilterTLS 在写出的数据里嗅探内层 TLS ClientHello（浏览器经代理
//      发起的 TLS 连接）；一旦发现内层应用数据记录（17 03 03 ...）且已
//      确认内层为 TLS 1.3，则发 commandPaddingDirect 并切换为「直接拼接」
//      —— 此后字节不再经过 TLS 加密，直接透传原始 socket（XTLS 的
//      核心收益：避免 TLS in TLS 双重加密）；
//    · 内层不是 TLS 时，packetsToFilter 耗尽后发 commandPaddingEnd，
//      回到普通 TLS 加密通道。
//
//  读方向：
//    · 首帧校验 UUID（服务端回显）；
//    · commandPaddingEnd 后回到普通 TLS 读取；
//    · commandPaddingDirect 时先取净 TLS 记录层已解密的残余明文，再切换
//      到原始 socket 直读（对应 Go 侧排空 tlsConn.input/rawInput）。
// ---------------------------------------------------------------------------

#include "common/Tls13Client.h"
#include "tunnel/OutboundStream.h"

#include <QByteArray>

namespace core {

class VisionStream : public OutboundStream {
    Q_OBJECT

public:
    // tls：已完成 REALITY 握手的底层流；uuid：16 字节用户 ID
    VisionStream(std::shared_ptr<RealityTlsStream> tls, const QByteArray &uuid,
                 QObject *parent = nullptr);

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

    // 阻塞等待有可读业务数据（供拨号阶段读取 VLESS 应答头）
    bool waitForData(int timeoutMs);

    // 精确读取 n 字节（内部缓冲多出的部分，供后续 readAll 继续消费）
    bool readExact(int n, QByteArray *out, int timeoutMs, QString *error);

    // 首个 readAll 前先消费「裸 VLESS 应答头」（VER+ADDONS_LEN+ADDONS）。
    // 服务端会把应答头缓冲到首次写出业务数据时才发送，因此不能在拨号阶段
    // 同步等待，否则与「先发请求体」的中继流程互相死锁。
    void setLazyVlessResponseHeader(bool enabled) { m_expectVlessHeader = enabled; }

private:
    enum : uchar {
        PaddingContinue = 0x00,
        PaddingEnd      = 0x01,
        PaddingDirect   = 0x02,
    };
    static constexpr int kPaddingHeaderLen = 21; // UUID(16) + cmd(1) + len(2) + padding(2)

    void applyPadding(QByteArray *out, uchar command, const QByteArray &content,
                      bool paddingTls);
    QList<QByteArray> reshape(const QByteArray &buffer);
    int  filterTls(const QByteArray &buffer);
    void pumpOut();

    std::shared_ptr<RealityTlsStream> m_tls;
    QByteArray m_uuid;

    // ---- 写方向状态 -------------------------------------------------------
    bool       m_writeFilterAppData = true;
    bool       m_writeDirect        = false;
    QByteArray m_writeOnceUuid;      // 首写携带，用后即清

    // ---- FilterTLS 共享状态（读写两侧同一函数） ---------------------------
    bool    m_isTls             = false;
    bool    m_isTls12orAbove    = false;
    bool    m_enableXtls        = false;
    quint16 m_cipher            = 0;
    int     m_packetsToFilter   = 8;
    int     m_remainingServerHello = 0;

    // ---- 读方向状态 -------------------------------------------------------
    int        m_readRemainingContent = 0;
    int        m_readRemainingPadding = 0;
    bool       m_readProcess          = true;
    bool       m_readFilterUuid       = true;
    bool       m_readRaw              = false;
    bool       m_expectVlessHeader    = false; // 懒消费裸应答头
    int        m_vlessHeaderRemaining = -1;    // -1 = 尚未读到长度字节
    uchar      m_readLastCommand      = PaddingContinue;
    QByteArray m_tlsIn;               // 从 TLS 流取出、待状态机处理的字节
    QByteArray m_out;                 // 状态机输出、待调用方取走

    bool    m_closed = false;
    QString m_error;
};

} // namespace core
