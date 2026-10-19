// ---------------------------------------------------------------------------
//  JSON 深合并与键名修饰语法
//
//  订阅覆写（override yaml）大量使用 `<key>` / `key!` / `+key` / `key+`，
//  这套语义直接照搬参照实现的 merge.ts。它最容易出两类问题：
//    · 该递归合并的整段被覆盖 —— 用户只写了 dns 的一个字段，整个 dns 段没了
//    · `key!` 该整段替换的却递归合并 —— 想删掉的旧字段永远删不掉
//  这两种都不会报错，只会让「覆写好像没生效」。
// ---------------------------------------------------------------------------

#include <QtTest>

#include "common/JsonUtil.h"

using namespace core;

namespace {

QJsonObject obj(std::initializer_list<QPair<QString, QJsonValue>> items)
{
    QJsonObject object;
    for (const auto &item : items) object.insert(item.first, item.second);
    return object;
}

} // namespace

class TestJsonUtil : public QObject
{
    Q_OBJECT

private slots:
    void recursiveMerge();
    void scalarOverrides();
    void angleBracketsAreStripped();
    void replaceWholeObject();
    void arraysReplaceByDefault();
    void prependOnlyInOverrideMode();
    void appendOnlyInOverrideMode();
    void getAndSetPath();
    void typedAccessorsAcceptStrings();
    void trimWrapHandlesBothSides();
};

void TestJsonUtil::recursiveMerge()
{
    const QJsonObject target = obj({
        {QStringLiteral("dns"), obj({{QStringLiteral("enable"), true},
                                     {QStringLiteral("nameserver"), QJsonArray{QStringLiteral("1.1.1.1")}}})},
        {QStringLiteral("port"), 7890},
    });

    const QJsonObject patch = obj({
        {QStringLiteral("dns"), obj({{QStringLiteral("ipv6"), false}})},
    });

    const QJsonObject merged = jsonutil::deepMerge(target, patch);
    const QJsonObject dns    = merged.value(QStringLiteral("dns")).toObject();

    // 补丁里没提到的字段必须保留
    QVERIFY(dns.value(QStringLiteral("enable")).toBool());
    QCOMPARE(dns.value(QStringLiteral("nameserver")).toArray().size(), 1);
    QCOMPARE(dns.value(QStringLiteral("ipv6")).toBool(), false);
    QCOMPARE(merged.value(QStringLiteral("port")).toInt(), 7890);
}

void TestJsonUtil::scalarOverrides()
{
    const QJsonObject target = obj({{QStringLiteral("mode"), QStringLiteral("rule")},
                                    {QStringLiteral("log-level"), QStringLiteral("info")}});
    const QJsonObject patch  = obj({{QStringLiteral("log-level"), QStringLiteral("debug")}});

    const QJsonObject merged = jsonutil::deepMerge(target, patch);
    QCOMPARE(merged.value(QStringLiteral("log-level")).toString(), QStringLiteral("debug"));
    QCOMPARE(merged.value(QStringLiteral("mode")).toString(), QStringLiteral("rule"));
}

void TestJsonUtil::angleBracketsAreStripped()
{
    const QJsonObject target = obj({{QStringLiteral("dns"), obj({{QStringLiteral("enable"), true}})}});
    const QJsonObject patch  = obj({{QStringLiteral("<dns>"), obj({{QStringLiteral("ipv6"), true}})},
                                    {QStringLiteral("<port>"), 7891}});

    const QJsonObject merged = jsonutil::deepMerge(target, patch);
    QVERIFY(!merged.contains(QStringLiteral("<dns>")));
    QVERIFY(!merged.contains(QStringLiteral("<port>")));
    QCOMPARE(merged.value(QStringLiteral("port")).toInt(), 7891);

    const QJsonObject dns = merged.value(QStringLiteral("dns")).toObject();
    QCOMPARE(dns.value(QStringLiteral("ipv6")).toBool(), true);
    QCOMPARE(dns.value(QStringLiteral("enable")).toBool(), true); // 递归合并生效
}

void TestJsonUtil::replaceWholeObject()
{
    const QJsonObject target = obj({
        {QStringLiteral("dns"),
         obj({{QStringLiteral("enable"), true},
              {QStringLiteral("nameserver"), QJsonArray{QStringLiteral("1.1.1.1")}}})},
    });
    // 结尾的 '!' 表示整段替换：这是「让旧字段消失」的唯一手段
    const QJsonObject patch = obj({
        {QStringLiteral("dns!"), obj({{QStringLiteral("enable"), false}})},
    });

    const QJsonObject merged = jsonutil::deepMerge(target, patch);
    QVERIFY(!merged.contains(QStringLiteral("dns!")));
    const QJsonObject dns = merged.value(QStringLiteral("dns")).toObject();
    QCOMPARE(dns.size(), 1);
    QVERIFY(!dns.contains(QStringLiteral("nameserver")));
}

void TestJsonUtil::arraysReplaceByDefault()
{
    const QJsonObject target = obj({{QStringLiteral("nameserver"),
                                     QJsonArray{QStringLiteral("1.1.1.1"),
                                                QStringLiteral("8.8.8.8")}}});
    const QJsonObject patch  = obj({{QStringLiteral("nameserver"),
                                     QJsonArray{QStringLiteral("223.5.5.5")}}});

    const QJsonObject merged = jsonutil::deepMerge(target, patch);
    const QJsonArray  list   = merged.value(QStringLiteral("nameserver")).toArray();
    QCOMPARE(list.size(), 1);
    QCOMPARE(list.first().toString(), QStringLiteral("223.5.5.5"));
}

void TestJsonUtil::prependOnlyInOverrideMode()
{
    const QJsonObject target = obj({{QStringLiteral("rules"),
                                     QJsonArray{QStringLiteral("MATCH,DIRECT")}}});
    const QJsonObject patch  = obj({{QStringLiteral("+rules"),
                                     QJsonArray{QStringLiteral("DOMAIN,a.example,DIRECT")}}});

    // 普通合并（订阅自带配置）："+rules" 不是特殊语法，只是一个新键
    const QJsonObject plain = jsonutil::deepMerge(target, patch, false);
    QCOMPARE(plain.value(QStringLiteral("rules")).toArray().size(), 1);

    // 覆写模式：前置拼接
    const QJsonObject merged = jsonutil::deepMerge(target, patch, true);
    QVERIFY(!merged.contains(QStringLiteral("+rules")));
    const QJsonArray rules = merged.value(QStringLiteral("rules")).toArray();
    QCOMPARE(rules.size(), 2);
    QCOMPARE(rules.at(0).toString(), QStringLiteral("DOMAIN,a.example,DIRECT"));
    QCOMPARE(rules.at(1).toString(), QStringLiteral("MATCH,DIRECT"));
}

void TestJsonUtil::appendOnlyInOverrideMode()
{
    const QJsonObject target = obj({{QStringLiteral("rules"),
                                     QJsonArray{QStringLiteral("MATCH,DIRECT")}}});
    const QJsonObject patch  = obj({{QStringLiteral("rules+"),
                                     QJsonArray{QStringLiteral("DOMAIN,b.example,PROXY")}}});

    const QJsonObject merged = jsonutil::deepMerge(target, patch, true);
    QVERIFY(!merged.contains(QStringLiteral("rules+")));
    const QJsonArray rules = merged.value(QStringLiteral("rules")).toArray();
    QCOMPARE(rules.size(), 2);
    QCOMPARE(rules.at(0).toString(), QStringLiteral("MATCH,DIRECT"));
    QCOMPARE(rules.at(1).toString(), QStringLiteral("DOMAIN,b.example,PROXY"));
}

void TestJsonUtil::getAndSetPath()
{
    QJsonObject object;
    object = jsonutil::setPath(object, QStringLiteral("a.b.c"), 42);
    QCOMPARE(jsonutil::getPath(object, QStringLiteral("a.b.c")).toInt(), 42);

    // 中间层缺失或有类型冲突时返回 Undefined，而不是崩
    QVERIFY(jsonutil::getPath(object, QStringLiteral("a.x.c")).isUndefined());
    QVERIFY(jsonutil::getPath(object, QStringLiteral("a.b.c.d")).isUndefined());
    QVERIFY(jsonutil::getPath(object, QString()).isUndefined());

    // 已有值不是对象时，setPath 重建中间层
    QJsonObject broken = obj({{QStringLiteral("a"), QStringLiteral("not-an-object")}});
    broken = jsonutil::setPath(broken, QStringLiteral("a.b"), 1);
    QCOMPARE(jsonutil::getPath(broken, QStringLiteral("a.b")).toInt(), 1);
}

void TestJsonUtil::typedAccessorsAcceptStrings()
{
    // 订阅里端口、开关经常以字符串形式出现（YAML 引号 / 机场生成器），
    // 取值函数必须同时接受字符串与数字
    const QJsonObject object = obj({{QStringLiteral("port"), QStringLiteral("7890")},
                                    {QStringLiteral("enable"), QStringLiteral("true")},
                                    {QStringLiteral("name"), 12345}});

    QCOMPARE(jsonutil::intValue(object, QStringLiteral("port")), 7890);
    QCOMPARE(jsonutil::boolValue(object, QStringLiteral("enable")), true);
    QCOMPARE(jsonutil::stringValue(object, QStringLiteral("name")), QStringLiteral("12345"));

    // 缺失或不可解析时返回调用方给的默认值
    QCOMPARE(jsonutil::intValue(object, QStringLiteral("missing"), 7), 7);
    QCOMPARE(jsonutil::boolValue(object, QStringLiteral("missing"), true), true);
    QCOMPARE(jsonutil::stringArrayValue(object, QStringLiteral("missing")).size(), 0);
}

void TestJsonUtil::trimWrapHandlesBothSides()
{
    QCOMPARE(jsonutil::trimWrap(QStringLiteral("<key>")), QStringLiteral("key"));
    QCOMPARE(jsonutil::trimWrap(QStringLiteral("key")), QStringLiteral("key"));
    QCOMPARE(jsonutil::trimWrap(QString()), QString());

    // 只有成对出现才去掉：单边的尖括号可能是键名本身的字符，
    // 无条件裁掉会把「<key」写成「key」，等于改了用户的键名
    QCOMPARE(jsonutil::trimWrap(QStringLiteral("<key")), QStringLiteral("<key"));
    QCOMPARE(jsonutil::trimWrap(QStringLiteral("key>")), QStringLiteral("key>"));
    QCOMPARE(jsonutil::trimWrap(QStringLiteral("<>")), QString());
}

QTEST_GUILESS_MAIN(TestJsonUtil)
#include "test_json_util.moc"
