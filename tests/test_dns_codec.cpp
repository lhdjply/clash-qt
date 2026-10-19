// ---------------------------------------------------------------------------
//  DNS 报文编解码
//
//  这段代码跑在 TUN 数据面的每一条 DNS 查询上，出错的表现是「网页时好时坏」，
//  极难从日志定位。因此这里覆盖：
//    · 查询报文的结构（头部位、名字编码）
//    · 查询 → 解析 → 应答 → 再解析 的闭环
//    · 名字压缩指针（应答段用 0xC00C 回指问题名，解析侧必须跟得上）
//    · 恶意/损坏报文不能死循环、不能越界（指针成环是经典崩溃点）
// ---------------------------------------------------------------------------

#include <QtTest>

#include "component/dns/Resolver.h"

using namespace core;

namespace {

QByteArray makeHeader(quint16 id, quint16 flags, quint16 qd, quint16 an)
{
    QByteArray out;
    const auto append = [&out](quint16 value) {
        out.append(char((value >> 8) & 0xFF));
        out.append(char(value & 0xFF));
    };
    append(id);
    append(flags);
    append(qd);
    append(an);
    append(0); // NSCOUNT
    append(0); // ARCOUNT
    return out;
}

} // namespace

class TestDnsCodec : public QObject
{
    Q_OBJECT

private slots:
    void queryStructure();
    void queryRoundTrip();
    void queryRejectsBadName();
    void responseRoundTrip();
    void responseWithCname();
    void responseKeepsRcode();
    void responseRequiresValidQuery();
    void rejectsTruncatedPayload();
    void rejectsCompressionLoop();
    void randomIdStaysInRange();
};

void TestDnsCodec::queryStructure()
{
    const QByteArray payload = dns::buildQuery(0x1234, QStringLiteral("www.example.com"), 1);
    QVERIFY(!payload.isEmpty());
    QVERIFY(payload.size() > 12);

    // 头部：id / 标准查询 + RD / QDCOUNT=1
    QCOMPARE(quint8(payload.at(0)), quint8(0x12));
    QCOMPARE(quint8(payload.at(1)), quint8(0x34));
    QCOMPARE(quint8(payload.at(2)), quint8(0x01));
    QCOMPARE(quint8(payload.at(3)), quint8(0x00));
    QCOMPARE(quint8(payload.at(5)), quint8(0x01));

    // 名字按标签编码：3 'w''w''w' 7 'example' 3 'com' 0
    QCOMPARE(quint8(payload.at(12)), quint8(3));
    QCOMPARE(payload.mid(13, 3), QByteArray("www"));
    QCOMPARE(quint8(payload.at(16)), quint8(7));
    QCOMPARE(payload.mid(17, 7), QByteArray("example"));
}

void TestDnsCodec::queryRoundTrip()
{
    const QByteArray payload = dns::buildQuery(0xBEEF, QStringLiteral("a.b.example.com"), 28);
    const auto       message = dns::parseMessage(payload);
    QVERIFY(message.has_value());
    QVERIFY(message->isValid());
    QVERIFY(!message->isResponse);
    QCOMPARE(message->id, quint16(0xBEEF));
    QCOMPARE(message->questions.size(), 1);
    QCOMPARE(message->questions.first().name, QStringLiteral("a.b.example.com"));
    QCOMPARE(message->questions.first().type, quint16(28)); // AAAA
    QCOMPARE(message->questions.first().cls, quint16(1));
    QVERIFY(message->answers.isEmpty());
}

void TestDnsCodec::queryRejectsBadName()
{
    // 超长标签（64 字节）无法编码，必须返回空而不是发出坏包
    const QString longLabel(64, QLatin1Char('a'));
    QVERIFY(dns::buildQuery(1, longLabel + QStringLiteral(".com"), 1).isEmpty());
    // 空名字在 DNS 里表示根查询（合法），编码结果只有终止符
    const QByteArray root = dns::buildQuery(1, QString(), 1);
    QVERIFY(!root.isEmpty());
    const auto rootMessage = dns::parseMessage(root);
    QVERIFY(rootMessage.has_value());
    QCOMPARE(rootMessage->questions.size(), 1);
    QVERIFY(rootMessage->questions.first().name.isEmpty());
}

void TestDnsCodec::responseRoundTrip()
{
    const QByteArray queryBytes = dns::buildQuery(0x0F0F, QStringLiteral("example.com"), 1);
    const auto       query      = dns::parseMessage(queryBytes);
    QVERIFY(query.has_value());

    dns::Answer answer;
    answer.name    = QStringLiteral("example.com");
    answer.type    = 1;
    answer.ttl     = 300;
    answer.address = QHostAddress(QStringLiteral("93.184.216.34"));

    const QByteArray responseBytes = dns::buildResponse(*query, {answer});
    QVERIFY(!responseBytes.isEmpty());

    const auto response = dns::parseMessage(responseBytes);
    QVERIFY(response.has_value());
    QVERIFY(response->isResponse);
    QCOMPARE(response->id, quint16(0x0F0F));
    QCOMPARE(response->rcode, 0);
    QCOMPARE(response->questions.size(), 1);
    QCOMPARE(response->questions.first().name, QStringLiteral("example.com"));
    QCOMPARE(response->answers.size(), 1);
    QCOMPARE(response->answers.first().type, quint16(1));
    QCOMPARE(response->answers.first().ttl, quint32(300));
    QVERIFY(response->answers.first().address == QHostAddress(QStringLiteral("93.184.216.34")));
}

void TestDnsCodec::responseWithCname()
{
    const auto query = dns::parseMessage(
        dns::buildQuery(7, QStringLiteral("www.example.com"), 1));
    QVERIFY(query.has_value());

    dns::Answer cname;
    cname.name    = QStringLiteral("www.example.com");
    cname.type    = 5;
    cname.ttl     = 60;
    cname.text    = QStringLiteral("example.com");

    dns::Answer address;
    address.name    = QStringLiteral("www.example.com");
    address.type    = 1;
    address.ttl     = 60;
    address.address = QHostAddress(QStringLiteral("1.2.3.4"));

    const auto response = dns::parseMessage(dns::buildResponse(*query, {cname, address}));
    QVERIFY(response.has_value());
    QCOMPARE(response->answers.size(), 2);
    QCOMPARE(response->answers.at(0).type, quint16(5));
    QCOMPARE(response->answers.at(0).text, QStringLiteral("example.com"));
    QVERIFY(response->answers.at(1).address == QHostAddress(QStringLiteral("1.2.3.4")));
}

void TestDnsCodec::responseKeepsRcode()
{
    const auto query = dns::parseMessage(
        dns::buildQuery(9, QStringLiteral("missing.example"), 1));
    QVERIFY(query.has_value());

    // NXDOMAIN：客户端据此判定「域名不存在」，不能变成空应答
    const auto response = dns::parseMessage(dns::buildResponse(*query, {}, 3));
    QVERIFY(response.has_value());
    QCOMPARE(response->rcode, 3);
    QCOMPARE(response->answers.size(), 0);
}

void TestDnsCodec::responseRequiresValidQuery()
{
    dns::Message invalid; // 没经过 parseMessage，isValid() == false
    invalid.id = 1;
    QVERIFY(dns::buildResponse(invalid, {}).isEmpty());

    // 有 id 但没有问题段，同样不能编造应答
    const auto parsed = dns::parseMessage(makeHeader(1, 0x0100, 0, 0));
    QVERIFY(parsed.has_value());
    QVERIFY(dns::buildResponse(*parsed, {}).isEmpty());
}

void TestDnsCodec::rejectsTruncatedPayload()
{
    QVERIFY(!dns::parseMessage(QByteArray()).has_value());
    QVERIFY(!dns::parseMessage(QByteArray(5, '\0')).has_value());

    // 头部说有问题段，但后面什么都没有
    QVERIFY(!dns::parseMessage(makeHeader(1, 0x0100, 1, 0)).has_value());

    // 名字中途被截断（声明 7 字节，只给了 3 字节）
    QByteArray cut = makeHeader(1, 0x0100, 1, 0);
    cut.append(char(7)).append("abc");
    QVERIFY(!dns::parseMessage(cut).has_value());
}

void TestDnsCodec::rejectsCompressionLoop()
{
    // 问题名写成指向自己的压缩指针（0xC00C 指回偏移 12）。跟随它就会
    // 无限递归，这里必须靠深度保护停下并判定为非法报文。
    QByteArray payload = makeHeader(1, 0x0100, 1, 0);
    payload.append(char(0xC0)).append(char(0x0C));
    payload.append(char(0)).append(char(1)).append(char(0)).append(char(1));

    // 只要不崩溃、不挂死即可（结果允许是 nullopt 或空名字）
    const auto message = dns::parseMessage(payload);
    if (message) {
        QVERIFY(message->questions.isEmpty()
                || message->questions.first().name.isEmpty());
    }
}

void TestDnsCodec::randomIdStaysInRange()
{
    bool seenDifferent = false;
    quint16 first = dns::randomId();
    for (int i = 0; i < 64; ++i) {
        const quint16 id = dns::randomId();
        QVERIFY(id != 0);
        if (id != first) seenDifferent = true;
    }
    // 不测分布，只确认不是常量（常量 id 会让并发查询互相串包）
    QVERIFY(seenDifferent);
}

QTEST_GUILESS_MAIN(TestDnsCodec)
#include "test_dns_codec.moc"
