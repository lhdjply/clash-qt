// ---------------------------------------------------------------------------
//  FakeIpPool —— fake-ip 模式的地址池
//
//  fake-ip 是 TUN 模式能用的前提：客户端拿到的必须是一个稳定、可反查的
//  占位地址。这里的两条硬要求：
//    · 同一域名反复查询要拿到同一个地址（否则连接会被拆成多条）
//    · 反查必须能还原域名（否则 TUN 中继不知道要连哪里，只能失败）
//  地址池耗尽后的回绕也要保持双向映射自洽，不能出现「地址指向 A、A 指向
//  另一个地址」的错乱。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "component/dns/Resolver.h"

using namespace core;

class TestFakeIp : public QObject
{
    Q_OBJECT

private slots:
    void disabledPool();
    void allocateInsideRange();
    void allocationIsStable();
    void distinctHostsGetDistinctAddresses();
    void reverseLookupWorks();
    void containsChecksRange();
    void invalidRangeFallsBack();
    void clearDisablesPool();
    void manyAllocationsStayConsistent();
    void wrapAroundKeepsMappingsConsistent();
};

void TestFakeIp::disabledPool()
{
    FakeIpPool pool;
    QVERIFY(!pool.enabled());
    QCOMPARE(pool.size(), 0);
    QVERIFY(pool.allocate(QStringLiteral("example.com")).isNull());
    QVERIFY(!pool.reverseLookup(QHostAddress(QStringLiteral("198.18.0.2"))).has_value());
    QVERIFY(!pool.contains(QHostAddress(QStringLiteral("198.18.0.2"))));
}

void TestFakeIp::allocateInsideRange()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));
    QVERIFY(pool.enabled());

    const QHostAddress address = pool.allocate(QStringLiteral("example.com"));
    QVERIFY(!address.isNull());
    QCOMPARE(address.protocol(), QAbstractSocket::IPv4Protocol);
    QVERIFY2(address.toString().startsWith(QStringLiteral("198.18.")),
             qPrintable(address.toString()));
    QVERIFY(pool.contains(address));

    // 池自己的地址（198.18.0.1）留给网关，不应分配出去
    QVERIFY(address.toString() != QStringLiteral("198.18.0.1"));
}

void TestFakeIp::allocationIsStable()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));

    const QHostAddress first  = pool.allocate(QStringLiteral("example.com"));
    const QHostAddress second = pool.allocate(QStringLiteral("example.com"));

    QVERIFY(first == second);
    QCOMPARE(pool.size(), 1); // 幂等，不能每次查询都新建映射
}

void TestFakeIp::distinctHostsGetDistinctAddresses()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));

    const QHostAddress a = pool.allocate(QStringLiteral("a.example.com"));
    const QHostAddress b = pool.allocate(QStringLiteral("b.example.com"));
    QVERIFY(a != b);
    QCOMPARE(pool.size(), 2);
}

void TestFakeIp::reverseLookupWorks()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));

    const QHostAddress address = pool.allocate(QStringLiteral("www.example.com"));
    QCOMPARE(pool.reverseLookup(address).value_or(QString()), QStringLiteral("www.example.com"));

    // 池外地址没有映射
    QVERIFY(!pool.reverseLookup(QHostAddress(QStringLiteral("1.1.1.1"))).has_value());
    // IPv6 地址不会被误判
    QVERIFY(!pool.reverseLookup(QHostAddress(QStringLiteral("2001:db8::1"))).has_value());
}

void TestFakeIp::containsChecksRange()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("10.10.0.0/16"));

    QVERIFY(pool.contains(QHostAddress(QStringLiteral("10.10.0.5"))));
    QVERIFY(pool.contains(QHostAddress(QStringLiteral("10.10.255.255"))));
    QVERIFY(!pool.contains(QHostAddress(QStringLiteral("10.11.0.1"))));
    QVERIFY(!pool.contains(QHostAddress(QStringLiteral("192.168.0.1"))));
    QVERIFY(!pool.contains(QHostAddress(QStringLiteral("2001:db8::1"))));
}

void TestFakeIp::invalidRangeFallsBack()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("not-a-cidr"));

    // 非法配置不能让池失效，而是退回默认的 198.18.0.0/16
    QVERIFY(pool.enabled());
    const QHostAddress address = pool.allocate(QStringLiteral("example.com"));
    QVERIFY(address.toString().startsWith(QStringLiteral("198.18.")));

    // 非 IPv4 的段同样回退（fake-ip 只有 IPv4 池）
    FakeIpPool v6pool;
    v6pool.configure(QStringLiteral("2001:db8::/32"));
    QVERIFY(v6pool.allocate(QStringLiteral("example.com")).toString().startsWith(
        QStringLiteral("198.18.")));
}

void TestFakeIp::clearDisablesPool()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));
    const QHostAddress address = pool.allocate(QStringLiteral("example.com"));
    QVERIFY(pool.contains(address));

    pool.clear();
    QVERIFY(!pool.enabled());
    QCOMPARE(pool.size(), 0);
    QVERIFY(!pool.reverseLookup(address).has_value());
    QVERIFY(pool.allocate(QStringLiteral("example.com")).isNull());
}

void TestFakeIp::manyAllocationsStayConsistent()
{
    FakeIpPool pool;
    pool.configure(QStringLiteral("198.18.0.1/16"));

    QSet<QString> seen;
    for (int i = 0; i < 500; ++i) {
        const QString      host    = QStringLiteral("host%1.example.com").arg(i);
        const QHostAddress address = pool.allocate(host);
        QVERIFY(!address.isNull());
        QVERIFY(pool.contains(address));

        // 每个域名拿到唯一地址，且反查回自己
        const QString key = address.toString();
        QVERIFY2(!seen.contains(key), qPrintable(key));
        seen.insert(key);
        QCOMPARE(pool.reverseLookup(address).value_or(QString()), host);
    }
    QCOMPARE(pool.size(), 500);
}

void TestFakeIp::wrapAroundKeepsMappingsConsistent()
{
    // /30 只有两个可用地址：第 3 次分配必然回绕覆盖
    FakeIpPool pool;
    pool.configure(QStringLiteral("10.0.0.0/30"));

    const QHostAddress a = pool.allocate(QStringLiteral("a.example"));
    const QHostAddress b = pool.allocate(QStringLiteral("b.example"));
    const QHostAddress c = pool.allocate(QStringLiteral("c.example"));

    QVERIFY(!a.isNull() && !b.isNull() && !c.isNull());
    QVERIFY(pool.contains(c));

    // 被覆盖的地址必须指向新域名，不能留下「旧域名 → 这个地址」的前向映射
    QCOMPARE(pool.reverseLookup(c).value_or(QString()), QStringLiteral("c.example"));
    QVERIFY(pool.allocate(QStringLiteral("c.example")) == c);
}

QTEST_GUILESS_MAIN(TestFakeIp)
#include "test_fakeip.moc"
