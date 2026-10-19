#include "tunnel/TunIpStack.h"

#include "tunnel/FlowWorker.h"
#include "tunnel/UdpWorker.h"

#include <QElapsedTimer>
#include <QThread>

#include "component/dns/DnsServer.h"
#include "component/dns/Resolver.h"
#include "tunnel/OutboundStream.h"

#include <QDateTime>
#include <QHostAddress>
#include <QRandomGenerator>
#include <QTimer>

namespace core {
namespace {

constexpr int kIpHeaderSize  = 20;
constexpr int kTcpHeaderSize = 20;
constexpr int kMaxPayload    = 1400; // 给 1500 MTU 留余量
constexpr int kFlowIdleMs    = 180000;
// UDP 会话：QUIC 的 keepalive 约 30 秒，STUN 绑定 10 分钟，取 120 秒折中
constexpr int kUdpIdleMs     = 120000;
constexpr int kMaxUdpFlows   = 256;
// 拨号期间客户端已经在发包（QUIC Initial 会重传）：缓存有限几条，避免丢握手
constexpr int kUdpPendingMax       = 16;
constexpr int kUdpPendingBytesMax  = 16384;

// UDP 会话键：客户端 (ip,port) 与目标 (ip,port) 各打包成一个 64 位值
inline UdpSessionKey udpSessionKey(quint32 clientIp, quint16 clientPort, quint32 ourIp,
                                   quint16 ourPort)
{
    return {(quint64(clientIp) << 16) | quint64(clientPort),
            (quint64(ourIp) << 16) | quint64(ourPort)};
}

constexpr quint8 kFlagFin = 0x01;
constexpr quint8 kFlagSyn = 0x02;
constexpr quint8 kFlagRst = 0x04;
constexpr quint8 kFlagPsh = 0x08;
constexpr quint8 kFlagAck = 0x10;

quint16 readU16(const quint8 *p) { return quint16((p[0] << 8) | p[1]); }
quint32 readU32(const quint8 *p)
{
    return (quint32(p[0]) << 24) | (quint32(p[1]) << 16) | (quint32(p[2]) << 8) | quint32(p[3]);
}
void writeU16(QByteArray &out, quint16 value)
{
    out.append(char((value >> 8) & 0xFF)).append(char(value & 0xFF));
}
void writeU32(QByteArray &out, quint32 value)
{
    out.append(char((value >> 24) & 0xFF)).append(char((value >> 16) & 0xFF));
    out.append(char((value >> 8) & 0xFF)).append(char(value & 0xFF));
}

// 16 位反码和（RFC 1071）；奇数长度补 0
quint32 checksumAccumulate(quint32 sum, const quint8 *data, int length)
{
    for (int i = 0; i + 1 < length; i += 2) {
        sum += quint32((data[i] << 8) | data[i + 1]);
    }
    if (length & 1) sum += quint32(data[length - 1] << 8);
    return sum;
}

quint16 foldChecksum(quint32 sum)
{
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return quint16(~sum & 0xFFFF);
}

// 目的地址是否只在本链路有意义（组播 / 广播 / 链路本地 / 未指定）。
// 这类包不该交给代理，也不该回 ICMP。
bool isLocalScopeAddress(quint32 ip)
{
    if ((ip >> 24) >= 224) return true;                       // 224.0.0.0/3：组播 + 保留 + 广播
    if (ip == 0) return true;                                 // 0.0.0.0（DHCP discover）
    if ((ip & 0xFFFF0000u) == 0xA9FE0000u) return true;       // 169.254.0.0/16 链路本地
    return false;
}

} // namespace

// ---------------------------------------------------------------------------
//  单条 TCP 流
// ---------------------------------------------------------------------------
struct TunIpStack::TunFlow {
    quint32 clientIp = 0;
    quint32 ourIp    = 0; // 客户端眼里的目标地址（可能是 fake-ip）
    quint16 clientPort = 0;
    quint16 ourPort    = 0;

    quint32 clientNext = 0; // 客户端下一个期望的序号
    quint32 ourNext    = 0; // 我们下一个要发的序号
    quint32 ourIsn     = 0;

    bool clientFin   = false;
    bool ourFin      = false;

    // 客户端通告的接收窗口，以及它已确认到我们哪个序号。
    // 发数据前必须满足：ourNext - clientAck <= clientWindow
    quint16 clientWindow = 65535;
    quint32 clientAck    = 0;
    QByteArray toClient; // 已到、但因窗口不足还没发出去的数据
    QByteArray toUpstream; // 已 ACK 给客户端、但上游还没写完的数据

    QString host; // 反查到的域名（空表示目标是字面 IP）
    qint64  lastActive = 0;

    // 拨号与上游读写跑在独立线程里（FlowWorker），数据面只持有句柄。
    // 这样 TLS/WS 握手、DoH 解析都不会阻塞其他连接。
    //
    // thread 必须是 QPointer：QThread 在 finished → deleteLater 之后**已经销毁**，
    // 而流在「上游已断、客户端还没 FIN」的状态下仍留在 m_flows 里，
    // 之后关流（客户端 FIN / 3 分钟空闲回收）再对裸指针 quit() 就是
    // use-after-free —— 表现为 TUN 开着跑一会儿突然闪退，栈上落在 malloc/free。
    QPointer<FlowWorker> worker;
    QPointer<QThread>    thread;

    bool established    = false;
    bool dialing        = false;
    bool upstreamClosed = false;
    QByteArray initialPayload; // SYN 携带的数据（TFO），连上后再发
};

// ---------------------------------------------------------------------------
//  一条 UDP 会话
//
//  UDP 没有握手，所以「会话」= 四元组 + 一个承载通道。首包到达时拨号，
//  拨号期间后续的数据报先缓存（QUIC Initial 会重传，丢首包握手就废了）。
// ---------------------------------------------------------------------------
struct TunIpStack::UdpFlow {
    quint32 clientIp   = 0;
    quint32 ourIp      = 0;
    quint16 clientPort = 0;
    quint16 ourPort    = 0;

    QString host;      // 反查到的域名（空表示字面 IP）
    QString targetKey; // "host:port" / "ip:port"，用于失败缓存
    qint64  lastActive = 0;

    bool established = false;
    bool failed      = false;

    // 与 TCP 流一致：拨号在独立线程，数据面只持有句柄
    QPointer<UdpWorker> worker;
    QPointer<QThread>   thread;

    QList<QByteArray> pending; // 拨号期间缓存的数据报
    int               pendingBytes = 0;
};

TunIpStack::TunIpStack(DialFn dialFn, std::shared_ptr<Resolver> resolver,
                   std::shared_ptr<DnsServer> dnsServer, QObject *parent)
    : QObject(parent),
      m_dial(std::move(dialFn)),
      m_resolver(std::move(resolver)),
      m_dns(std::move(dnsServer))
{
    // 逐包追踪日志默认关闭（每包一行会严重拖慢数据面并撑爆日志文件）
    m_trace = qEnvironmentVariableIsSet("CLASHQT_TUN_TRACE");

    // 空闲流清理：没有这个，长期运行会积累死流
    auto *timer = new QTimer(this);
    timer->setInterval(30000);
    QObject::connect(timer, &QTimer::timeout, this, [this]() {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        QList<quint64> stale;
        for (auto it = m_flows.constBegin(); it != m_flows.constEnd(); ++it) {
            if (now - it.value()->lastActive > kFlowIdleMs) stale.append(it.key());
        }
        for (quint64 key : stale) closeFlow(key, true, QStringLiteral("空闲超时"));

        // UDP 会话同样要回收：QUIC/STUN 结束后不会通知我们
        QList<UdpSessionKey> staleUdp;
        for (auto it = m_udpFlows.constBegin(); it != m_udpFlows.constEnd(); ++it) {
            if (now - it.value()->lastActive > kUdpIdleMs) staleUdp.append(it.key());
        }
        for (const UdpSessionKey &key : staleUdp)
            closeUdpFlow(key, QStringLiteral("空闲超时"));

        // 清理过期的失败记录
        for (auto it = m_failedTargets.begin(); it != m_failedTargets.end();) {
            if (it.value() <= now) it = m_failedTargets.erase(it);
            else ++it;
        }
    });
    timer->start();
}

TunIpStack::~TunIpStack() { closeAll(); }

int TunIpStack::flowCount() const { return int(m_flows.size()); }

void TunIpStack::closeAll()
{
    QList<QPointer<QThread>> threads;
    const QList<quint64>     keys = m_flows.keys();
    for (quint64 key : keys) {
        auto it = m_flows.find(key);
        if (it != m_flows.end() && it.value()->thread)
            threads.append(it.value()->thread);
        closeFlow(key, false, QStringLiteral("关闭 TUN"));
    }

    // UDP 会话的工作线程同样要等：它们持有拨号回调
    const QList<UdpSessionKey> udpKeys = m_udpFlows.keys();
    for (const UdpSessionKey &key : udpKeys) {
        auto it = m_udpFlows.find(key);
        if (it != m_udpFlows.end() && it.value()->thread) threads.append(it.value()->thread);
        closeUdpFlow(key, QStringLiteral("关闭 TUN"));
    }

    // 关闭时等工作线程退出（有界等待）：它们持有 Hub 注入的拨号回调，
    // 若在 Hub 析构之后仍在拨号，就会访问已释放的对象。正常情况下几十毫秒内
    // 就退出；这里总预算 1.5 秒，超时也不阻塞用户。
    QElapsedTimer budget;
    budget.start();
    for (const QPointer<QThread> &thread : threads) {
        if (thread && thread->isRunning()) {
            const qint64 left = 1500 - budget.elapsed();
            if (left <= 0) break;
            thread->wait(int(qMin<qint64>(left, 500)));
        }
    }
}

// ---------------------------------------------------------------------------
//  收包
// ---------------------------------------------------------------------------
void TunIpStack::onPackets(const QByteArray &data)
{
    if (m_inPacket) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN 收包重入，本次丢弃（避免流表被并发改写）"));
        return;
    }
    // 用 RAII 复位：下面有多处提前 return，手写会漏
    struct ResetOnExit {
        bool &flag;
        ~ResetOnExit() { flag = false; }
    } reset{m_inPacket};
    m_inPacket = true;

    const auto *bytes = reinterpret_cast<const quint8 *>(data.constData());
    int         offset = 0;

    // TUN 读出的是连续的裸 IP 包（IFF_NO_PI），按 IP 总长度逐个切分
    while (offset + kIpHeaderSize <= data.size()) {
        const quint8 *packet = bytes + offset;
        if ((packet[0] >> 4) != 4) {
            // IPv6 是常态（TUN 接口自己就有链路本地地址，mDNS/RA 也会从它发出），
            // 每包一条 warning 只会把日志刷满，降为 debug 即可。
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("TUN 收到非 IPv4 包，已丢弃（IPv6 尚未支持）"));
            return;
        }
        const int totalLength = readU16(packet + 2);
        if (totalLength < kIpHeaderSize || offset + totalLength > data.size()) return;

        handlePacket(packet, totalLength);
        offset += totalLength;
    }
}

// ---------------------------------------------------------------------------
//  流量计数
//
//  协议栈每个收发点都走这里，一是让 m_uploaded / m_downloaded 保持完整，
//  二是把字节数上报给 Hub —— TUN 的字节不经过 Tunnel::handleStream，
//  少了这一步界面上的上行/下行速度就一直是 0。
// ---------------------------------------------------------------------------
void TunIpStack::countUpload(qint64 bytes)
{
    if (bytes <= 0) return;
    m_uploaded += quint64(bytes);
    if (m_trafficSink) m_trafficSink(bytes, 0);
}

void TunIpStack::countDownload(qint64 bytes)
{
    if (bytes <= 0) return;
    m_downloaded += quint64(bytes);
    if (m_trafficSink) m_trafficSink(0, bytes);
}

void TunIpStack::handlePacket(const quint8 *data, int length)
{
    const int ihl = (data[0] & 0x0F) * 4;
    if (ihl < kIpHeaderSize || length < ihl) return;

    const quint8 protocol = data[9];
    const quint32 srcIp = readU32(data + 12);
    const quint32 dstIp = readU32(data + 16);

    // 分片包不做重组：把分片当成完整数据报转发出去，上游收到的就是垃圾。
    // QUIC/STUN 会自己探测 PMTU，正常不会分片；宁可丢掉这一片，也不污染链路。
    const quint16 fragment = readU16(data + 6);
    if ((fragment & 0x2000) != 0 || (fragment & 0x1FFF) != 0) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN 收到 IP 分片，未做重组，已丢弃"));
        return;
    }

    if (protocol == 6) {
        handleTcp(srcIp, dstIp, data + ihl, length - ihl);
        return;
    }
    if (protocol == 17) {
        handleUdp(srcIp, dstIp, data + ihl, length - ihl, data, length);
        return;
    }
    if (protocol == 1) {
        handleIcmp(srcIp, dstIp, data + ihl, length - ihl, data, length);
        return;
    }
    // 其余协议（GRE / ESP 等）：不涉及，忽略
}

// ---------------------------------------------------------------------------
//  UDP：53 端口交给 DNS 应答（fake-ip 生效的前提），其余回 ICMP 端口不可达
// ---------------------------------------------------------------------------
void TunIpStack::handleUdp(quint32 srcIp, quint32 dstIp, const quint8 *udp, int length,
                           const quint8 *originalIp, int originalLength)
{
    if (length < 8) return;

    const quint16 srcPort = readU16(udp);
    const quint16 dstPort = readU16(udp + 2);
    const int     udpLength = readU16(udp + 4); // UDP 头 + 数据
    if (udpLength < 8 || udpLength > length) return;

    // 必须**拷贝**出载荷：QByteArray::fromRawData 得到的是不拥有数据的视图，
    // 而拷贝一个 raw 数组只会共享同一个指针（Qt 对 raw 数据不做深拷贝），
    // 这块内存属于 TUN 读缓冲，下一次 ::read 就会把它覆盖掉。
    const QByteArray payload(reinterpret_cast<const char *>(udp + 8), udpLength - 8);

    // DNS 劫持：直接问 DnsServer，回包按 UDP 重新封装
    if (dstPort == 53) {
        if (!m_dns || srcPort == 0) return;

        const QByteArray reply = m_dns->handleQuery(payload, QHostAddress(srcIp));
        if (reply.isEmpty()) return;

        QByteArray udpReply;
        writeU16(udpReply, 53);      // 源端口：DNS
        writeU16(udpReply, srcPort); // 目的端口：客户端
        const int segmentLength = 8 + reply.size();
        writeU16(udpReply, quint16(segmentLength));
        writeU16(udpReply, 0); // 校验和先占位
        udpReply.append(reply);

        // UDP 校验和（IPv4 下可选，填了更严谨；为 0 表示不校验）
        quint32 sum = 0;
        const quint8 pseudo[12] = {
            quint8((dstIp >> 24) & 0xFF),  quint8((dstIp >> 16) & 0xFF),
            quint8((dstIp >> 8) & 0xFF),   quint8(dstIp & 0xFF),
            quint8((srcIp >> 24) & 0xFF),  quint8((srcIp >> 16) & 0xFF),
            quint8((srcIp >> 8) & 0xFF),   quint8(srcIp & 0xFF),
            0, 17, quint8((segmentLength >> 8) & 0xFF), quint8(segmentLength & 0xFF)};
        sum = checksumAccumulate(sum, pseudo, sizeof(pseudo));
        sum = checksumAccumulate(sum, reinterpret_cast<const quint8 *>(udpReply.constData()),
                                 udpReply.size());
        const quint16 udpChecksum = foldChecksum(sum);
        udpReply[6] = char((udpChecksum >> 8) & 0xFF);
        udpReply[7] = char(udpChecksum & 0xFF);

        // IP 头（源/目的对调）
        QByteArray packet;
        const int  totalLength = kIpHeaderSize + udpReply.size();
        packet.append(char(0x45));
        packet.append(char(0));
        writeU16(packet, quint16(totalLength));
        writeU16(packet, quint16(m_ipId++ & 0xFFFF));
        writeU16(packet, 0x4000);
        packet.append(char(64));
        packet.append(char(17));
        writeU16(packet, 0);
        writeU32(packet, dstIp);  // 我们的地址
        writeU32(packet, srcIp);  // 客户端
        const quint16 ipChecksum = foldChecksum(checksumAccumulate(
            0, reinterpret_cast<const quint8 *>(packet.constData()), kIpHeaderSize));
        packet[10] = char((ipChecksum >> 8) & 0xFF);
        packet[11] = char(ipChecksum & 0xFF);
        packet.append(udpReply);

        countDownload(qint64(packet.size()));
        emit packetsReady(packet);
        return;
    }

    // ---- 本地链路 / 组播：静默丢弃 ----
    // mDNS(5353/ff02::fb)、SSDP(1900)、LLMNR(5355)、DHCP(67/68) 这类流量只在
    // 本地网段有意义。它们进 TUN 是因为内核会从**每个** up 的接口发一份组播，
    // 而不是因为路由被劫持。这里回 ICMP 端口不可达只会让 avahi/SSDP 客户端
    // 立刻重试，日志里刷屏却没有任何收益 —— 直接丢掉即可，物理网卡上那一份
    // 照常送达。
    if (isLocalScopeAddress(dstIp)) {
        if (m_trace) {
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("TUN：本地链路/组播 UDP %1 已丢弃").arg(dstPort));
        }
        return;
    }

    // 其余 UDP（QUIC / STUN / 游戏 / WireGuard…）：交给 UDP 中继。
    // 这是 TUN 模式下 WebRTC、HTTP/3、语音通话能不能用的关键路径。
    forwardUdp(srcIp, dstIp, srcPort, dstPort, payload, originalIp, originalLength);
}

// ---------------------------------------------------------------------------
//  ICMP 目的不可达 / 端口不可达（type 3, code 3）
//  回这个包比静默丢弃好：QUIC 客户端会立即回落 TCP，而不是卡到超时
// ---------------------------------------------------------------------------
void TunIpStack::sendIcmpUnreachable(quint32 srcIp, quint32 dstIp, const quint8 *originalIp,
                                     int originalLength, quint8 code)
{
    if (originalLength < kIpHeaderSize) return;

    // 引用：原始 IP 头 + 最多 8 字节数据
    const int quoted = qMin(originalLength, kIpHeaderSize + 8);

    QByteArray icmp;
    icmp.append(char(3)); // type: destination unreachable
    icmp.append(char(code));
    writeU16(icmp, 0);    // 校验和占位
    writeU16(icmp, 0);    // 未使用
    icmp.append(reinterpret_cast<const char *>(originalIp), quoted);

    const quint16 icmpChecksum = foldChecksum(
        checksumAccumulate(0, reinterpret_cast<const quint8 *>(icmp.constData()), icmp.size()));
    icmp[2] = char((icmpChecksum >> 8) & 0xFF);
    icmp[3] = char(icmpChecksum & 0xFF);

    QByteArray packet;
    const int  totalLength = kIpHeaderSize + icmp.size();
    packet.append(char(0x45));
    packet.append(char(0));
    writeU16(packet, quint16(totalLength));
    writeU16(packet, quint16(m_ipId++ & 0xFFFF));
    writeU16(packet, 0x4000);
    packet.append(char(64));
    packet.append(char(1)); // ICMP
    writeU16(packet, 0);
    writeU32(packet, dstIp); // 源：我们（原目的地址）
    writeU32(packet, srcIp); // 目的：客户端
    const quint16 ipChecksum = foldChecksum(
        checksumAccumulate(0, reinterpret_cast<const quint8 *>(packet.constData()), kIpHeaderSize));
    packet[10] = char((ipChecksum >> 8) & 0xFF);
    packet[11] = char(ipChecksum & 0xFF);
    packet.append(icmp);

    emit packetsReady(packet);
}

// ---------------------------------------------------------------------------
//  ICMP echo
//
//  TUN 下域名被解析成 fake-ip（198.18.0.0/16），这个地址在真实网络里不存在；
//  VLESS / Hysteria2 也都不承载 ICMP（没有哪条隧道能转发 ping）。因此：
//    · fake-ip 目标：由协议栈扮演目的地回 echo reply。ping 域名能立刻得到
//      「代理链路在工作」的回应，代价是 RTT 只是本机往返（~0.1ms），不代表
//      节点质量 —— 想看节点延迟用列表里的测速按钮。
//    · 真实 IP 目标：不代答。否则 ping 8.8.8.8 也会变成 0.1ms 的假结果，
//      把真正的问题掩盖掉；这里立刻回 host unreachable，让 ping 马上结束，
//      而不是一声不响地等到超时。
// ---------------------------------------------------------------------------
void TunIpStack::handleIcmp(quint32 srcIp, quint32 dstIp, const quint8 *icmp, int length,
                            const quint8 *originalIp, int originalLength)
{
    if (length < 8) return;

    // 只处理 echo request（type 8 / code 0），别的类型（时间戳、traceroute
    // 用的超时消息等）一律不答
    if (icmp[0] != 8 || icmp[1] != 0) return;

    if (!m_resolver || !m_resolver->isFakeIp(QHostAddress(dstIp))) {
        sendIcmpUnreachable(srcIp, dstIp, originalIp, originalLength, 1); // host unreachable
        return;
    }

    // echo reply：type/code 改回 0，标识符、序号、载荷原样带回（ping 靠 id+seq 配对）
    QByteArray reply(reinterpret_cast<const char *>(icmp), length);
    reply[0] = char(0);
    reply[1] = char(0);
    reply[2] = char(0);
    reply[3] = char(0);

    const quint16 icmpChecksum = foldChecksum(checksumAccumulate(
        0, reinterpret_cast<const quint8 *>(reply.constData()), reply.size()));
    reply[2] = char((icmpChecksum >> 8) & 0xFF);
    reply[3] = char(icmpChecksum & 0xFF);

    QByteArray packet;
    const int  totalLength = kIpHeaderSize + reply.size();
    packet.append(char(0x45));
    packet.append(char(0));
    writeU16(packet, quint16(totalLength));
    writeU16(packet, quint16(m_ipId++ & 0xFFFF));
    writeU16(packet, 0x0000); // 不分片
    packet.append(char(64));  // TTL
    packet.append(char(1));   // ICMP
    writeU16(packet, 0);
    writeU32(packet, dstIp);  // 源：被 ping 的 fake-ip
    writeU32(packet, srcIp);  // 目的：客户端
    const quint16 ipChecksum = foldChecksum(
        checksumAccumulate(0, reinterpret_cast<const quint8 *>(packet.constData()), kIpHeaderSize));
    packet[10] = char((ipChecksum >> 8) & 0xFF);
    packet[11] = char(ipChecksum & 0xFF);
    packet.append(reply);

    emit packetsReady(packet);

    emit logMessage(QStringLiteral("debug"),
                    QStringLiteral("TUN：ICMP echo 由本机应答（%1，fake-ip）")
                        .arg(QHostAddress(dstIp).toString()));
}

void TunIpStack::handleTcp(quint32 srcIp, quint32 dstIp, const quint8 *tcp, int length)
{
    if (length < kTcpHeaderSize) return;

    const quint16 srcPort = readU16(tcp);
    const quint16 dstPort = readU16(tcp + 2);
    const quint32 seq     = readU32(tcp + 4);
    const quint32 ack     = readU32(tcp + 8);
    const int     dataOffset = (tcp[12] >> 4) * 4;
    const quint8  flags      = tcp[13];
    if (dataOffset < kTcpHeaderSize || length < dataOffset) return;

    // 这条 payload 会**跨线程**存活（投递给连接的工作线程），所以必须拥有自己的
    // 内存。用 fromRawData 的话，拷贝只共享同一个指针（Qt 对 raw 数组不深拷贝），
    // 而指针指向 TUN 的读缓冲 —— readAvailable() 一返回这块内存就被释放，
    // 工作线程随后读到的是已释放的堆：表现为「TUN 开着跑一会儿就闪退」，
    // 即使侥幸没崩，上传的数据也是被下一个读覆盖后的垃圾。
    const QByteArray payload(reinterpret_cast<const char *>(tcp + dataOffset),
                             length - dataOffset);

    const quint64 key = (quint64(srcIp) << 16) | quint64(srcPort);
    auto          it  = m_flows.find(key);

    // 收包日志（debug 级）：排查「包没进来 / 被丢弃」时最直接的一行
    if (m_trace) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN TCP 段 %1:%2 → %3 标志=0x%4 序号=%5 长度=%6 已知流=%7")
                            .arg(QString::number(srcPort))
                            .arg(QHostAddress(srcIp).toString())
                            .arg(dstPort)
                            .arg(flags, 2, 16, QChar('0'))
                            .arg(seq)
                            .arg(payload.size())
                            .arg(it == m_flows.end() ? 0 : 1));
    }

    // ---- 新连接 ----
    if (flags & kFlagSyn) {
        // 已有同一条流：正在拨号就忽略重传（否则会重复拨号），
        // 已建好就重发 SYN-ACK，保持幂等
        if (it != m_flows.end()) {
            FlowPtr flow = it.value();
            if (!flow->established) return;
            flow->lastActive = QDateTime::currentMSecsSinceEpoch();
            sendSegment(flow, kFlagSyn | kFlagAck, flow->ourIsn, flow->clientNext);
            return;
        }

        // TCP 上的 DNS：本步骤不支持，回 RST 让客户端回落到 UDP
        if (dstPort == 53) {
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("TUN：TCP 53 端口暂不支持，回 RST 促其回落到 UDP"));
            FlowPtr temp = std::make_shared<TunFlow>();
            temp->clientIp = srcIp;
            temp->ourIp    = dstIp;
            temp->clientPort = srcPort;
            temp->ourPort    = dstPort;
            temp->clientNext = seq + 1;
            temp->ourIsn     = QRandomGenerator::global()->generate();
            temp->ourNext    = temp->ourIsn + 1;
            temp->established = true;
            sendSegment(temp, kFlagRst | kFlagAck, temp->ourNext, temp->clientNext);
            return;
        }

        openFlow(srcIp, dstIp, srcPort, dstPort, seq, payload);
        return;
    }

    if (it == m_flows.end()) {
        // 未知流的非 SYN 报文：回 RST，避免客户端一直重传
        if (!(flags & kFlagRst)) {
            FlowPtr temp = std::make_shared<TunFlow>();
            temp->clientIp   = srcIp;
            temp->ourIp      = dstIp;
            temp->clientPort = srcPort;
            temp->ourPort    = dstPort;
            temp->clientNext = seq + quint32(payload.size());
            temp->ourIsn     = QRandomGenerator::global()->generate();
            temp->ourNext    = temp->ourIsn;
            temp->established = true;
            sendSegment(temp, kFlagRst | kFlagAck, temp->ourNext, temp->clientNext);
        }
        return;
    }

    FlowPtr flow = it.value();
    if (!flow->established) return; // 正在拨号：此时客户端还没收到 SYN-ACK
    flow->lastActive = QDateTime::currentMSecsSinceEpoch();

    // 窗口与确认号：客户端每个段都会带，及时更新
    const quint16 window = readU16(tcp + 14);
    const quint32 ackSeq = readU32(tcp + 8);
    if (flags & kFlagAck) {
        flow->clientWindow = window;
        if (qint32(ackSeq - flow->clientAck) > 0) flow->clientAck = ackSeq;
        flushToClient(flow); // 窗口可能刚被打开
    }

    // ---- RST：直接拆流 ----
    if (flags & kFlagRst) {
        closeFlow(key, false, QStringLiteral("客户端 RST"));
        return;
    }

    // ---- 数据 ----
    if (!payload.isEmpty()) {
        // 只接受「按序号衔接」的部分：TUN 链路无损有序，出现空洞说明是
        // 客户端重传，直接按已有 ACK 让它对齐即可
        if (seq == flow->clientNext) {
            // 投递给该连接的工作线程：这里只做一次队列投递，永不阻塞数据面。
            // write() 只写一部分的情况由 FlowWorker 自己重试。
            if (flow->worker && !flow->upstreamClosed) {
                const QByteArray chunk = payload;
                countUpload(qint64(chunk.size()));
                QMetaObject::invokeMethod(flow->worker, "writeData", Qt::QueuedConnection,
                                          Q_ARG(QByteArray, chunk));
            }
            flow->clientNext += quint32(payload.size());
        }
        sendSegment(flow, kFlagAck, flow->ourNext, flow->clientNext);
    }

    // ---- FIN：半关闭（不再收客户端数据，但仍把上游数据发完）----
    if (flags & kFlagFin) {
        if (seq + quint32(payload.size()) == flow->clientNext) ++flow->clientNext;
        flow->clientFin = true;
        sendSegment(flow, kFlagAck, flow->ourNext, flow->clientNext);
        if (flow->upstreamClosed) {
            closeFlow(key, false, QStringLiteral("客户端关闭且上游已断"));
        }
    }

    // ---- 收到我们对端 FIN 的 ACK ----
    if ((flags & kFlagAck) && flow->ourFin && ack == flow->ourNext) {
        closeFlow(key, false, QStringLiteral("四次挥手完成"));
    }
}

// ---------------------------------------------------------------------------
//  UDP 中继
//
//  与 TCP 的差异：
//    · 没有握手，首包到达就得拨号；拨号期间后续包先缓存（QUIC 会立刻重传）
//    · 失败缓存命中时直接回 ICMP 端口不可达 —— 让 QUIC 立刻回落 TCP，
//      WebRTC 立刻改用 TURN/TCP，而不是干等 30 秒
//    · 会话按四元组区分，空闲 120 秒回收
// ---------------------------------------------------------------------------
bool TunIpStack::targetRecentlyFailed(const QString &targetKey) const
{
    const auto it = m_failedTargets.constFind(targetKey);
    return it != m_failedTargets.constEnd()
           && it.value() > QDateTime::currentMSecsSinceEpoch();
}

void TunIpStack::forwardUdp(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                            const QByteArray &payload, const quint8 *originalIp,
                            int originalLength)
{
    const UdpSessionKey key = udpSessionKey(srcIp, srcPort, dstIp, dstPort);
    const auto         it   = m_udpFlows.find(key);

    if (it != m_udpFlows.end()) {
        const UdpFlowPtr flow = it.value();
        flow->lastActive      = QDateTime::currentMSecsSinceEpoch();

        // 会话已判失败（拨号失败 / 会话超限）：必须继续回 ICMP。
        // 这里如果只是「不处理」，客户端会一直重传（QUIC 每 1~2 秒一次），
        // 而 lastActive 每次都被刷新 —— 会话永不过期，表现为该应用的 UDP
        // 流量永久卡死。回 ICMP 才能让它立刻回落 TCP 或换路。
        if (flow->failed) {
            if (originalIp && originalLength > 0) {
                sendIcmpUnreachable(srcIp, dstIp, originalIp, originalLength, 3);
            }
            return;
        }

        if (flow->established && flow->worker) {
            countUpload(qint64(payload.size()));
            QMetaObject::invokeMethod(flow->worker, "sendDatagram", Qt::QueuedConnection,
                                      Q_ARG(QByteArray, payload));
        } else if (flow->pending.size() < kUdpPendingMax
                   && flow->pendingBytes + payload.size() <= kUdpPendingBytesMax) {
            // 拨号中：先攒着，connected 之后按到达顺序补发
            flow->pending.append(payload);
            flow->pendingBytes += payload.size();
        }
        return;
    }

    openUdpFlow(srcIp, dstIp, srcPort, dstPort, payload);

    // 拨号失败（或超出会话上限）：本次直接回 ICMP，让客户端立刻换路
    const auto created = m_udpFlows.constFind(key);
    if (created != m_udpFlows.constEnd() && created.value()->failed) {
        if (originalIp && originalLength > 0) {
            sendIcmpUnreachable(srcIp, dstIp, originalIp, originalLength, 3);
        }
    }
}

void TunIpStack::openUdpFlow(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                             const QByteArray &payload)
{
    QString host;
    QString dstIpText = QHostAddress(dstIp).toString();
    bool    fakeTarget = false;

    // fake-ip → 域名：UDP 也必须还原成域名，否则 QUIC 会去连 198.18.x.x
    if (m_resolver) {
        if (auto resolved = m_resolver->fakeIpToHost(QHostAddress(dstIp))) {
            host       = *resolved;
            fakeTarget = true;
        }
    }
    if (fakeTarget) dstIpText.clear();

    const QString targetKey =
        QStringLiteral("%1:%2").arg(host.isEmpty() ? dstIpText : host).arg(dstPort);

    auto flow        = std::make_shared<UdpFlow>();
    flow->clientIp   = srcIp;
    flow->ourIp      = dstIp;
    flow->clientPort = srcPort;
    flow->ourPort    = dstPort;
    flow->host       = host;
    flow->targetKey  = targetKey;
    flow->lastActive = QDateTime::currentMSecsSinceEpoch();
    flow->pending.append(payload);
    flow->pendingBytes = payload.size();

    const UdpSessionKey key = udpSessionKey(srcIp, srcPort, dstIp, dstPort);
    m_udpFlows.insert(key, flow);

    // 目标刚失败过（或会话已满）：不拨号，标记失败让调用方回 ICMP
    if (targetRecentlyFailed(targetKey)) {
        flow->failed = true;
        if (m_trace) {
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("TUN UDP 目标近期失败过，直接回 ICMP：%1")
                                .arg(targetKey));
        }
        return;
    }
    if (m_udpFlows.size() > kMaxUdpFlows) {
        emit logMessage(QStringLiteral("warning"),
                        QStringLiteral("TUN UDP 会话数超过 %1，拒绝新会话").arg(kMaxUdpFlows));
        flow->failed = true;
        m_failedTargets.insert(targetKey, QDateTime::currentMSecsSinceEpoch() + 30000);
        return;
    }

    Metadata metadata;
    metadata.network     = QStringLiteral("udp");
    metadata.srcIp       = QHostAddress(srcIp).toString();
    metadata.dstIp       = dstIpText;
    metadata.srcPort     = srcPort;
    metadata.dstPort     = dstPort;
    metadata.host        = host;
    metadata.inboundType = QStringLiteral("TUN");
    metadata.inboundName = QStringLiteral("tun");

    DialContext context;
    context.host     = host;
    context.dstIp    = dstIpText;
    context.dstPort  = dstPort;
    context.network  = QStringLiteral("udp");
    context.metadata = metadata;

    auto *thread = new QThread;
    thread->setObjectName(QStringLiteral("tun-udp"));
    thread->setStackSize(256 * 1024);
    auto *worker = new UdpWorker(m_dial, context);
    worker->moveToThread(thread);
    flow->worker = worker;
    flow->thread = thread;

    QObject::connect(thread, &QThread::started, worker, &UdpWorker::start);

    // 回调里额外持有 worker 的 QPointer：会话可能在拨号期间被空闲回收，
    // 那时若不主动收掉这条刚拨通的通道，线程会空转且无人回收（长期运行泄漏）。
    const QPointer<UdpWorker> guard(worker);
    QObject::connect(worker, &UdpWorker::connected, this, [this, key, guard]() {
        if (!m_udpFlows.contains(key)) {
            if (guard) {
                guard->closeChannel();
                if (QThread *owner = guard->thread()) owner->quit();
            }
            return;
        }
        onUdpConnected(key);
    });
    QObject::connect(worker, &UdpWorker::failed, this,
                     [this, key, guard](const QString &reason) {
                         if (!m_udpFlows.contains(key)) {
                             if (guard) {
                                 if (QThread *owner = guard->thread()) owner->quit();
                             }
                             return;
                         }
                         onUdpFailed(key, reason);
                     });
    QObject::connect(worker, &UdpWorker::datagram, this,
                     [this, key](const QByteArray &data) { onUdpDatagram(key, data); });
    QObject::connect(worker, &UdpWorker::closed, this, [this, key]() { onUdpClosed(key); });
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void TunIpStack::onUdpConnected(const UdpSessionKey &key)
{
    const auto it = m_udpFlows.find(key);
    if (it == m_udpFlows.end()) {
        // 会话在拨号期间已被回收（空闲清理 / 关闭 TUN）：连上也要立刻关掉
        // worker 由 closeUdpFlow 负责，这里无需额外处理
        return;
    }
    const UdpFlowPtr flow = it.value();
    flow->established     = true;
    flow->lastActive      = QDateTime::currentMSecsSinceEpoch();

    if (m_trace) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN UDP 会话建立：%1:%2 → %3")
                            .arg(QHostAddress(flow->clientIp).toString())
                            .arg(flow->clientPort)
                            .arg(flow->targetKey));
    }

    // 补发拨号期间缓存的数据报（顺序保持：QUIC 握手依赖首包顺序）
    if (flow->worker) {
        for (const QByteArray &datagram : flow->pending) {
            countUpload(qint64(datagram.size()));
            QMetaObject::invokeMethod(flow->worker, "sendDatagram", Qt::QueuedConnection,
                                      Q_ARG(QByteArray, datagram));
        }
    }
    flow->pending.clear();
    flow->pendingBytes = 0;
}

void TunIpStack::onUdpFailed(const UdpSessionKey &key, const QString &reason)
{
    const auto it = m_udpFlows.find(key);
    if (it == m_udpFlows.end()) return;

    const UdpFlowPtr flow = it.value();
    flow->failed          = true;

    emit logMessage(QStringLiteral("warning"),
                    QStringLiteral("TUN UDP 拨号失败 %1：%2").arg(flow->targetKey, reason));

    // 30 秒内同一目标直接回 ICMP：避免每个数据报都触发一次失败拨号
    m_failedTargets.insert(flow->targetKey, QDateTime::currentMSecsSinceEpoch() + 30000);

    // 保留会话（failed=true），后续包走「立即回 ICMP」的快速失败路径，
    // 直到空闲回收把它清掉。
    if (flow->thread && flow->thread->isRunning()) flow->thread->quit();
}

void TunIpStack::onUdpDatagram(const UdpSessionKey &key, const QByteArray &data)
{
    const auto it = m_udpFlows.find(key);
    if (it == m_udpFlows.end()) return;

    const UdpFlowPtr flow = it.value();
    flow->lastActive      = QDateTime::currentMSecsSinceEpoch();
    countDownload(qint64(data.size()));

    sendUdpToClient(flow->clientIp, flow->clientPort, flow->ourIp, flow->ourPort, data);
}

void TunIpStack::onUdpClosed(const UdpSessionKey &key)
{
    const auto it = m_udpFlows.find(key);
    if (it == m_udpFlows.end()) return;
    // 通道被上游关闭：清掉会话，客户端下次发包会重新拨号
    if (m_trace) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN UDP 通道关闭：%1").arg(it.value()->targetKey));
    }
    closeUdpFlow(key, QStringLiteral("上游关闭"));
}

void TunIpStack::closeUdpFlow(const UdpSessionKey &key, const QString &reason)
{
    const auto it = m_udpFlows.find(key);
    if (it == m_udpFlows.end()) return;

    const UdpFlowPtr flow = it.value();
    m_udpFlows.erase(it);

    if (flow->worker) {
        QMetaObject::invokeMethod(flow->worker, "closeChannel", Qt::QueuedConnection);
    }
    if (flow->thread && flow->thread->isRunning()) flow->thread->quit();

    if (m_trace) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN 关闭 UDP 会话 %1:%2（%3）")
                            .arg(QHostAddress(flow->clientIp).toString())
                            .arg(flow->clientPort)
                            .arg(reason));
    }
}

// 把一个数据报封装成 UDP/IP 包发回客户端（源/目的与入向对调）
void TunIpStack::sendUdpToClient(quint32 clientIp, quint16 clientPort, quint32 ourIp,
                                 quint16 ourPort, const QByteArray &payload)
{
    if (payload.isEmpty()) return;

    QByteArray udp;
    writeU16(udp, ourPort);
    writeU16(udp, clientPort);
    const int segmentLength = 8 + payload.size();
    writeU16(udp, quint16(segmentLength));
    writeU16(udp, 0); // 校验和占位
    udp.append(payload);

    // UDP 校验和（含伪首部）
    const quint8 pseudo[12] = {
        quint8((ourIp >> 24) & 0xFF),    quint8((ourIp >> 16) & 0xFF),
        quint8((ourIp >> 8) & 0xFF),     quint8(ourIp & 0xFF),
        quint8((clientIp >> 24) & 0xFF), quint8((clientIp >> 16) & 0xFF),
        quint8((clientIp >> 8) & 0xFF),  quint8(clientIp & 0xFF),
        0, 17, quint8((segmentLength >> 8) & 0xFF), quint8(segmentLength & 0xFF)};
    quint32 sum = checksumAccumulate(0, pseudo, sizeof(pseudo));
    sum = checksumAccumulate(sum, reinterpret_cast<const quint8 *>(udp.constData()), udp.size());
    quint16 udpChecksum = foldChecksum(sum);
    // UDP 里校验和 0 表示「未计算」，算出来恰好是 0 时必须写成 0xFFFF（RFC 768）
    if (udpChecksum == 0) udpChecksum = 0xFFFF;
    udp[6] = char((udpChecksum >> 8) & 0xFF);
    udp[7] = char(udpChecksum & 0xFF);

    QByteArray packet;
    const int  totalLength = kIpHeaderSize + udp.size();
    packet.append(char(0x45));
    packet.append(char(0));
    writeU16(packet, quint16(totalLength));
    writeU16(packet, quint16(m_ipId++ & 0xFFFF));
    // UDP 回包**不设 DF**：个别数据报会超过 MTU（大 DNS / 游戏数据），
    // 不设 DF 时内核能按需分片送达；设了 DF 就只能丢弃。
    writeU16(packet, 0);
    packet.append(char(64));
    packet.append(char(17));
    writeU16(packet, 0);
    writeU32(packet, ourIp);
    writeU32(packet, clientIp);
    const quint16 ipChecksum = foldChecksum(checksumAccumulate(
        0, reinterpret_cast<const quint8 *>(packet.constData()), kIpHeaderSize));
    packet[10] = char((ipChecksum >> 8) & 0xFF);
    packet[11] = char(ipChecksum & 0xFF);
    packet.append(udp);

    emit packetsReady(packet);
}

// ---------------------------------------------------------------------------
//  建流：反查域名 → 拨号 → 回 SYN-ACK
// ---------------------------------------------------------------------------
void TunIpStack::openFlow(quint32 srcIp, quint32 dstIp, quint16 srcPort, quint16 dstPort,
                        quint32 seq, const QByteArray &payload)
{
    QString host;
    QString dstIpText = QHostAddress(dstIp).toString();
    bool    fakeTarget = false;

    // fake-ip → 域名：这才是域名规则能生效的关键
    if (m_resolver) {
        if (auto resolved = m_resolver->fakeIpToHost(QHostAddress(dstIp))) {
            host       = *resolved;
            fakeTarget = true;
        }
    }

    // 目标是假 IP 时，**必须把它从规则匹配和拨号里摘掉**：
    //   · 订阅规则普遍带 `IP-CIDR,198.18.0.0/16,DIRECT,no-resolve`，
    //     假 IP 必然命中它 → 所有连接都变成「直连 198.18.0.x」，网页全打不开；
    //   · 直连一个不存在的假地址只会得到 Unknown error。
    // 这里真正的目标身份是域名；需要真实 IP 的 DIRECT 出站会在拨号前
    // 用内核解析器（resolveReal）解出，绝不走系统解析器（那会把 DNS 指回自己）。
    if (fakeTarget) dstIpText.clear();

    // 该目标刚失败过（典型：解析到不可达的私网地址）：直接 RST，不再拨号。
    // 否则客户端会按 8 秒节奏重试，每次重试都把整个数据面堵满一个拨号超时。
    const QString targetKey =
        QStringLiteral("%1:%2").arg(host.isEmpty() ? dstIpText : host).arg(dstPort);
    const qint64  nowMs     = QDateTime::currentMSecsSinceEpoch();
    const auto    failedIt  = m_failedTargets.constFind(targetKey);
    if (failedIt != m_failedTargets.constEnd() && failedIt.value() > nowMs) {
        if (m_trace) {
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("TUN 目标近期失败过，直接 RST：%1").arg(targetKey));
        }
        FlowPtr temp      = std::make_shared<TunFlow>();
        temp->clientIp    = srcIp;
        temp->ourIp       = dstIp;
        temp->clientPort  = srcPort;
        temp->ourPort     = dstPort;
        temp->clientNext  = seq + 1 + quint32(payload.size());
        temp->ourIsn      = QRandomGenerator::global()->generate();
        temp->ourNext     = temp->ourIsn + 1;
        temp->established = true;
        sendSegment(temp, kFlagRst | kFlagAck, temp->ourNext, temp->clientNext);
        return;
    }

    Metadata metadata;
    metadata.network     = QStringLiteral("tcp");
    metadata.srcIp       = QHostAddress(srcIp).toString();
    metadata.dstIp       = dstIpText;
    metadata.srcPort     = srcPort;
    metadata.dstPort     = dstPort;
    metadata.host        = host;
    metadata.inboundType = QStringLiteral("TUN");
    metadata.inboundName = QStringLiteral("tun");

    DialContext context;
    context.host     = host;
    context.dstIp    = dstIpText;
    context.dstPort  = dstPort;
    context.network  = QStringLiteral("tcp");
    context.metadata = metadata;

    auto flow          = std::make_shared<TunFlow>();
    flow->clientIp     = srcIp;
    flow->ourIp        = dstIp;
    flow->clientPort   = srcPort;
    flow->ourPort      = dstPort;
    flow->clientNext   = seq + 1 + quint32(payload.size());
    flow->ourIsn       = QRandomGenerator::global()->generate();
    flow->ourNext      = flow->ourIsn + 1; // SYN 占一个序号
    flow->clientAck    = flow->ourNext;     // 客户端即将 ACK 到这里
    flow->host         = host;
    flow->lastActive   = QDateTime::currentMSecsSinceEpoch();

    const quint64 key = (quint64(srcIp) << 16) | quint64(srcPort);
    m_flows.insert(key, flow); // 先占位：拨号期间的重传报文会被识别为同一条流

    // ---- 拨号放到独立线程 ----
    // TLS/WS 握手、DoH 解析都是同步阻塞的（300ms ~ 5s）。留在数据面线程里
    // 会让所有其他连接排队，这就是「网页很卡」的根因。这里每连接一个线程，
    // 数据面只做 TCP 状态机。线程数上限避免异常情况下爆掉。
    constexpr int kMaxFlows = 256;
    if (m_flows.size() > kMaxFlows) {
        emit logMessage(QStringLiteral("warning"),
                        QStringLiteral("TUN 并发连接超过 %1，拒绝新连接").arg(kMaxFlows));
        flow->established = true;
        sendSegment(flow, kFlagRst | kFlagAck, flow->ourNext, flow->clientNext);
        m_flows.erase(m_flows.find(key));
        return;
    }

    flow->initialPayload = payload;
    flow->dialing        = true;

    auto *thread = new QThread;
    thread->setObjectName(QStringLiteral("tun-flow"));
    thread->setStackSize(512 * 1024); // 每连接一个线程，不需要默认 8MB 栈
    auto *worker = new FlowWorker(m_dial, context);
    worker->moveToThread(thread);
    flow->worker = worker;
    flow->thread = thread;

    QObject::connect(thread, &QThread::started, worker, &FlowWorker::start);
    QObject::connect(worker, &FlowWorker::connected, this, [this, key]() { onFlowConnected(key); });
    QObject::connect(worker, &FlowWorker::failed, this,
                     [this, key](const QString &reason) { onFlowFailed(key, reason); });
    QObject::connect(worker, &FlowWorker::dataReady, this,
                     [this, key](const QByteArray &data) { onFlowData(key, data); });
    QObject::connect(worker, &FlowWorker::closed, this, [this, key]() { onFlowClosed(key); });
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

// ---------------------------------------------------------------------------
//  工作线程回调：全部在数据面线程执行，只做状态机与回包
// ---------------------------------------------------------------------------
void TunIpStack::onFlowConnected(quint64 key)
{
    auto it = m_flows.find(key);
    if (it == m_flows.end()) return;
    const FlowPtr flow = it.value();

    flow->dialing     = false;
    flow->established  = true;
    flow->lastActive   = QDateTime::currentMSecsSinceEpoch();

    emit logMessage(QStringLiteral("info"),
                    QStringLiteral("TUN 新连接 %1:%2 → %3:%4")
                        .arg(QHostAddress(flow->clientIp).toString())
                        .arg(flow->clientPort)
                        .arg(flow->host.isEmpty() ? QHostAddress(flow->ourIp).toString()
                                                  : flow->host)
                        .arg(flow->ourPort));

    // SYN-ACK 回给客户端，它才会发数据
    sendSegment(flow, kFlagSyn | kFlagAck, flow->ourIsn, flow->clientNext);

    // SYN 携带的数据（TFO）
    if (!flow->initialPayload.isEmpty() && flow->worker) {
        countUpload(qint64(flow->initialPayload.size()));
        QMetaObject::invokeMethod(flow->worker, "writeData", Qt::QueuedConnection,
                                  Q_ARG(QByteArray, flow->initialPayload));
        flow->initialPayload.clear();
    }
}

void TunIpStack::onFlowFailed(quint64 key, const QString &reason)
{
    auto it = m_flows.find(key);
    if (it == m_flows.end()) return;
    const FlowPtr flow = it.value();
    m_flows.erase(it);

    const QString target = flow->host.isEmpty() ? QHostAddress(flow->ourIp).toString() : flow->host;
    emit logMessage(QStringLiteral("warning"),
                    QStringLiteral("TUN 拨号失败 %1:%2：%3")
                        .arg(target)
                        .arg(flow->ourPort)
                        .arg(reason));

    // 记入失败缓存：30 秒内同一目标直接 RST，别让客户端反复重试
    m_failedTargets.insert(QStringLiteral("%1:%2").arg(target).arg(flow->ourPort),
                           QDateTime::currentMSecsSinceEpoch() + 30000);

    flow->established = true;
    sendSegment(flow, kFlagRst | kFlagAck, flow->ourNext, flow->clientNext);
    if (flow->thread && flow->thread->isRunning()) flow->thread->quit();
}

void TunIpStack::onFlowData(quint64 key, const QByteArray &data)
{
    auto it = m_flows.find(key);
    if (it == m_flows.end()) return;
    const FlowPtr flow = it.value();

    flow->lastActive = QDateTime::currentMSecsSinceEpoch();
    countDownload(qint64(data.size()));

    if (m_trace) {
        emit logMessage(QStringLiteral("debug"),
                        QStringLiteral("TUN 上游数据 %1 字节（待发 %2）")
                            .arg(data.size())
                            .arg(flow->toClient.size()));
    }
    flow->toClient.append(data);
    flushToClient(flow);
}

void TunIpStack::onFlowClosed(quint64 key)
{
    auto it = m_flows.find(key);
    if (it == m_flows.end()) return;
    const FlowPtr flow = it.value();

    flow->upstreamClosed = true;
    if (!flow->ourFin) {
        flow->ourFin = true;
        sendSegment(flow, kFlagFin | kFlagAck, flow->ourNext, flow->clientNext);
        flow->ourNext += 1; // FIN 占一个序号
    }
    if (flow->clientFin) closeFlow(key, false, QStringLiteral("客户端关闭且上游已断"));
    if (flow->thread && flow->thread->isRunning()) flow->thread->quit();
}

void TunIpStack::flushToClient(const FlowPtr &flow)
{
    // 可发送量 = 客户端窗口 - 我们已发但未被确认的字节数
    while (!flow->toClient.isEmpty()) {
        const quint32 unacked = flow->ourNext - flow->clientAck;
        if (unacked >= flow->clientWindow) break; // 窗口用尽：等客户端 ACK

        int available = int(flow->clientWindow - unacked);
        if (available > kMaxPayload) available = kMaxPayload;
        const QByteArray chunk = flow->toClient.left(available);
        flow->toClient.remove(0, chunk.size());

        sendSegment(flow, kFlagPsh | kFlagAck, flow->ourNext, flow->clientNext, chunk);
        flow->ourNext += quint32(chunk.size());
    }
}

// ---------------------------------------------------------------------------
//  构造并发出一个 TCP 段（含 IP 头与校验和）
// ---------------------------------------------------------------------------
void TunIpStack::sendSegment(const FlowPtr &flow, quint8 flags, quint32 seq, quint32 ack,
                           const QByteArray &payload)
{
    // ---- TCP 头 ----
    QByteArray tcp;
    tcp.reserve(kTcpHeaderSize + payload.size());
    writeU16(tcp, flow->ourPort);
    writeU16(tcp, flow->clientPort);
    writeU32(tcp, seq);
    writeU32(tcp, ack);
    tcp.append(char(quint8(5 << 4))); // 数据偏移 5（无选项）
    tcp.append(char(flags));
    writeU16(tcp, 65535); // 窗口：本地无损链路，永远开满
    writeU16(tcp, 0);     // 校验和占位
    writeU16(tcp, 0);     // 紧急指针
    tcp.append(payload);

    // ---- TCP 校验和（含伪首部）----
    quint32 sum = 0;
    {
        const quint8 pseudo[12] = {
            quint8((flow->ourIp >> 24) & 0xFF),   quint8((flow->ourIp >> 16) & 0xFF),
            quint8((flow->ourIp >> 8) & 0xFF),    quint8(flow->ourIp & 0xFF),
            quint8((flow->clientIp >> 24) & 0xFF), quint8((flow->clientIp >> 16) & 0xFF),
            quint8((flow->clientIp >> 8) & 0xFF),  quint8(flow->clientIp & 0xFF),
            0,
            6,
            quint8((tcp.size() >> 8) & 0xFF),
            quint8(tcp.size() & 0xFF),
        };
        sum = checksumAccumulate(sum, pseudo, sizeof(pseudo));
    }
    sum = checksumAccumulate(sum, reinterpret_cast<const quint8 *>(tcp.constData()), tcp.size());
    const quint16 tcpChecksum = foldChecksum(sum);
    tcp[16] = char((tcpChecksum >> 8) & 0xFF);
    tcp[17] = char(tcpChecksum & 0xFF);

    // ---- IP 头 ----
    QByteArray packet;
    const int  totalLength = kIpHeaderSize + tcp.size();
    packet.reserve(totalLength);
    packet.append(char(0x45)); // IPv4, IHL=5
    packet.append(char(0));    // TOS
    writeU16(packet, quint16(totalLength));
    writeU16(packet, quint16(m_ipId++ & 0xFFFF));
    writeU16(packet, 0x4000); // DF
    packet.append(char(64));  // TTL
    packet.append(char(6));   // TCP
    writeU16(packet, 0);      // IP 校验和占位
    writeU32(packet, flow->ourIp);
    writeU32(packet, flow->clientIp);

    const quint16 ipChecksum =
        foldChecksum(checksumAccumulate(0, reinterpret_cast<const quint8 *>(packet.constData()),
                                        kIpHeaderSize));
    packet[10] = char((ipChecksum >> 8) & 0xFF);
    packet[11] = char(ipChecksum & 0xFF);

    packet.append(tcp);
    emit packetsReady(packet);
}

void TunIpStack::closeFlow(quint64 key, bool sendRst, const QString &reason)
{
    auto it = m_flows.find(key);
    if (it == m_flows.end()) return;

    FlowPtr flow = it.value();
    m_flows.erase(it);

    if (sendRst && flow->established) {
        sendSegment(flow, kFlagRst | kFlagAck, flow->ourNext, flow->clientNext);
    }
    if (flow->worker) {
        QMetaObject::invokeMethod(flow->worker, "closeStream", Qt::QueuedConnection);
    }
    if (flow->thread && flow->thread->isRunning()) {
        // 线程退出后由 finished → deleteLater 自行回收，这里不等待，
        // 避免数据面被连接收尾拖住。QPointer 保证线程已销毁时这里为空指针。
        flow->thread->quit();
    }

    emit logMessage(QStringLiteral("debug"),
                    QStringLiteral("TUN 关闭连接 %1:%2（%3）")
                        .arg(QHostAddress(flow->clientIp).toString())
                        .arg(flow->clientPort)
                        .arg(reason));
}

} // namespace core
