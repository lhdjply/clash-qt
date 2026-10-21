#pragma once

// ---------------------------------------------------------------------------
//  Hysteria2 客户端（QUIC）
//
//  对应参照实现 adapter/outbound/hysteria2 与 sing-quic/hysteria2。
//
//  协议分层：
//    UDP（QUIC 数据报）
//      └─ QUIC v1 + TLS 1.3，ALPN = h3
//           ├─ HTTP/3 鉴权（POST https://hysteria/auth）
//           │    请求头 Hysteria-Auth      = 节点密码
//           │           Hysteria-CC-RX     = 声明的接收带宽（0 = 不声明）
//           │           Hysteria-Padding   = 随机填充（256~2048 字符）
//           │    应答   :status: 233 表示通过，另带 Hysteria-UDP / CC-RX
//           └─ 隧道流（每条目标的 TCP 连接一条双向流）
//                请求 0x401(varint) | 地址长度(varint) | 地址(host:port)
//                     | 填充长度(varint) | 填充
//                应答 状态(1B，0=成功) | 消息长度(varint) | 消息
//                     | 填充长度(varint) | 填充，其后即隧道载荷
//
//  实现取舍（明确的边界，不做静默降级）：
//   * QUIC 使用 OpenSSL 3.2+ 自带的 QUIC 客户端 API，不引入第三方 QUIC 库；
//     HTTP/3 只实现鉴权所需的最小子集（SETTINGS + 一次 HEADERS 请求/应答），
//     QPACK 只使用静态表 + Huffman 解码，因此声明 QPACK 动态表容量为 0。
//   * 每条入站连接独立建立一条 QUIC 会话（不复用流），因此新建连接的
//     握手开销略高于原生实现的多路复用形态；好处是对象全部归属连接自己
//     的工作线程，无跨线程共享，状态简单且互不影响。
//   * OpenSSL 3.2 尚未实现 QUIC DATAGRAM，故不支持 UDP 中继（仅 TCP）。
//     UDP 流量不会静默直连，会按「该节点不支持 UDP」明确失败。
//   * 端口跳跃（ports 区间）在每次建立连接时随机选一个端口；不做连接中迁移
//     （OpenSSL 3.2 无连接迁移 API）。服务端在该区间整段监听，功能上等效。
// ---------------------------------------------------------------------------

#include "tunnel/OutboundStream.h"

#include <openssl/types.h>

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>

#include <memory>

class QSocketNotifier;
class QTimer;

namespace core {

// 与节点配置一一对应的连接参数
struct Hysteria2Config {
    QString    server;       // 服务器地址（IP 或域名）
    quint16    port = 443;   // 主端口
    QString    portRange;    // "60000-63000"；非空时在区间内随机取端口
    QString    serverName;   // TLS SNI
    QString    password;     // Hysteria-Auth
    bool       skipVerify = false;
    QByteArray fingerprint;  // 证书 SHA-256 指纹（32 字节）；非空且未跳过校验时比对
    bool       udp = false;  // 配置是否声明 UDP（仅影响状态展示）
};

class Hysteria2Stream;

// ---------------------------------------------------------------------------
//  一条已鉴权的 QUIC 会话
//
//  线程模型：与创建它的工作线程绑定（Qt 对象亲缘），内部不加锁。
//  所有 QUIC 操作都发生在该线程内：拨号阶段用「轮询 + 驱动」的阻塞循环，
//  进入中继阶段后改用 QSocketNotifier + 定时器的事件驱动。
// ---------------------------------------------------------------------------
class Hysteria2Session : public QObject, public std::enable_shared_from_this<Hysteria2Session> {
    Q_OBJECT

public:
    // 建立 QUIC 连接并完成鉴权。失败返回空，原因写入 error。
    // retryable：失败是否值得重试（换端口再试）。鉴权被拒 / 配置缺失属于
    // 确定性失败，重试只会掩盖真实原因，故置 false。
    static std::shared_ptr<Hysteria2Session> create(const Hysteria2Config &config,
                                                   int timeoutMs, QString *error,
                                                   bool *retryable = nullptr);

    ~Hysteria2Session() override;

    // 打开一条到 addr（"host:port"）的隧道流；返回的流持有本会话的引用
    std::shared_ptr<Hysteria2Stream> openTcp(const QString &addr, int timeoutMs,
                                            QString *error);

    bool    isOpen() const { return m_open; }
    bool    serverUdpEnabled() const { return m_serverUdp; }
    QString lastError() const { return m_lastError; }

    // 驱动一次网络事件（读取 UDP 数据报、推进 QUIC 状态机），
    // 并把各流上已到达的数据取出交由流自身缓存
    void pump();

    // 把刚写入 QUIC 的数据尽量刷到网络（SSL_write 只入队，需要驱动才发出）
    void flushOutgoing();

    // 流结束时注销（由 Hysteria2Stream 析构调用）
    void detachStream(Hysteria2Stream *stream);

private:
    friend class Hysteria2Stream; // 流需要登记/注销自身

    explicit Hysteria2Session(QObject *parent = nullptr);

    bool openSocket(QString *error);
    bool handshake(const Hysteria2Config &config, int timeoutMs, QString *error);
    bool authenticate(int timeoutMs, QString *error);
    // 打开 HTTP/3 请求流并写出内容（QPACK 由调用方构造）
    SSL *openQuicStream(bool uni, const QByteArray &payload, bool conclude, QString *error);
    // 接收并读空服务端自行开启的流（控制流 / QPACK 流）
    void drainIncomingStreams();
    void startEventLoop();
    void closeNow(const QString &reason);

    SSL    *m_ssl  = nullptr;  // 连接级 SSL 对象
    // UDP 套接字（非阻塞）。Windows 的 SOCKET 是 64 位句柄，不能塞进 int，
    // 这里用 qintptr 承载两个平台；使用处再转 platform::SocketHandle。
    qintptr m_fd   = -1;
    bool    m_open = false;
    bool    m_serverUdp = false;
    bool    m_authRejected = false; // 鉴权被服务端明确拒绝（不可重试）
    QString m_lastError;
    QString m_server;          // 服务器地址（未解析）
    quint16 m_peerPort = 0;    // 实际使用的端口（含端口跳跃结果）
    QString m_portRange;       // "60000-63000"
    QString m_password;        // 鉴权密码

    QList<Hysteria2Stream *> m_streams;
    QSocketNotifier *m_notifier = nullptr;
    QTimer          *m_timer    = nullptr;
};

// ---------------------------------------------------------------------------
//  承载在 QUIC 流上的出站流
// ---------------------------------------------------------------------------
class Hysteria2Stream : public OutboundStream {
    Q_OBJECT

public:
    Hysteria2Stream(std::shared_ptr<Hysteria2Session> session, SSL *stream,
                    QObject *parent = nullptr);
    ~Hysteria2Stream() override;

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

    // 会话收到 QUIC 数据后调用：取出本流上已到达的字节并缓存
    void pullFromQuic();
    // dial 阶段已从流中读出、属于隧道载荷的字节
    void prependPlaintext(const QByteArray &data);
    // 会话异常断开
    void markBroken(const QString &reason);

private:
    std::shared_ptr<Hysteria2Session> m_session;
    SSL       *m_stream = nullptr;
    QByteArray m_in;             // 已解密、待中继层取走的载荷
    bool       m_closed = false;
    bool       m_eof    = false;
    QString    m_error;
};

} // namespace core
