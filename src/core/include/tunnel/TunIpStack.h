#pragma once

// ---------------------------------------------------------------------------
//  TUN 用户态协议栈（IPv4 + TCP 最小实现）
//
//  TUN 设备只把原始 IP 包丢给我们，内核不会帮忙处理 TCP，所以要在这里
//  自己实现「收包 → 认流 → 拨号 → 双向搬运 → 回包」这条链路。
//
//  实现范围（够 proxy 客户端用）：
//    * IPv4 头 / TCP 段解析，TCP 段构造与校验和（伪首部）
//    * 每个 TCP 流一个状态机：SYN → 拨号 → 数据双向转发 → FIN
//    * 目标是 fake-ip 时反查域名，交给规则匹配 —— 于是域名规则照常生效
//    * 上游关闭时给客户端回 FIN；客户端先 FIN 时只做半关闭（继续收上游数据）
//
//  明确不做（都是「本地无损链路」下可以省的）：
//    * 拥塞控制 / 重传 / SACK / 窗口缩放：TUN 是点对点无损且有序的，
//      **但客户端窗口必须遵守** —— 超窗口发送会被客户端内核直接丢掉，
//      而我们不重传，表现为大流量（视频）卡死
//      收到即接受并立刻 ACK；不做重传也不会丢包
//    * 分片重组：按 MTU 切段发送，正常不会出现分片；收到的分片直接丢弃
//    * IPv6：暂不支持
//    * UDP 中继：按四元组建会话，首包触发拨号（DIRECT / VLESS CMD=0x02），
//      拨号在独立线程完成，数据面只做封装。会话空闲 120 秒回收；
//      拨号失败的目标 30 秒内直接回 ICMP，促使 QUIC 立刻回落 TCP
//    * TCP 上的 DNS：回 RST 让客户端回落到 UDP（stub resolver 本来就走 UDP）
//    * ICMP：fake-ip 目标（也就是 ping 域名）由本机回 echo reply；真实 IP
//      回 host unreachable。没有哪条隧道能承载 ICMP，所以 ping 的 RTT 只是
//      本机往返，不能当作节点质量指标
//
//  UDP 53 端口是**劫持**的：直接交给 DnsServer 应答（fake-ip 因此生效），
//  这是 TUN 模式下名字解析能工作的前提
//
//  拨号通过构造时注入的 DialFn 完成，栈本身不依赖规则与适配器 —— 便于
//  在没有 TUN 设备、没有 root 的环境里用合成报文做自检。
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

#include <QByteArray>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QString>

#include <functional>
#include <memory>
#include <utility>

class QThread; // 全局作用域前向声明（不能在 namespace core 里声明）

namespace core {

class DnsServer;
class FlowWorker;
class OutboundStream;
class Resolver;

// UDP 会话键：(客户端 IP+端口, 目标 IP+端口) 各打包成一个 64 位值。
// UDP 无连接状态，同一个 socket 对不同目标也是不同的「会话」。
using UdpSessionKey = std::pair<quint64, quint64>;

class TunIpStack : public QObject {
    Q_OBJECT

public:
    // 拨号回调：Hub 传入「规则派发 + adapter->dial」的组合
    using DialFn = std::function<DialResult(const DialContext &)>;

    TunIpStack(DialFn dialFn, std::shared_ptr<Resolver> resolver,
             std::shared_ptr<DnsServer> dnsServer, QObject *parent = nullptr);
    ~TunIpStack() override;

    // TUN 侧读到的数据：可以是多个连续 IP 包，按 IP 头里的总长度切分
    void onPackets(const QByteArray &data);

    int    flowCount() const;
    qint64 uploaded() const { return m_uploaded; }
    qint64 downloaded() const { return m_downloaded; }

    // 流量计数出口。TUN 的连接不经过 Tunnel::handleStream（协议栈自己拨号），
    // 所以字节数必须由这里主动上报，否则 Hub 的采样永远是 0。
    // 回调在数据面线程里同步执行，实现方只应做原子累加这类无阻塞操作。
    using TrafficSink = std::function<void(qint64 upload, qint64 download)>;
    void setTrafficSink(TrafficSink sink) { m_trafficSink = std::move(sink); }

    // 关闭全部流（停用 TUN 时调用）
    void closeAll();

    // 是否正在处理包。拨号 / DNS 内部有局部事件循环，排队的 shutdown() 会在
    // 那个嵌套循环里被执行；此时若析构协议栈，等于在它自己的调用栈上 delete
    // 自己 —— 返回时必崩。关闭路径必须先查这个。
    bool busy() const { return m_inPacket; }

signals:
    void packetsReady(const QByteArray &data); // 需要写回 TUN 的 IP 包
    void logMessage(const QString &level, const QString &message);

private:
    struct TunFlow;
    using FlowPtr = std::shared_ptr<TunFlow>;

    // ---- UDP 会话（无连接，按四元组区分）----
    struct UdpFlow;
    using UdpFlowPtr = std::shared_ptr<UdpFlow>;

    void handlePacket(const quint8 *data, int length);
    void handleTcp(quint32 srcIp, quint32 dstIp, const quint8 *tcp, int length);
    void handleUdp(quint32 srcIp, quint32 dstIp, const quint8 *udp, int length,
                   const quint8 *originalIp, int originalLength);
    // ICMP：echo request 由本机代答（仅 fake-ip 目标），其余回 host unreachable
    void handleIcmp(quint32 srcIp, quint32 dstIp, const quint8 *icmp, int length,
                    const quint8 *originalIp, int originalLength);
    // 回一条 ICMP 不可达（type 3，code 由调用方给；把原始 IP 头 + 8 字节带回，
    // 符合 RFC 792）
    void sendIcmpUnreachable(quint32 srcIp, quint32 dstIp, const quint8 *originalIp,
                             int originalLength, quint8 code);
    void openFlow(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                  quint32 seq, const QByteArray &payload);

    // ---- UDP 中继 ----
    // 把客户端的数据报交给对应会话（必要时新建会话并在工作线程里拨号）
    void forwardUdp(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                    const QByteArray &payload, const quint8 *originalIp, int originalLength);
    void openUdpFlow(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                     const QByteArray &payload);
    void closeUdpFlow(const UdpSessionKey &key, const QString &reason);
    // 把上游回包封装成 UDP/IP 包发回客户端
    void sendUdpToClient(quint32 clientIp, quint16 clientPort, quint32 ourIp, quint16 ourPort,
                         const QByteArray &payload);
    // 目标近期失败过（避免每包重拨一次）
    bool targetRecentlyFailed(const QString &targetKey) const;

    // ---- 工作线程回调（全部通过队列信号进入数据面，不阻塞）----
    void onFlowConnected(quint64 key);
    void onFlowFailed(quint64 key, const QString &reason);
    void onFlowData(quint64 key, const QByteArray &data);
    void onFlowClosed(quint64 key);

    void onUdpConnected(const UdpSessionKey &key);
    void onUdpFailed(const UdpSessionKey &key, const QString &reason);
    void onUdpDatagram(const UdpSessionKey &key, const QByteArray &data);
    void onUdpClosed(const UdpSessionKey &key);

    // 按客户端当前可用窗口把排队数据发出去（窗口为 0 时停手）
    void flushToClient(const FlowPtr &flow);
    void sendSegment(const FlowPtr &flow, quint8 flags, quint32 seq, quint32 ack,
                     const QByteArray &payload = QByteArray());

    // 计数 + 上报。所有收发字节都从这里过，避免某条路径漏报。
    void countUpload(qint64 bytes);
    void countDownload(qint64 bytes);
    void closeFlow(quint64 key, bool sendRst, const QString &reason);

    DialFn                    m_dial;
    std::shared_ptr<Resolver> m_resolver;
    std::shared_ptr<DnsServer> m_dns;

    QHash<quint64, FlowPtr> m_flows;
    QHash<UdpSessionKey, UdpFlowPtr> m_udpFlows;
    // 近期失败过的目标（"host:port" -> 过期时间）。同一目标再次握手时直接 RST，
    // 避免客户端每 8 秒重试一次、每次都把数据面堵满一次拨号超时。
    QHash<QString, qint64>  m_failedTargets;
    quint64                 m_uploaded   = 0;
    quint64                 m_downloaded = 0;
    TrafficSink             m_trafficSink;
    quint32                 m_ipId       = 1;
    // 重入保护：处理一包时可能进入带局部事件循环的调用（DNS 真实解析 /
    // 出站拨号），若这期间又收到包就会同时改写流表与 fake-ip 池。
    // 外层（TunDevice）已关 notifier，这里再兜一层。
    bool                    m_inPacket   = false;
    // 逐包调试日志（每包一行会显著拖慢数据面）：仅在设置环境变量
    // CLASHQT_TUN_TRACE=1 时开启，排查时再打开。
    bool                    m_trace      = false;
};

} // namespace core
