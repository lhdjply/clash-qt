// ---------------------------------------------------------------------------
//  IpCidrSet —— IP-CIDR / SRC-IP-CIDR / GEOIP 规则的匹配后端
//
//  核心契约是「最长前缀优先」：一条连接同时落在 10.0.0.0/8 与 10.1.0.0/16
//  里时，必须返回 /16 对应的策略。这条如果反了，精细化规则全部失效。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "component/cidr/IpCidrSet.h"

using namespace core;

class TestCidr : public QObject
{
    Q_OBJECT

private slots:
    void emptySet();
    void basicHitAndMiss();
    void longestPrefixWins();
    void siblingPrefixes();
    void ipv6Set();
    void rejectsInvalidCidr();
    void acceptsBareAddress();
    void clearResets();
};

void TestCidr::emptySet()
{
    IpCidrSet set;
    QVERIFY(set.isEmpty());
    QCOMPARE(set.size(), 0);
    QVERIFY(!set.lookup(QStringLiteral("10.0.0.1")).has_value());
    QVERIFY(!set.lookup(QStringLiteral("2001:db8::1")).has_value());
}

void TestCidr::basicHitAndMiss()
{
    IpCidrSet set;
    QVERIFY(set.insert(QStringLiteral("10.0.0.0/8"), QStringLiteral("DIRECT")));
    QCOMPARE(set.size(), 1);

    QCOMPARE(set.lookup(QStringLiteral("10.1.2.3")).value_or(QString()), QStringLiteral("DIRECT"));
    QCOMPARE(set.lookup(QStringLiteral("10.255.255.255")).value_or(QString()),
             QStringLiteral("DIRECT"));
    QVERIFY(!set.lookup(QStringLiteral("11.0.0.1")).has_value());

    // 非法地址文本不能命中（也不应崩）
    QVERIFY(!set.lookup(QStringLiteral("not-an-ip")).has_value());
}

void TestCidr::longestPrefixWins()
{
    IpCidrSet set;
    set.insert(QStringLiteral("10.0.0.0/8"), QStringLiteral("WIDE"));
    set.insert(QStringLiteral("10.1.0.0/16"), QStringLiteral("NARROW"));
    set.insert(QStringLiteral("10.1.2.0/24"), QStringLiteral("NARROWEST"));

    // 插入顺序不影响结果：只看前缀长度
    QCOMPARE(set.lookup(QStringLiteral("10.1.2.9")).value_or(QString()), QStringLiteral("NARROWEST"));
    QCOMPARE(set.lookup(QStringLiteral("10.1.3.9")).value_or(QString()), QStringLiteral("NARROW"));
    QCOMPARE(set.lookup(QStringLiteral("10.2.0.1")).value_or(QString()), QStringLiteral("WIDE"));
    QVERIFY(!set.lookup(QStringLiteral("192.168.1.1")).has_value());
}

void TestCidr::siblingPrefixes()
{
    // 同长度、互不包含的两段：必须各自命中自己的策略
    IpCidrSet set;
    set.insert(QStringLiteral("10.0.0.0/24"), QStringLiteral("A"));
    set.insert(QStringLiteral("10.0.1.0/24"), QStringLiteral("B"));

    QCOMPARE(set.lookup(QStringLiteral("10.0.0.5")).value_or(QString()), QStringLiteral("A"));
    QCOMPARE(set.lookup(QStringLiteral("10.0.1.5")).value_or(QString()), QStringLiteral("B"));
    QVERIFY(!set.lookup(QStringLiteral("10.0.2.5")).has_value());
}

void TestCidr::ipv6Set()
{
    IpCidrSet set;
    QVERIFY(set.insert(QStringLiteral("2001:db8::/32"), QStringLiteral("V6")));
    QVERIFY(set.insert(QStringLiteral("2001:db8:1::/48"), QStringLiteral("V6-NARROW")));

    QCOMPARE(set.lookup(QStringLiteral("2001:db8:1::5")).value_or(QString()),
             QStringLiteral("V6-NARROW"));
    QCOMPARE(set.lookup(QStringLiteral("2001:db8:2::5")).value_or(QString()), QStringLiteral("V6"));

    // 地址族隔离：IPv4 地址不能落进 IPv6 段
    QVERIFY(!set.lookup(QStringLiteral("10.0.0.1")).has_value());
}

void TestCidr::rejectsInvalidCidr()
{
    IpCidrSet set;
    QVERIFY(!set.insert(QStringLiteral("10.0.0.0/33")));
    QVERIFY(!set.insert(QStringLiteral("example.com/24")));
    QVERIFY(!set.insert(QString()));
    QVERIFY(set.isEmpty());
}

void TestCidr::acceptsBareAddress()
{
    IpCidrSet set;
    QVERIFY(set.insert(QStringLiteral("1.1.1.1"))); // 无掩码 = /32

    // 这里没给 payload，命中时返回的是「存在但为空」的值 —— 必须用
    // has_value() 判断命中，value_or 会把两者混为一谈
    const auto hit = set.lookup(QStringLiteral("1.1.1.1"));
    QVERIFY(hit.has_value());
    QVERIFY(hit->isEmpty());
    QVERIFY(!set.lookup(QStringLiteral("1.1.1.2")).has_value());
}

void TestCidr::clearResets()
{
    IpCidrSet set;
    set.insert(QStringLiteral("10.0.0.0/8"), QStringLiteral("A"));
    set.insert(QStringLiteral("2001:db8::/32"), QStringLiteral("B"));
    QCOMPARE(set.size(), 2);

    set.clear();
    QVERIFY(set.isEmpty());
    QVERIFY(!set.lookup(QStringLiteral("10.0.0.1")).has_value());
    QVERIFY(!set.lookup(QStringLiteral("2001:db8::1")).has_value());
}

QTEST_GUILESS_MAIN(TestCidr)
#include "test_cidr.moc"
