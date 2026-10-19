// ---------------------------------------------------------------------------
//  YAML <-> JSON
//
//  配置的「读入 → 覆写合并 → 生成运行配置」全程以 QJsonObject 为载体，
//  所以这一层一旦丢字段或改类型，用户配置就被静默破坏。这里盯三件事：
//    · 结构化往返不丢内容（含数组、嵌套映射、中文与 emoji）
//    · 整数不能写成 7890.0（内核按整数读端口，浮点会导致端口解析失败）
//    · 坏配置要明确报错，而不是返回半个空对象
// ---------------------------------------------------------------------------

#include <QtTest>

#include "common/Yaml.h"

#include <QJsonDocument>

using namespace core;

class TestYaml : public QObject
{
    Q_OBJECT

private slots:
    void parseSimpleMap();
    void parseNestedStructures();
    void stringifyIsStable();
    void integerNotWrittenAsFloat();
    void quotedNumberStaysString();
    void emptyDocument();
    void invalidYamlReportsError();
    void topLevelSequence();
    void parseObjectRequiresMap();
    void commentsIgnored();
    void unicodeRoundTrip();
};

void TestYaml::parseSimpleMap()
{
    const QJsonObject object = Yaml::parseObject(
        QStringLiteral("port: 7890\nmode: rule\nlog-level: info\n"));

    QCOMPARE(object.size(), 3);
    // 数字可能以字符串或数字形式落地（见 jsonutil 的兼容处理），
    // 这里只要求文本正确，不锁死内部表示
    QCOMPARE(object.value(QStringLiteral("port")).toVariant().toString(), QStringLiteral("7890"));
    QCOMPARE(object.value(QStringLiteral("mode")).toString(), QStringLiteral("rule"));
}

void TestYaml::parseNestedStructures()
{
    const QString text = QStringLiteral(
        "proxies:\n"
        "  - name: node-a\n"
        "    type: vless\n"
        "    port: 443\n"
        "    ws-opts:\n"
        "      path: /ws\n"
        "      headers:\n"
        "        Host: example.com\n"
        "  - name: node-b\n"
        "    type: hysteria2\n"
        "rules:\n"
        "  - DOMAIN-SUFFIX,example.com,DIRECT\n"
        "  - MATCH,PROXY\n");

    const QJsonObject object = Yaml::parseObject(text);
    const QJsonArray  proxies = object.value(QStringLiteral("proxies")).toArray();
    QCOMPARE(proxies.size(), 2);
    QCOMPARE(proxies.at(0).toObject().value(QStringLiteral("name")).toString(),
             QStringLiteral("node-a"));
    QCOMPARE(proxies.at(0).toObject().value(QStringLiteral("ws-opts")).toObject()
                 .value(QStringLiteral("headers")).toObject()
                 .value(QStringLiteral("Host")).toString(),
             QStringLiteral("example.com"));

    const QJsonArray rules = object.value(QStringLiteral("rules")).toArray();
    QCOMPARE(rules.size(), 2);
    QCOMPARE(rules.at(1).toString(), QStringLiteral("MATCH,PROXY"));
}

void TestYaml::stringifyIsStable()
{
    const QString source = QStringLiteral(
        "mixed-port: 7890\n"
        "allow-lan: false\n"
        "proxies:\n"
        "  - name: a\n"
        "    port: 443\n"
        "rules:\n"
        "  - MATCH,DIRECT\n");

    const auto first = Yaml::parse(source);
    QVERIFY(first.has_value());

    const QString emitted = Yaml::stringify(*first);
    QVERIFY(!emitted.isEmpty());

    // 再解析一次必须得到同样的结构：这条断言能抓住「某类值在写出时被
    // 改了类型」的回归（例如字符串 "on" 被当成布尔）
    const auto second = Yaml::parse(emitted);
    QVERIFY(second.has_value());
    const QJsonObject firstObject  = first->toObject();
    const QJsonObject secondObject = second->toObject();
    if (firstObject != secondObject) {
        qWarning("重新发射的 YAML：\n%s", emitted.toUtf8().constData());
        qWarning("首次解析：%s", QJsonDocument(firstObject).toJson(QJsonDocument::Compact).constData());
        qWarning("再次解析：%s", QJsonDocument(secondObject).toJson(QJsonDocument::Compact).constData());
    }
    QVERIFY(firstObject == secondObject);

    // 再写出一次应当完全稳定
    QCOMPARE(Yaml::stringify(*second), emitted);
}

void TestYaml::integerNotWrittenAsFloat()
{
    const QJsonObject object{
        {QStringLiteral("mixed-port"), 7890},
        {QStringLiteral("interval"), 300},
        {QStringLiteral("ratio"), 0.5},
    };

    const QString text = Yaml::stringify(object);
    QVERIFY(text.contains(QStringLiteral("7890")));
    QVERIFY(!text.contains(QStringLiteral("7890.0")));
    QVERIFY(!text.contains(QStringLiteral("300.0")));
    QVERIFY(text.contains(QStringLiteral("0.5")));
}

void TestYaml::quotedNumberStaysString()
{
    // uuid、密码这类字段可能是纯数字；带引号就必须保持字符串，
    // 否则会被当成数字、丢掉前导零或精度
    const QJsonObject object = Yaml::parseObject(
        QStringLiteral("uuid: \"0012345678\"\npassword: \"123456\"\n"));

    const QJsonValue uuid = object.value(QStringLiteral("uuid"));
    QVERIFY2(uuid.isString(), qPrintable(QString::number(uuid.type())));
    QCOMPARE(uuid.toString(), QStringLiteral("0012345678"));
    QVERIFY(object.value(QStringLiteral("password")).isString());
}

void TestYaml::emptyDocument()
{
    const auto value = Yaml::parse(QString());
    QVERIFY(value.has_value());
    QVERIFY(value->isNull());

    const auto blank = Yaml::parse(QStringLiteral("   \n\n  "));
    QVERIFY(blank.has_value());
    QVERIFY(blank->isNull());

    QVERIFY(Yaml::parseObject(QString()).isEmpty());
}

void TestYaml::invalidYamlReportsError()
{
    QString    error;
    const auto value = Yaml::parse(QStringLiteral("key: [unclosed\nother: 1"), &error);
    QVERIFY(!value.has_value());
    QVERIFY(error.contains(QStringLiteral("YAML")));
}

void TestYaml::topLevelSequence()
{
    const auto value = Yaml::parse(QStringLiteral("- a\n- b\n- 3\n"));
    QVERIFY(value.has_value());
    QVERIFY(value->isArray());
    QCOMPARE(value->toArray().size(), 3);

    // 顶层不是映射时 parseObject 要给出空对象，而不是崩掉
    QVERIFY(Yaml::parseObject(QStringLiteral("- a\n- b\n")).isEmpty());
}

void TestYaml::parseObjectRequiresMap()
{
    QVERIFY(Yaml::parseObject(QStringLiteral("just a string")).isEmpty());
    QVERIFY(Yaml::parseObject(QStringLiteral("42")).isEmpty());
}

void TestYaml::commentsIgnored()
{
    const QJsonObject object = Yaml::parseObject(
        QStringLiteral("# 顶部注释\n"
                       "a: 1  # 行尾注释\n"
                       "# 整行注释\n"
                       "b: two\n"));
    QCOMPARE(object.size(), 2);
    QCOMPARE(object.value(QStringLiteral("b")).toString(), QStringLiteral("two"));
}

void TestYaml::unicodeRoundTrip()
{
    QJsonObject object;
    object.insert(QStringLiteral("name"), QStringLiteral("🇯🇵 日本节点 · 东京"));
    object.insert(QStringLiteral("desc"), QStringLiteral("包含: 冒号与 #井号"));

    const QString text = Yaml::stringify(object);
    const auto    parsed = Yaml::parse(text);
    QVERIFY(parsed.has_value());
    QCOMPARE(parsed->toObject().value(QStringLiteral("name")).toString(),
             QStringLiteral("🇯🇵 日本节点 · 东京"));
    QCOMPARE(parsed->toObject().value(QStringLiteral("desc")).toString(),
             QStringLiteral("包含: 冒号与 #井号"));
}

QTEST_GUILESS_MAIN(TestYaml)
#include "test_yaml.moc"
