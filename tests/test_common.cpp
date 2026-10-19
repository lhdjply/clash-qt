// ---------------------------------------------------------------------------
//  IpNet / strutil —— 最底层的类型
//
//  这里的用例大多来自真实故障：
//    · IpNet 曾把小端机上的 quint32 直接 memcpy 进字节数组，导致 /24 落在
//      错误字节上 —— 所有 IP-CIDR 规则的判定都是错的（TUN 下表现为
//      「明明写了直连规则却仍然走代理」）。
//    · contains(IpNet) 用于规则集折叠，前缀关系写反会让整段规则被误删。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "base/Common.h"

using namespace core;

class TestCommon : public QObject
{
    Q_OBJECT

private slots:
    void parseIpv4();
    void ipv4PrefixMatching();
    void ipv4ByteOrderRegression();
    void ipv4BoundaryPrefixes();
    void parseIpv6();
    void ipv6PrefixMatching();
    void invalidInputRejected();
    void netContainsNet();
    void toStringKeepsPrefix();

    void splitAndTrim();
    void parseBooleans();
    void parseInts();
    void caseHelpers();
};

void TestCommon::parseIpv4()
{
    const auto address = IpNet::parseIp(QStringLiteral("192.168.1.10"));
    QVERIFY(address.has_value());
    QCOMPARE(address->protocol(), QAbstractSocket::IPv4Protocol);
    QCOMPARE(address->toString(), QStringLiteral("192.168.1.10"));

    // 首尾空白是常见的手写配置问题，必须容忍
    const auto padded = IpNet::parseIp(QStringLiteral("  1.2.3.4  "));
    QVERIFY(padded.has_value());
    QCOMPARE(padded->toString(), QStringLiteral("1.2.3.4"));
}

void TestCommon::ipv4PrefixMatching()
{
    const auto net = IpNet::parse(QStringLiteral("10.0.0.0/8"));
    QVERIFY(net.has_value());
    QCOMPARE(net->family(), 4);
    QCOMPARE(net->prefixLength(), 8);

    QVERIFY(net->contains(QHostAddress(QStringLiteral("10.0.0.1"))));
    QVERIFY(net->contains(QHostAddress(QStringLiteral("10.255.255.255"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("11.0.0.1"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("9.255.255.255"))));

    // 地址族不同不能误判为命中
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("2001:db8::1"))));
}

void TestCommon::ipv4ByteOrderRegression()
{
    // /24 必须只比对最高 3 个字节。若字节序反了，192.168.1.0/24 会匹配
    // 192.0.1.x 这类完全无关的地址。
    const auto net = IpNet::parse(QStringLiteral("192.168.1.0/24"));
    QVERIFY(net.has_value());

    QVERIFY(net->contains(QHostAddress(QStringLiteral("192.168.1.1"))));
    QVERIFY(net->contains(QHostAddress(QStringLiteral("192.168.1.255"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("192.168.2.1"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("192.0.1.5"))));

    // /16 同理：10.1.0.0/16 不应匹配 10.0.1.1
    const auto net16 = IpNet::parse(QStringLiteral("10.1.0.0/16"));
    QVERIFY(net16.has_value());
    QVERIFY(net16->contains(QHostAddress(QStringLiteral("10.1.2.3"))));
    QVERIFY(!net16->contains(QHostAddress(QStringLiteral("10.0.1.1"))));
}

void TestCommon::ipv4BoundaryPrefixes()
{
    const auto all = IpNet::parse(QStringLiteral("0.0.0.0/0"));
    QVERIFY(all.has_value());
    QVERIFY(all->contains(QHostAddress(QStringLiteral("1.2.3.4"))));
    QVERIFY(all->contains(QHostAddress(QStringLiteral("255.255.255.255"))));

    const auto single = IpNet::parse(QStringLiteral("8.8.8.8/32"));
    QVERIFY(single.has_value());
    QVERIFY(single->contains(QHostAddress(QStringLiteral("8.8.8.8"))));
    QVERIFY(!single->contains(QHostAddress(QStringLiteral("8.8.8.9"))));

    // 无掩码等价于单主机
    const auto bare = IpNet::parse(QStringLiteral("1.1.1.1"));
    QVERIFY(bare.has_value());
    QCOMPARE(bare->prefixLength(), 32);
    QVERIFY(bare->contains(QHostAddress(QStringLiteral("1.1.1.1"))));

    // 非整字节前缀：掩码要落在正确的位上（/25 只覆盖后半段）
    const auto half = IpNet::parse(QStringLiteral("192.168.0.0/25"));
    QVERIFY(half.has_value());
    QVERIFY(half->contains(QHostAddress(QStringLiteral("192.168.0.127"))));
    QVERIFY(!half->contains(QHostAddress(QStringLiteral("192.168.0.128"))));
}

void TestCommon::parseIpv6()
{
    const auto net = IpNet::parse(QStringLiteral("2001:db8::/32"));
    QVERIFY(net.has_value());
    QCOMPARE(net->family(), 6);
    QCOMPARE(net->prefixLength(), 32);

    // 方括号写法（URL 里的形式）也要能解析
    const auto bracketed = IpNet::parse(QStringLiteral("[2001:db8::1]/128"));
    QVERIFY(bracketed.has_value());
    QCOMPARE(bracketed->prefixLength(), 128);
}

void TestCommon::ipv6PrefixMatching()
{
    const auto net = IpNet::parse(QStringLiteral("2001:db8::/32"));
    QVERIFY(net.has_value());
    QVERIFY(net->contains(QHostAddress(QStringLiteral("2001:db8::1"))));
    QVERIFY(net->contains(QHostAddress(QStringLiteral("2001:db8:ffff::1"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("2001:db9::1"))));
    QVERIFY(!net->contains(QHostAddress(QStringLiteral("10.0.0.1"))));
}

void TestCommon::invalidInputRejected()
{
    QVERIFY(!IpNet::parseIp(QString()).has_value());
    QVERIFY(!IpNet::parseIp(QStringLiteral("999.1.1.1")).has_value());
    QVERIFY(!IpNet::parseIp(QStringLiteral("example.com")).has_value());

    QVERIFY(!IpNet::parse(QStringLiteral("10.0.0.0/33")).has_value());  // 前缀越界
    QVERIFY(!IpNet::parse(QStringLiteral("10.0.0.0/-1")).has_value());
    QVERIFY(!IpNet::parse(QStringLiteral("10.0.0.0/abc")).has_value()); // 非数字前缀
    QVERIFY(!IpNet::parse(QStringLiteral("not-an-ip/24")).has_value());
    QVERIFY(!IpNet::parse(QString()).has_value());
}

void TestCommon::netContainsNet()
{
    const auto big   = IpNet::parse(QStringLiteral("10.0.0.0/8"));
    const auto small = IpNet::parse(QStringLiteral("10.1.0.0/16"));
    const auto other = IpNet::parse(QStringLiteral("11.0.0.0/16"));
    const auto wide  = IpNet::parse(QStringLiteral("0.0.0.0/0"));
    QVERIFY(big && small && other && wide);

    QVERIFY(big->contains(*small));    // 小的落在大的里面
    QVERIFY(!small->contains(*big));   // 反向不成立（前缀更短）
    QVERIFY(!big->contains(*other));
    QVERIFY(wide->contains(*big));     // 0.0.0.0/0 包含一切

    // 地址族不同不做包含判断
    const auto v6 = IpNet::parse(QStringLiteral("2001:db8::/32"));
    QVERIFY(v6.has_value());
    QVERIFY(!big->contains(*v6));
}

void TestCommon::toStringKeepsPrefix()
{
    const auto net = IpNet::parse(QStringLiteral("10.0.0.0/24"));
    QVERIFY(net.has_value());
    QCOMPARE(net->toString(), QStringLiteral("10.0.0.0/24"));

    // 非法对象输出空串，而不是 "0.0.0.0/0" 这种会误导人的值
    QCOMPARE(IpNet().toString(), QString());
}

void TestCommon::splitAndTrim()
{
    const QStringList parts =
        strutil::splitAndTrim(QStringLiteral(" a , b ,, c "), QLatin1Char(','));
    QCOMPARE(parts, (QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}));

    QVERIFY(strutil::splitAndTrim(QString(), QLatin1Char(',')).isEmpty());
}

void TestCommon::parseBooleans()
{
    for (const char *text : {"true", "TRUE", "yes", "on", "1", " true "}) {
        const auto value = strutil::parseBool(QString::fromLatin1(text));
        QVERIFY2(value.has_value() && *value, text);
    }
    for (const char *text : {"false", "No", "off", "0"}) {
        const auto value = strutil::parseBool(QString::fromLatin1(text));
        QVERIFY2(value.has_value() && !*value, text);
    }
    // 不认识的值必须是「无结果」，不能被当成 false —— 调用方会据此保留默认值
    QVERIFY(!strutil::parseBool(QStringLiteral("maybe")).has_value());
    QVERIFY(!strutil::parseBool(QString()).has_value());
}

void TestCommon::parseInts()
{
    QCOMPARE(strutil::parseInt(QStringLiteral(" 8080 ")).value_or(-1), 8080);
    QCOMPARE(strutil::parseInt(QStringLiteral("0")).value_or(-1), 0);
    QVERIFY(!strutil::parseInt(QStringLiteral("80-443")).has_value());
    QVERIFY(!strutil::parseInt(QStringLiteral("12abc")).has_value());
}

void TestCommon::caseHelpers()
{
    QVERIFY(strutil::startsWithIgnoreCase(QStringLiteral("Example.COM"), QStringLiteral("example")));
    QVERIFY(strutil::endsWithIgnoreCase(QStringLiteral("Example.COM"), QStringLiteral(".com")));
    QVERIFY(strutil::equalsIgnoreCase(QStringLiteral("DIRECT"), QStringLiteral("direct")));

    // 长度不足不能越界比较
    QVERIFY(!strutil::startsWithIgnoreCase(QStringLiteral("ab"), QStringLiteral("abc")));
    QVERIFY(!strutil::endsWithIgnoreCase(QStringLiteral("ab"), QStringLiteral("abc")));
}

QTEST_GUILESS_MAIN(TestCommon)
#include "test_common.moc"
