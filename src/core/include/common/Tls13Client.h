#pragma once

// ---------------------------------------------------------------------------
//  手工 TLS 1.3 客户端（REALITY 专用）
//
//  为什么不用 QSslSocket：REALITY 协议要求对 ClientHello 做两处 Qt 无法
//  触达的定制（协议见参照实现 component/tls/reality.go 与 XTLS/REALITY）：
//    1. session_id 字段必须携带 AES-GCM 认证数据（ECDH(客户端临时私钥,
//       服务器静态公钥) -> HKDF -> AES-256-GCM）；
//    2. key_share 必须使用我们自己的 X25519 密钥对，否则算不出认证密钥。
//  Qt 的 TLS 后端不开放这两处，因此基于 OpenSSL EVP 自行实现 TLS 1.3：
//    ClientHello 构造 -> ServerHello 解析 -> 密钥调度（RFC 8446 §7.1）
//    -> 握手记录解密（EE/Cert/CertVerify/Finished）-> 客户端 Finished
//    -> 应用数据记录层。
//
//  REALITY 服务端验证（对应 realityVerifier）：
//    证书叶子的公钥是 ed25519，且其 X.509 签名字段 =
//    HMAC-SHA512(HKDF(authKey), ed25519公钥)。校验通过即认为对端是
//    REALITY 服务器；否则报「认证失败」（服务端此时是借用站点本身）。
//
//  应用数据经由 Vision 流控时存在「直接拼接」模式（XTLS Vision
//  commandPaddingDirect）：此后字节不再经过 TLS 记录层，直接透传原始
//  socket。因此本类提供 setRawMode()/takeRaw()/writeRaw()。
// ---------------------------------------------------------------------------

#include "tunnel/OutboundStream.h"

#include <QByteArray>
#include <QString>

class QTcpSocket;

namespace core {

struct RealityConfig {
    QString    server;      // TCP 拨号地址（服务器真实地址）
    quint16    port = 443;
    QString    serverName;  // SNI / 借用站点域名（servername 字段）
    QByteArray publicKey;   // reality-opts.public-key（base64 解码后 32 字节）
    QByteArray shortId;     // reality-opts.short-id（≤8 字节）
    // reality=false 时为普通 TLS 1.3 客户端：session_id 不携带 REALITY 认证、
    // 不做 ed25519-HMAC 校验，改为标准证书链校验（系统根证书 + 主机名匹配，
    // 可用 skipVerify 跳过）。供 VLESS+TLS(+WS) 等普通 TLS 传输复用 —— Qt 的
    // QSslSocket 无法分离「拨号地址」与「SNI」，也无法定制握手。
    bool       reality    = true;
    bool       skipVerify = false;
};

class RealityTlsStream : public OutboundStream {
    Q_OBJECT

public:
    // 建连 + 完成 REALITY 握手；失败返回 nullptr 并给出原因
    static std::shared_ptr<RealityTlsStream> connect(const RealityConfig &config,
                                                     int timeoutMs, QString *error);
    ~RealityTlsStream() override;

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

    // 阻塞等待有可读数据（供拨号阶段按字节读取应答头使用）
    bool waitForData(int timeoutMs);
    // 精确读取 n 字节（多出的部分经后续 readAll 继续（内部缓冲）
    bool readExact(int n, QByteArray *out, int timeoutMs, QString *error);

    // 首个 readAll 前先消费「裸 VLESS 应答头」（VER + ADDONS_LEN + ADDONS）。
    // 服务端把应答头缓冲到首次写出业务数据时才发送，拨号阶段同步等待会死锁，
    // 因此统一改为首次读取时懒消费。与 VisionStream / WsStream 语义一致。
    void setLazyVlessResponseHeader(bool enabled) { m_expectVlessHeader = enabled; }

    // ---- Vision 直接拼接支持 -------------------------------------------
    // 停止 TLS 记录层解析；此后 takeRaw()/writeRaw() 直通原始 socket
    void setRawMode();
    QByteArray takeRaw();               // 读取 socket 原始字节（不解密）
    qint64     writeRaw(const QByteArray &data);
    // 记录层中是否还有已解密但未取走的明文（Vision 切直连前必须取净）
    bool hasBufferedPlaintext() const { return !m_plaintext.isEmpty(); }

private:
    explicit RealityTlsStream(QTcpSocket *socket, QObject *parent = nullptr);

public:
    // 协商后的密码套件（无内部加密细节暴露，供工具函数选择 AEAD 名称）
    enum class Cipher {
        Aes128GcmSha256,
        Aes256GcmSha384,
        ChaCha20Poly1305Sha256,
    };

private:

    // ---- 握手阶段 -------------------------------------------------------
    bool handshake(const RealityConfig &config, int timeoutMs, QString *error);
    bool buildClientHello(const RealityConfig &config, QString *error);
    bool sendRecord(uchar recordType, const QByteArray &payload, QString *error);
    bool sendEncryptedRecord(uchar recordType, const QByteArray &encryptedBody,
                             QString *error);
    bool recvRecord(uchar *recordType, QByteArray *payload, QByteArray *header,
                    int timeoutMs, QString *error);

    // ---- 记录层 ----------------------------------------------------------
    QByteArray encryptRecord(uchar recordType, const QByteArray &plaintext);
    bool       decryptAvailable();  // 从 socket 读入并解密记录到 m_plaintext

    // ---- OpenSSL 封装 ----------------------------------------------------
    struct KeyMaterial {
        QByteArray ourPriv;      // X25519 私钥 32B
        QByteArray ourPub;       // X25519 公钥 32B
        QByteArray clientRandom; // ClientHello random 32B
        QByteArray rawHello;     // ClientHello 握手消息（session_id 认证前）
        QByteArray authKey;      // HKDF 派生的 REALITY 认证密钥 32B
    };

    QTcpSocket *m_socket = nullptr;

    Cipher     m_cipher       = Cipher::Aes128GcmSha256;
    int        m_hashLen      = 32;
    int        m_keyLen       = 16;
    QByteArray m_clientHsSecret; // 客户端握手流量 secret
    QByteArray m_serverHsSecret; // 服务端握手流量 secret
    QByteArray m_clientHsKey;    // 由 secret 派生的 AEAD 密钥
    QByteArray m_clientHsIv;     // 12B
    QByteArray m_serverHsKey;
    QByteArray m_serverHsIv;
    QByteArray m_clientApKey;  // 客户端应用流量密钥
    QByteArray m_clientApIv;
    QByteArray m_serverApKey;
    QByteArray m_serverApIv;
    quint64    m_clientSeq = 0;
    quint64    m_serverSeq = 0;

    // X25519MLKEM768 混合密钥分享（新版 REALITY 服务器强制要求）
    QByteArray m_mlkemEk;      // ML-KEM-768 封装密钥 1184B（进入 ClientHello）
    QByteArray m_mlkemSk;      // ML-KEM-768 解封装密钥 2400B
    QByteArray m_mlkemXPriv;   // 混合分享尾部的 x25519 私钥 32B
    QByteArray m_mlkemXPub;    // 混合分享尾部的 x25519 公钥 32B
    QByteArray m_mlkemCt;      // 服务端封装密文 1088B（来自 ServerHello）
    QList<QByteArray> m_peerCerts; // 服务端证书链 DER（普通模式校验用）

    QByteArray m_transcript;   // 握手消息级联（CH..client Finished）
    QByteArray m_plaintext;
    // 懒消费裸 VLESS 应答头的状态（-1 表示尚未读到长度字节）
    bool       m_expectVlessHeader    = false;
    int        m_vlessHeaderRemaining = -1;
    QByteArray m_hdrBuf;    // 已解密未取走的应用数据
    QByteArray m_hsPending;    // 握手阶段跨记录的消息缓存
    QByteArray m_rawInput;     // socket 原始字节缓存
    KeyMaterial m_km;

    bool m_established = false;
    bool m_rawMode     = false;
    bool m_closed      = false;
    QString m_error;
};

} // namespace core
