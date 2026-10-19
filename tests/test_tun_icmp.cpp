// ---------------------------------------------------------------------------
//  TUN 数据面的 ICMP 行为
//
//  TUN 下域名解析成 fake-ip（198.18.0.0/16），这个地址在真实网络里不存在，
//  VLESS / Hysteria2 也没有承载 ICMP 的能力。所以数据面必须自己决定怎么
//  回应 ping：
//    · fake-ip（= ping 域名）→ 本机回 echo reply，让「代理链路在工作」可观测
//    · 真实 IP               → 回 host unreachable，立刻失败，不伪造 0.1ms
//  这里用合成报文验证这两条，外加「非 echo 的 ICMP 不回包」。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QJsonArray>
#include <QJsonObject>

#include "component/dns/Resolver.h"
#include "tunnel/TunIpStack.h"

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

quint16 read16(const QByteArray &data, int offset)
{
    return quint16((quint8(data[offset]) << 8) | quint8(data[offset + 1]));
}

quint32 read32(const QByteArray &data, int offset)
{
    return (quint32(quint8(data[offset])) << 24) | (quint32(quint8(data[offset + 1])) << 16)
           | (quint32(quint8(data[offset + 2])) << 8) | quint32(quint8(data[offset + 3]));
}

// 校验和校验：把整段（含校验和字段）累加应得 0
quint16 checksumAt(const QByteArray &data, int offset, int length)
{
    quint32 sum = 0;
    for (int i = offset; i + 1 < offset + length; i += 2) {
        sum += (quint32(quint8(data[i])) << 8) | quint8(data[i + 1]);
    }
    if (length % 2) sum += quint32(quint8(data[offset + length - 1])) << 8;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return quint16(~sum & 0xFFFF);
}

quint16 computeChecksum(const QByteArray &data, int offset, int length)
{
    quint32 sum = 0;
    for (int i = offset; i + 1 < offset + length; i += 2) {
        sum += (quint32(quint8(data[i])) << 8) | quint8(data[i + 1]);
    }
    if (length % 2) sum += quint32(quint8(data[offset + length - 1])) << 8;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return quint16(~sum & 0xFFFF);
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
    const quint16 sum = computeChecksum(packet, 0, 20);
    packet[10] = char((sum >> 8) & 0xFF);
    packet[11] = char(sum & 0xFF);
    packet.append(payload);
    return packet;
}

QByteArray icmpEcho(quint16 id, quint16 seq, const QByteArray &payload)
{
    QByteArray icmp;
    icmp.append(char(8)); // echo request
    icmp.append(char(0));
    put16(icmp, 0);
    put16(icmp, id);
    put16(icmp, seq);
    icmp.append(payload);
    const quint16 sum = computeChecksum(icmp, 0, icmp.size());
    icmp[2] = char((sum >> 8) & 0xFF);
    icmp[3] = char(sum & 0xFF);
    return icmp;
}

constexpr quint32 kClientIp = 0x0A000002; // 10.0.0.2
constexpr quint32 kRealIp   = 0x08080808; // 8.8.8.8

} // namespace

class TestTunIcmp : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void fakeIpGetsEchoReply();
    void realIpGetsHostUnreachable();
    void nonEchoIcmpIsIgnored();

private:
    std::shared_ptr<Resolver> m_resolver;
    quint32                   m_fakeIp = 0;
};

void TestTunIcmp::initTestCase()
{
    m_resolver = std::make_shared<Resolver>();
    m_resolver->applyDnsConfig(
        QJsonObject{{QStringLiteral("enhanced-mode"), QStringLiteral("fake-ip")},
                    {QStringLiteral("fake-ip-range"), QStringLiteral("198.18.0.1/16")},
                    {QStringLiteral("nameserver"), QJsonArray{QStringLiteral("223.5.5.5")}}});

    const auto fake = m_resolver->mappingFakeIp(QStringLiteral("youtube.com"));
    QVERIFY2(fake.has_value(), "fake-ip 未能分配");
    m_fakeIp = fake->toIPv4Address();
}

void TestTunIcmp::fakeIpGetsEchoReply()
{
    TunIpStack stack([](const DialContext &) { return DialResult{}; }, m_resolver, nullptr);

    QByteArray reply;
    int        count = 0;
    QObject::connect(&stack, &TunIpStack::packetsReady, [&](const QByteArray &data) {
        reply = data;
        ++count;
    });

    stack.onPackets(ipv4(kClientIp, m_fakeIp, 1, icmpEcho(0x1234, 7, QByteArray("ping-payload"))));

    QCOMPARE(count, 1);
    QCOMPARE(read32(reply, 12), m_fakeIp); // 源：被 ping 的 fake-ip
    QCOMPARE(read32(reply, 16), kClientIp);
    QCOMPARE(quint8(reply[9]), quint8(1)); // ICMP

    const int icmpOffset = 20;
    QCOMPARE(quint8(reply[icmpOffset]), quint8(0));       // echo reply
    QCOMPARE(read16(reply, icmpOffset + 4), quint16(0x1234)); // 标识符
    QCOMPARE(read16(reply, icmpOffset + 6), quint16(7));  // 序号
    QCOMPARE(reply.mid(icmpOffset + 8), QByteArray("ping-payload"));
    // 校验和字段参与累加后应为 0
    QCOMPARE(checksumAt(reply, icmpOffset, reply.size() - icmpOffset), quint16(0));
    QCOMPARE(checksumAt(reply, 0, 20), quint16(0));
}

void TestTunIcmp::realIpGetsHostUnreachable()
{
    TunIpStack stack([](const DialContext &) { return DialResult{}; }, m_resolver, nullptr);

    QByteArray reply;
    int        count = 0;
    QObject::connect(&stack, &TunIpStack::packetsReady, [&](const QByteArray &data) {
        reply = data;
        ++count;
    });

    stack.onPackets(ipv4(kClientIp, kRealIp, 1, icmpEcho(0x2222, 1, QByteArray("x"))));

    QCOMPARE(count, 1);
    QCOMPARE(quint8(reply[20]), quint8(3)); // destination unreachable
    QCOMPARE(quint8(reply[21]), quint8(1)); // host unreachable
    QCOMPARE(read32(reply, 12), kRealIp);   // 源：原目标
    QCOMPARE(checksumAt(reply, 0, 20), quint16(0));
}

void TestTunIcmp::nonEchoIcmpIsIgnored()
{
    TunIpStack stack([](const DialContext &) { return DialResult{}; }, m_resolver, nullptr);

    int count = 0;
    QObject::connect(&stack, &TunIpStack::packetsReady, [&](const QByteArray &) { ++count; });

    QByteArray icmp;
    icmp.append(char(3)); // destination unreachable（客户端发来的，不该被回）
    icmp.append(char(3));
    put16(icmp, 0);
    put32(icmp, 0);

    stack.onPackets(ipv4(kClientIp, m_fakeIp, 1, icmp));
    QCOMPARE(count, 0);
}

QTEST_GUILESS_MAIN(TestTunIcmp)
#include "test_tun_icmp.moc"
