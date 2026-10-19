// ---------------------------------------------------------------------------
//  DomainTrie —— DOMAIN / DOMAIN-SUFFIX 与 GEOSITE 规则集共用的域名树
//
//  这里最关键的一条边界：后缀匹配必须按「标签」比较，不能按字符串后缀。
//  写成 endsWith 的话 example.com 会匹配上 notexample.com —— 这类误判会
//  让本应走直连的域名被送进代理，而且从日志上完全看不出来。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "component/trie/DomainTrie.h"

using namespace core;

class TestDomainTrie : public QObject
{
    Q_OBJECT

private slots:
    void emptyTrie();
    void exactMatch();
    void suffixMatch();
    void suffixRespectsLabelBoundary();
    void longestSuffixWins();
    void caseAndDotInsensitive();
    void reinsertUpdatesPayload();
    void clearRemovesEverything();
    void ignoresEmptyDomain();
};

void TestDomainTrie::emptyTrie()
{
    DomainTrie trie;
    QVERIFY(trie.isEmpty());
    QCOMPARE(trie.size(), 0);
    QVERIFY(!trie.searchExact(QStringLiteral("example.com")).has_value());
    QVERIFY(!trie.searchSuffix(QStringLiteral("example.com")).has_value());
}

void TestDomainTrie::exactMatch()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("example.com"), QStringLiteral("DIRECT"));

    QCOMPARE(trie.searchExact(QStringLiteral("example.com")).value_or(QString()),
             QStringLiteral("DIRECT"));
    // 精确匹配不接受子域
    QVERIFY(!trie.searchExact(QStringLiteral("www.example.com")).has_value());
    QCOMPARE(trie.size(), 1);
}

void TestDomainTrie::suffixMatch()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("example.com"), QStringLiteral("PROXY"));

    QCOMPARE(trie.searchSuffix(QStringLiteral("example.com")).value_or(QString()),
             QStringLiteral("PROXY"));
    QCOMPARE(trie.searchSuffix(QStringLiteral("www.example.com")).value_or(QString()),
             QStringLiteral("PROXY"));
    QCOMPARE(trie.searchSuffix(QStringLiteral("a.b.c.example.com")).value_or(QString()),
             QStringLiteral("PROXY"));

    // 只插入的域名本身是终止节点，中间层级不是
    QVERIFY(!trie.searchSuffix(QStringLiteral("com")).has_value());
    QVERIFY(!trie.searchSuffix(QStringLiteral("other.com")).has_value());
}

void TestDomainTrie::suffixRespectsLabelBoundary()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("example.com"), QStringLiteral("PROXY"));

    // 字符串后缀相同但标签不同 —— 必须不命中
    QVERIFY(!trie.searchSuffix(QStringLiteral("notexample.com")).has_value());
    QVERIFY(!trie.searchSuffix(QStringLiteral("myexample.com")).has_value());
    // 反过来也要成立：插入 example.com，不能命中 example.com.cn
    QVERIFY(!trie.searchSuffix(QStringLiteral("example.com.cn")).has_value());
}

void TestDomainTrie::longestSuffixWins()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("com"), QStringLiteral("GLOBAL"));
    trie.insert(QStringLiteral("example.com"), QStringLiteral("MID"));
    trie.insert(QStringLiteral("api.example.com"), QStringLiteral("SPECIFIC"));

    QCOMPARE(trie.searchSuffix(QStringLiteral("a.api.example.com")).value_or(QString()),
             QStringLiteral("SPECIFIC"));
    QCOMPARE(trie.searchSuffix(QStringLiteral("www.example.com")).value_or(QString()),
             QStringLiteral("MID"));
    QCOMPARE(trie.searchSuffix(QStringLiteral("other.com")).value_or(QString()),
             QStringLiteral("GLOBAL"));
    QCOMPARE(trie.size(), 3);
}

void TestDomainTrie::caseAndDotInsensitive()
{
    DomainTrie trie;
    trie.insert(QStringLiteral(".Example.COM."), QStringLiteral("PROXY"));

    QCOMPARE(trie.searchSuffix(QStringLiteral("WWW.example.com")).value_or(QString()),
             QStringLiteral("PROXY"));
    QCOMPARE(trie.searchExact(QStringLiteral("example.com")).value_or(QString()),
             QStringLiteral("PROXY"));
    // 查询侧带点也要能命中
    QCOMPARE(trie.searchSuffix(QStringLiteral(".example.com")).value_or(QString()),
             QStringLiteral("PROXY"));
}

void TestDomainTrie::reinsertUpdatesPayload()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("example.com"), QStringLiteral("A"));
    trie.insert(QStringLiteral("example.com"), QStringLiteral("B"));

    QCOMPARE(trie.searchExact(QStringLiteral("example.com")).value_or(QString()),
             QStringLiteral("B"));
    // 同一个域名重复插入不应把计数翻倍（规则集去重依赖它）
    QCOMPARE(trie.size(), 1);
}

void TestDomainTrie::clearRemovesEverything()
{
    DomainTrie trie;
    trie.insert(QStringLiteral("a.com"), QStringLiteral("A"));
    trie.insert(QStringLiteral("b.com"), QStringLiteral("B"));
    QCOMPARE(trie.size(), 2);

    trie.clear();
    QVERIFY(trie.isEmpty());
    QCOMPARE(trie.size(), 0);
    QVERIFY(!trie.searchSuffix(QStringLiteral("a.com")).has_value());
}

void TestDomainTrie::ignoresEmptyDomain()
{
    DomainTrie trie;
    trie.insert(QString(), QStringLiteral("X"));
    trie.insert(QStringLiteral("   "), QStringLiteral("X"));
    trie.insert(QStringLiteral("..."), QStringLiteral("X"));

    QVERIFY(trie.isEmpty());
    QVERIFY(!trie.searchSuffix(QStringLiteral("anything")).has_value());
}

QTEST_GUILESS_MAIN(TestDomainTrie)
#include "test_domain_trie.moc"
