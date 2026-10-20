// ---------------------------------------------------------------------------
//  规则解析与匹配
//
//  覆盖三类最容易出错的语义：
//    1. 域名后缀必须按点边界比较（example.com 不能匹配 notexample.com）
//    2. 需要解析的规则（IP-CIDR 等）只有在 no-resolve 下才跳过 DNS 解析，
//       否则 Tunnel 会拿不到 dstIp 直接判定不命中 —— 表现为「规则像没写」
//    3. 逻辑规则（AND / OR / NOT）的子规则不带策略，解析时要补占位策略，
//       求值时不能因为子规则 target 为空而误判
// ---------------------------------------------------------------------------

#include <QtTest>

#include "rules/RuleParser.h"

using namespace core;

namespace {

Metadata tcpMetadata(const QString &host, const QString &dstIp = QString())
{
    Metadata metadata;
    metadata.network = QStringLiteral("tcp");
    metadata.host    = host;
    metadata.dstIp   = dstIp;
    metadata.srcIp   = QStringLiteral("192.168.1.100");
    metadata.srcPort = 51234;
    metadata.dstPort = 443;
    return metadata;
}

RulePtr parseOk(const QString &text)
{
    QString  error;
    RulePtr rule = RuleParser::parse(text, nullptr, &error);
    if (!rule) {
        qWarning("规则解析失败：%s（%s）", qPrintable(text), qPrintable(error));
    }
    return rule;
}

} // namespace

class TestRules : public QObject
{
    Q_OBJECT

private slots:
    void parseDomain();
    void domainSuffixBoundary();
    void domainKeyword();
    void domainRegex();
    void ipCidr();
    void ipCidrRequiresResolution();
    void srcIpCidr();
    void ipv6Cidr();
    void portRules();
    void portListSyntax();
    void networkRule();
    void processRules();
    void matchRule();
    void logicRules();
    void ruleSet();
    void disabledRuleNeverMatches();

    void rejectsInvalidRules();
    void parseAllSkipsInvalid();
    void splitTopLevelKeepsInnerCommas();
};

void TestRules::parseDomain()
{
    RulePtr rule = parseOk(QStringLiteral("DOMAIN,Example.COM,DIRECT"));
    QVERIFY(rule);
    QVERIFY(rule->type() == RuleType::Domain);
    QCOMPARE(rule->target(), QStringLiteral("DIRECT"));

    // 大小写不敏感，但不接受子域
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("EXAMPLE.COM")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("www.example.com")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QString()), nullptr));
}

void TestRules::domainSuffixBoundary()
{
    RulePtr rule = parseOk(QStringLiteral("DOMAIN-SUFFIX,example.com,PROXY"));
    QVERIFY(rule);
    QVERIFY(rule->type() == RuleType::DomainSuffix);

    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("a.b.example.com")), nullptr));

    // 点边界：字符串后缀相同但标签不同，绝不能命中
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("notexample.com")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("example.com.cn")), nullptr));
}

void TestRules::domainKeyword()
{
    RulePtr rule = parseOk(QStringLiteral("DOMAIN-KEYWORD,google,PROXY"));
    QVERIFY(rule);
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("www.google.com")), nullptr));
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("googleapis.com")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));
}

void TestRules::domainRegex()
{
    RulePtr rule = parseOk(QStringLiteral("DOMAIN-REGEX,^ads\\d*\\.example\\.com$,REJECT"));
    QVERIFY(rule);
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("ads12.example.com")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("ads.example.org")), nullptr));

    // 非法正则必须解析失败，而不是带着一个永不命中的空正则活着
    QString error;
    // "*bad" 是量词开头，QRegularExpression 判为非法；
    // 注意不要用未闭合的 "[a-z"：切分器会把 '[' 也当作分组括号，
    // 那样的输入会在「字段不足」处就被拦下，测不到正则校验本身
    QVERIFY(!RuleParser::parse(QStringLiteral("DOMAIN-REGEX,*bad,REJECT"), nullptr, &error));
    QVERIFY2(error.contains(QStringLiteral("regular expression")), qPrintable(error));
}

void TestRules::ipCidr()
{
    RulePtr rule = parseOk(QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT"));
    QVERIFY(rule);
    QVERIFY(rule->type() == RuleType::IpCidr);

    QVERIFY(rule->matches(tcpMetadata(QString(), QStringLiteral("10.1.2.3")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QString(), QStringLiteral("11.1.2.3")), nullptr));

    // 没有 dstIp（还没解析）时不命中，等 Tunnel 解析后重试
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));
}

void TestRules::ipCidrRequiresResolution()
{
    RulePtr resolving = parseOk(QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT"));
    QVERIFY(resolving);
    QVERIFY(resolving->requiresResolution());

    // no-resolve：不再触发 DNS 解析，只用现有 dstIp 判定
    RulePtr noResolve = parseOk(QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT,no-resolve"));
    QVERIFY(noResolve);
    QVERIFY(!noResolve->requiresResolution());
    QVERIFY(noResolve->noResolve());

    // SRC-IP-CIDR 从来不涉及目标解析
    RulePtr src = parseOk(QStringLiteral("SRC-IP-CIDR,192.168.0.0/16,DIRECT"));
    QVERIFY(src);
    QVERIFY(!src->requiresResolution());
}

void TestRules::srcIpCidr()
{
    RulePtr rule = parseOk(QStringLiteral("SRC-IP-CIDR,192.168.0.0/16,DIRECT"));
    QVERIFY(rule);

    Metadata hit = tcpMetadata(QStringLiteral("example.com"));
    hit.srcIp    = QStringLiteral("192.168.5.5");
    QVERIFY(rule->matches(hit, nullptr));

    Metadata miss = tcpMetadata(QStringLiteral("example.com"));
    miss.srcIp    = QStringLiteral("10.5.5.5");
    QVERIFY(!rule->matches(miss, nullptr));
}

void TestRules::ipv6Cidr()
{
    RulePtr rule = parseOk(QStringLiteral("IP-CIDR6,2001:db8::/32,DIRECT"));
    QVERIFY(rule);

    Metadata hit = tcpMetadata(QStringLiteral("example.com"), QStringLiteral("2001:db8::1"));
    QVERIFY(rule->matches(hit, nullptr));

    Metadata miss = tcpMetadata(QStringLiteral("example.com"), QStringLiteral("2001:db9::1"));
    QVERIFY(!rule->matches(miss, nullptr));
}

void TestRules::portRules()
{
    RulePtr single = parseOk(QStringLiteral("DST-PORT,443,PROXY"));
    QVERIFY(single);
    QCOMPARE(single->target(), QStringLiteral("PROXY"));
    QVERIFY(single->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));

    Metadata other = tcpMetadata(QStringLiteral("example.com"));
    other.dstPort  = 8443;
    QVERIFY(!single->matches(other, nullptr));

    RulePtr range = parseOk(QStringLiteral("DST-PORT,8000-8100,PROXY"));
    QVERIFY(range);
    Metadata inRange = tcpMetadata(QStringLiteral("example.com"));
    inRange.dstPort  = 8080;
    QVERIFY(range->matches(inRange, nullptr));
    inRange.dstPort = 8101;
    QVERIFY(!range->matches(inRange, nullptr));

    RulePtr srcPort = parseOk(QStringLiteral("SRC-PORT,51234,DIRECT"));
    QVERIFY(srcPort);
    QVERIFY(srcPort->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));

    RulePtr inPort = parseOk(QStringLiteral("IN-PORT,7890,DIRECT"));
    QVERIFY(inPort);
    Metadata inbound = tcpMetadata(QStringLiteral("example.com"));
    inbound.inboundPort = 7890;
    QVERIFY(inPort->matches(inbound, nullptr));
}

void TestRules::portListSyntax()
{
    // Clash 允许端口列表：最后一段才是策略，中间的都属于 payload。
    // 之前把第二段当成策略，导致「80,443,DIRECT」被解析成 target=443。
    RulePtr rule = parseOk(QStringLiteral("DST-PORT,80,443,DIRECT"));
    QVERIFY(rule);
    QCOMPARE(rule->target(), QStringLiteral("DIRECT"));

    Metadata port80 = tcpMetadata(QStringLiteral("example.com"));
    port80.dstPort  = 80;
    QVERIFY(rule->matches(port80, nullptr));

    Metadata port443 = tcpMetadata(QStringLiteral("example.com"));
    QVERIFY(rule->matches(port443, nullptr));

    Metadata port22 = tcpMetadata(QStringLiteral("example.com"));
    port22.dstPort  = 22;
    QVERIFY(!rule->matches(port22, nullptr));
}

void TestRules::networkRule()
{
    RulePtr udp = parseOk(QStringLiteral("NETWORK,udp,REJECT"));
    QVERIFY(udp);

    Metadata udpMeta = tcpMetadata(QStringLiteral("example.com"));
    udpMeta.network  = QStringLiteral("UDP"); // 大小写不敏感
    QVERIFY(udp->matches(udpMeta, nullptr));
    QVERIFY(!udp->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));
}

void TestRules::processRules()
{
    RulePtr name = parseOk(QStringLiteral("PROCESS-NAME,Telegram,PROXY"));
    QVERIFY(name);

    Metadata byName = tcpMetadata(QStringLiteral("example.com"));
    byName.processName = QStringLiteral("telegram");
    QVERIFY(name->matches(byName, nullptr));

    // 进程名只填了完整路径时，也要能和 PROCESS-NAME 对上
    Metadata byPath = tcpMetadata(QStringLiteral("example.com"));
    byPath.processName = QStringLiteral("/usr/bin/Telegram");
    QVERIFY(name->matches(byPath, nullptr));

    Metadata missing = tcpMetadata(QStringLiteral("example.com"));
    QVERIFY(!name->matches(missing, nullptr));

    RulePtr path = parseOk(QStringLiteral("PROCESS-PATH,/usr/bin/curl,DIRECT"));
    QVERIFY(path);
    Metadata curl = tcpMetadata(QStringLiteral("example.com"));
    curl.processPath = QStringLiteral("/usr/bin/curl");
    QVERIFY(path->matches(curl, nullptr));
}

void TestRules::matchRule()
{
    RulePtr rule = parseOk(QStringLiteral("MATCH,DIRECT"));
    QVERIFY(rule);
    QVERIFY(rule->type() == RuleType::Match);
    QCOMPARE(rule->target(), QStringLiteral("DIRECT"));

    // 兜底规则对任何连接都命中，包括完全没有元数据的情况
    QVERIFY(rule->matches(Metadata{}, nullptr));
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("anything.example")), nullptr));
}

void TestRules::logicRules()
{
    RulePtr andRule =
        parseOk(QStringLiteral("AND,((DOMAIN-SUFFIX,google.com),(NETWORK,tcp)),PROXY"));
    QVERIFY(andRule);
    QVERIFY(andRule->type() == RuleType::And);
    QCOMPARE(andRule->target(), QStringLiteral("PROXY"));

    Metadata both = tcpMetadata(QStringLiteral("www.google.com"));
    QVERIFY(andRule->matches(both, nullptr));

    Metadata udp = both;
    udp.network  = QStringLiteral("udp");
    QVERIFY(!andRule->matches(udp, nullptr)); // 只满足一半

    Metadata otherHost = tcpMetadata(QStringLiteral("www.example.com"));
    QVERIFY(!andRule->matches(otherHost, nullptr));

    RulePtr orRule = parseOk(
        QStringLiteral("OR,((DOMAIN,a.example.com),(DOMAIN,b.example.com)),PROXY"));
    QVERIFY(orRule);
    QVERIFY(orRule->matches(tcpMetadata(QStringLiteral("b.example.com")), nullptr));
    QVERIFY(!orRule->matches(tcpMetadata(QStringLiteral("c.example.com")), nullptr));

    RulePtr notRule = parseOk(QStringLiteral("NOT,((DOMAIN-SUFFIX,ads.example.com)),DIRECT"));
    QVERIFY(notRule);
    QVERIFY(!notRule->matches(tcpMetadata(QStringLiteral("x.ads.example.com")), nullptr));
    QVERIFY(notRule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));

    // 有条件需要解析时，外层也要标记出来，否则 Tunnel 不会先去解析
    RulePtr resolving =
        parseOk(QStringLiteral("AND,((IP-CIDR,10.0.0.0/8),(NETWORK,tcp)),DIRECT"));
    QVERIFY(resolving);
    QVERIFY(resolving->requiresResolution());
}

void TestRules::ruleSet()
{
    QStringList warnings;
    auto        set = RuleParser::parseRuleSet(
        QStringLiteral("ads"), RuleSet::Behavior::Domain,
        QJsonArray{QStringLiteral("ads.example.com"), QStringLiteral("tracker.example.net")},
        &warnings);
    QVERIFY(set);
    QVERIFY(warnings.isEmpty());
    QCOMPARE(set->count(), 2);

    RuleSetRegistry registry;
    registry.registerSet(QStringLiteral("ads"), set);

    QString  error;
    RulePtr  rule = RuleParser::parse(QStringLiteral("RULE-SET,ads,REJECT"), &registry, &error);
    QVERIFY2(rule != nullptr, qPrintable(error));
    QVERIFY(rule);
    QCOMPARE(rule->target(), QStringLiteral("REJECT"));
    QVERIFY(rule->matches(tcpMetadata(QStringLiteral("cdn.ads.example.com")), nullptr));
    QVERIFY(!rule->matches(tcpMetadata(QStringLiteral("example.com")), nullptr));

    // 未注册的规则集：解析失败而不是产生一条永远不命中的规则
    QVERIFY(!RuleParser::parse(QStringLiteral("RULE-SET,missing,REJECT"), &registry, &error));
}

void TestRules::disabledRuleNeverMatches()
{
    RulePtr rule = parseOk(QStringLiteral("MATCH,DIRECT"));
    QVERIFY(rule);
    QVERIFY(rule->matches(Metadata{}, nullptr));

    rule->setDisabled(true);
    QVERIFY(rule->disabled());
    QVERIFY(!rule->matches(Metadata{}, nullptr));
}

void TestRules::rejectsInvalidRules()
{
    QString error;

    QVERIFY(!RuleParser::parse(QString(), nullptr, &error));
    QVERIFY(!error.isEmpty());

    QVERIFY(!RuleParser::parse(QStringLiteral("NOT-A-TYPE,payload,DIRECT"), nullptr, &error));
    QVERIFY(error.contains(QStringLiteral("Unknown rule type")));

    // 少一段：缺少策略名
    QVERIFY(!RuleParser::parse(QStringLiteral("DOMAIN,example.com"), nullptr, &error));
    QVERIFY(error.contains(QStringLiteral("too few fields")));

    QVERIFY(!RuleParser::parse(QStringLiteral("MATCH"), nullptr, &error));
    QVERIFY(error.contains(QStringLiteral("MATCH")));

    QVERIFY(!RuleParser::parse(QStringLiteral("IP-CIDR,10.0.0.0/33,DIRECT"), nullptr, &error));
    QVERIFY(error.contains(QStringLiteral("IP")));

    QVERIFY(!RuleParser::parse(QStringLiteral("DST-PORT,notaport,DIRECT"), nullptr, &error));
}

void TestRules::parseAllSkipsInvalid()
{
    const QJsonArray rules{
        QStringLiteral("DOMAIN,a.example.com,DIRECT"),
        QStringLiteral("BROKEN RULE"),
        QStringLiteral("MATCH,PROXY"),
    };

    QStringList  warnings;
    const auto   parsed = RuleParser::parseAll(rules, nullptr, &warnings);

    QCOMPARE(int(parsed.size()), 2);
    QCOMPARE(warnings.size(), 1);
    QCOMPARE(parsed.front()->target(), QStringLiteral("DIRECT"));
    QVERIFY(parsed.back()->type() == RuleType::Match);
}

void TestRules::splitTopLevelKeepsInnerCommas()
{
    const QStringList parts = RuleParser::splitTopLevel(
        QStringLiteral("AND,((DOMAIN,a.com),(DST-PORT,80,443)),REJECT"), QLatin1Char(','));
    QCOMPARE(parts.size(), 3);
    QCOMPARE(parts.at(0), QStringLiteral("AND"));
    QCOMPARE(parts.at(1), QStringLiteral("((DOMAIN,a.com),(DST-PORT,80,443))"));
    QCOMPARE(parts.at(2), QStringLiteral("REJECT"));
}

QTEST_GUILESS_MAIN(TestRules)
#include "test_rules.moc"
