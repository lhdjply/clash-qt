// ---------------------------------------------------------------------------
//  TUN 数据面的流量统计
//
//  TUN 的连接由协议栈自己拨号（FlowWorker 直接调 adapter->dial），不经过
//  Tunnel::handleStream 的 StreamRelay，因此中继里的统计钩子一次也不会触发。
//  如果协议栈不主动上报，Hub 每秒采样到的永远是 0 —— 现象就是「TUN 模式下
//  上行/下行一直是 0，切回系统代理就正常」。
//
//  这里用合成报文 + 假出站流验证：上行（客户端数据）与下行（上游回包）
//  都要出现在 sink 里，并且能累进 Tunnel 的全局计数。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QJsonArray>
#include <QJsonObject>

#include "component/dns/Resolver.h"
#include "tunnel/OutboundStream.h"
#include "tunnel/TunIpStack.h"
#include "tunnel/Tunnel.h"

using namespace core;

namespace {

void put16(QByteArray &out, quint16 value)
{
    out.append(char((value >> 8) & 0xFF)).append(char(value & 0xFF));
}

void put32(QByteArray &out, quint32 value)
{
    out.append(char((value >> 24) & 0xFF)).append(char((value >> 16) & 0xFF))
        .append(char((value >> 8) & 0xFF)).append(char(value & 0xFF));
}

QByteArray ipv4(quint32 src, quint32 dst, quint8 protocol, const QByteArray &payload)
{
    QByteArray packet;
    packet.append(char(0x45));
    packet.append(char(0));
    put16(packet, quint16(20 + payload.size()));
    put16(packet, 0x1234);
    put16(packet, 0x4000);
    packet.append(char(64));
    packet.append(char(protocol));
    put16(packet, 0);
    put32(packet, src);
    put32(packet, dst);
    packet.append(payload);
    return packet;
}

QByteArray tcp(quint16 srcPort, quint16 dstPort, quint32 seq, quint32 ack, quint8 flags,
               const QByteArray &payload = QByteArray())
{
    QByteArray segment;
    put16(segment, srcPort);
    put16(segment, dstPort);
    put32(segment, seq);
    put32(segment, ack);
    segment.append(char(5 << 4)); // 数据偏移 5（无选项）
    segment.append(char(flags));
    put16(segment, 0xFFFF); // 窗口
    put16(segment, 0);      // 校验和（协议栈不校验）
    put16(segment, 0);      // 紧急指针
    segment.append(payload);
    return segment;
}

constexpr quint8 kFlagSyn = 0x02;
constexpr quint8 kFlagAck = 0x10;
constexpr quint8 kFlagPsh = 0x08;

constexpr quint32 kClientIp = 0x0A000002; // 10.0.0.2
constexpr quint32 kServerIp = 0x01020304; // 1.2.3.4
constexpr quint16 kClientPort = 40000;
constexpr quint16 kServerPort = 80;

// 假出站流：记录上行字节，并在首次写入时回一段数据模拟服务端响应
class FakeStream : public OutboundStream {
public:
    QByteArray readAll() override
    {
        const QByteArray data = m_inbox;
        m_inbox.clear();
        return data;
    }

    qint64 write(const QByteArray &data) override
    {
        m_uploaded.append(data);
        // 首次写入后回包：TunIpStack 收到 readyToRead → 读走 → countDownload
        if (!m_replied) {
            m_replied = true;
            m_inbox   = QByteArray("world");
            emit readyToRead();
        }
        return data.size();
    }

    void closeStream() override { m_open = false; }
    QString errorText() const override { return {}; }
    bool isOpen() const override { return m_open; }

    QByteArray uploaded() const { return m_uploaded; }

private:
    QByteArray m_inbox;
    QByteArray m_uploaded;
    bool       m_replied = false;
    bool       m_open    = true;
};

} // namespace

class TestTunTraffic : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void tcpTrafficReachesSink();
    void tunnelAccumulatesTraffic();

private:
    std::shared_ptr<Resolver> m_resolver;
};

void TestTunTraffic::initTestCase()
{
    m_resolver = std::make_shared<Resolver>();
    m_resolver->applyDnsConfig(
        QJsonObject{{QStringLiteral("enhanced-mode"), QStringLiteral("fake-ip")},
                    {QStringLiteral("fake-ip-range"), QStringLiteral("198.18.0.1/16")},
                    {QStringLiteral("nameserver"), QJsonArray{QStringLiteral("223.5.5.5")}}});
}

void TestTunTraffic::tcpTrafficReachesSink()
{
    auto stream = std::make_shared<FakeStream>();

    TunIpStack stack(
        [stream](const DialContext &) {
            DialResult result;
            result.stream = stream;
            return result;
        },
        m_resolver, nullptr);

    qint64 upload = 0;
    qint64 download = 0;
    stack.setTrafficSink([&](qint64 up, qint64 down) {
        upload += up;
        download += down;
    });

    QByteArray synAck;
    int        synAckCount = 0;
    const auto read32 = [](const QByteArray &data, int offset) {
        return (quint32(quint8(data[offset])) << 24) | (quint32(quint8(data[offset + 1])) << 16)
               | (quint32(quint8(data[offset + 2])) << 8) | quint32(quint8(data[offset + 3]));
    };
    QObject::connect(&stack, &TunIpStack::packetsReady, [&](const QByteArray &data) {
        // 只看服务端 → 客户端的 TCP 段（源地址是目标 IP）
        if (quint8(data[9]) == 6 && read32(data, 12) == kServerIp) {
            synAck = data;
            ++synAckCount;
        }
    });

    // 1) 握手：SYN → 拨号（工作线程）→ SYN-ACK
    stack.onPackets(ipv4(kClientIp, kServerIp, 6, tcp(kClientPort, kServerPort, 1000, 0, kFlagSyn)));
    QTRY_VERIFY_WITH_TIMEOUT(synAckCount > 0, 5000);
    QCOMPARE(upload, qint64(0)); // SYN 本身不算业务流量

    // 2) 上行：客户端发数据 → 必须计入 upload
    const QByteArray body("hello-upload");
    stack.onPackets(ipv4(kClientIp, kServerIp, 6,
                         tcp(kClientPort, kServerPort, 1001, 0, kFlagAck | kFlagPsh, body)));

    QTRY_COMPARE_WITH_TIMEOUT(upload, qint64(body.size()), 5000);
    // 计数发生在数据面收到报文时，真正写入在连接的工作线程，稍后到
    QTRY_COMPARE_WITH_TIMEOUT(stream->uploaded(), body, 5000);

    // 3) 下行：假流回的 "world" → 必须计入 download
    QTRY_COMPARE_WITH_TIMEOUT(download, qint64(5), 5000);

    // 协议栈自身的累计值同步更新，供排查时对照
    QCOMPARE(stack.uploaded(), qint64(body.size()));
    QCOMPARE(stack.downloaded(), qint64(5));
}

void TestTunTraffic::tunnelAccumulatesTraffic()
{
    Tunnel tunnel;
    QCOMPARE(tunnel.uploadedBytes(), qint64(0));

    tunnel.addTraffic(1234, 5678);
    tunnel.addTraffic(0, -1); // 非法值忽略

    QCOMPARE(tunnel.uploadedBytes(), qint64(1234));
    QCOMPARE(tunnel.downloadedBytes(), qint64(5678));
}

QTEST_GUILESS_MAIN(TestTunTraffic)
#include "test_tun_traffic.moc"
