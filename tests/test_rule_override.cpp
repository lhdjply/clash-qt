// ---------------------------------------------------------------------------
//  规则覆写（覆写页背后的合并逻辑）
//
//  「覆写」页不直接改 work/config.yaml —— 那份是生成物，下次订阅更新就被
//  覆盖。真正的来源是 rules/<订阅 id>.yaml，由 Factory 在每次生成配置时
//  叠加。这里验证四种叠加语义：
//    prepend   插到订阅规则最前（顺序即优先级）
//    append    追加到末尾（注意 MATCH 通常在最末，追加项不会生效）
//    delete    精确删除订阅自带的规则
//    disabled  停用：文本仍在文件里，但应用时跳过
//
//  这一层的 bug 极难从界面看出来：规则还在列表里、日志也不报错，只是
//  「这条规则好像没生效」。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QTemporaryDir>

#include "common/Yaml.h"
#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "config/Factory.h"
#include "config/OverrideConfig.h"
#include "config/ProfileConfig.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

using namespace clashqt;

namespace {

const char kSubscribeYaml[] = R"(proxies:
  - name: node-a
    type: ss
    server: 1.2.3.4
    port: 443
    cipher: aes-128-gcm
    password: secret
rules:
  - DOMAIN-SUFFIX,sub.example.com,DIRECT
  - IP-CIDR,10.0.0.0/8,DIRECT,no-resolve
  - MATCH,node-a
)";

} // namespace

class TestRuleOverride : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();

    void noOverrideFileKeepsSubscription();
    void prependGoesFirst();
    void prependKeepsOrder();
    void offsetInsertAtExactPosition();
    void appendGoesLast();
    void deleteRemovesRule();
    void disabledIsSkippedButKeptInFile();
    void clearingRestoresOriginal();

private:
    QString profileId() const { return QStringLiteral("selftest-profile"); }

    void writeOverride(const QStringList &prepend, const QStringList &append = {},
                       const QStringList &remove = {}, const QStringList &disabled = {});
    QStringList effectiveRules();

    QTemporaryDir                          m_configHome;
    std::unique_ptr<AppConfig>             m_appConfig;
    std::unique_ptr<ControledKernelConfig> m_controled;
    std::unique_ptr<ProfileConfig>         m_profiles;
    std::unique_ptr<OverrideConfig>        m_overrides;
    std::unique_ptr<Factory>               m_factory;
};

void TestRuleOverride::initTestCase()
{
    QVERIFY(m_configHome.isValid());

    // 必须在构造任何配置对象之前改环境变量：dataDir() 是实时读取
    // QStandardPaths 的，晚设置会让对象读写到用户的真实配置目录
    qputenv("XDG_CONFIG_HOME", m_configHome.path().toUtf8());
    QCoreApplication::setApplicationName(QStringLiteral("clash-qt"));
    QCoreApplication::setOrganizationName(QStringLiteral("clash-qt"));

    m_appConfig = std::make_unique<AppConfig>();
    QVERIFY(m_appConfig->load());

    m_controled = std::make_unique<ControledKernelConfig>(m_appConfig.get());
    m_controled->load();

    m_profiles = std::make_unique<ProfileConfig>();
    m_profiles->load();

    ProfileItem item;
    item.id   = profileId();
    item.name = QStringLiteral("自检订阅");
    item.type = QStringLiteral("local");
    QVERIFY(m_profiles->addItem(item, true));
    QVERIFY(m_profiles->setProfileContent(profileId(), QString::fromUtf8(kSubscribeYaml)));

    m_overrides = std::make_unique<OverrideConfig>();
    m_overrides->load();

    m_factory = std::make_unique<Factory>(m_appConfig.get(), m_controled.get(),
                                          m_profiles.get(), m_overrides.get());
}

void TestRuleOverride::init()
{
    // 每个用例都从「没有覆写文件」开始
    QFile::remove(paths::rulePath(profileId()));
}

void TestRuleOverride::cleanupTestCase()
{
    QFile::remove(paths::rulePath(profileId()));
}

void TestRuleOverride::writeOverride(const QStringList &prepend, const QStringList &append,
                                     const QStringList &remove, const QStringList &disabled)
{
    QJsonObject root;
    if (!prepend.isEmpty()) root.insert(QStringLiteral("prepend"), QJsonArray::fromStringList(prepend));
    if (!append.isEmpty()) root.insert(QStringLiteral("append"), QJsonArray::fromStringList(append));
    if (!remove.isEmpty()) root.insert(QStringLiteral("delete"), QJsonArray::fromStringList(remove));
    if (!disabled.isEmpty()) {
        root.insert(QStringLiteral("disabled"), QJsonArray::fromStringList(disabled));
    }

    QString error;
    const QString path = paths::rulePath(profileId());
    QVERIFY2(safefile::writeAtomic(path, core::Yaml::stringify(QJsonValue(root)), &error),
             qPrintable(error));
}

QStringList TestRuleOverride::effectiveRules()
{
    Factory::Result result;
    const auto     config = m_factory->generateInMemory(&result);
    if (!config) {
        qWarning("生成配置失败：%s", qPrintable(result.error));
        return {};
    }

    QStringList rules;
    for (const QJsonValue &value : config->rules()) {
        if (value.isString()) rules << value.toString();
    }
    return rules;
}

void TestRuleOverride::noOverrideFileKeepsSubscription()
{
    const QStringList rules = effectiveRules();
    QCOMPARE(rules,
             (QStringList{QStringLiteral("DOMAIN-SUFFIX,sub.example.com,DIRECT"),
                          QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT,no-resolve"),
                          QStringLiteral("MATCH,node-a")}));
}

void TestRuleOverride::prependGoesFirst()
{
    writeOverride({QStringLiteral("DOMAIN-SUFFIX,company.internal,DIRECT")});

    const QStringList rules = effectiveRules();
    QCOMPARE(rules.size(), 4);
    QCOMPARE(rules.first(), QStringLiteral("DOMAIN-SUFFIX,company.internal,DIRECT"));
    // 订阅原有规则顺序不变
    QCOMPARE(rules.at(1), QStringLiteral("DOMAIN-SUFFIX,sub.example.com,DIRECT"));
    QCOMPARE(rules.last(), QStringLiteral("MATCH,node-a"));
}

void TestRuleOverride::prependKeepsOrder()
{
    writeOverride({QStringLiteral("DOMAIN,first.example,DIRECT"),
                   QStringLiteral("DOMAIN,second.example,PROXY")});

    const QStringList rules = effectiveRules();
    QCOMPARE(rules.size(), 5);
    QCOMPARE(rules.at(0), QStringLiteral("DOMAIN,first.example,DIRECT"));
    QCOMPARE(rules.at(1), QStringLiteral("DOMAIN,second.example,PROXY"));
}

void TestRuleOverride::offsetInsertAtExactPosition()
{
    // "0,规则" 表示插到第 0 条之前 —— 与普通 prepend 的区别在于可以精确
    // 落到订阅规则中间，用来「抢在订阅的某条规则之前」
    writeOverride({QStringLiteral("1,DOMAIN,before-second.example,DIRECT")});

    const QStringList rules = effectiveRules();
    QCOMPARE(rules.size(), 4);
    QCOMPARE(rules.at(1), QStringLiteral("DOMAIN,before-second.example,DIRECT"));
    QCOMPARE(rules.at(2), QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT,no-resolve"));
}

void TestRuleOverride::appendGoesLast()
{
    writeOverride({}, {QStringLiteral("DOMAIN,fallback.example,PROXY")});

    const QStringList rules = effectiveRules();
    QCOMPARE(rules.size(), 4);
    QCOMPARE(rules.last(), QStringLiteral("DOMAIN,fallback.example,PROXY"));
}

void TestRuleOverride::deleteRemovesRule()
{
    writeOverride({}, {}, {QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT,no-resolve")});

    const QStringList rules = effectiveRules();
    QCOMPARE(rules.size(), 2);
    QVERIFY(!rules.contains(QStringLiteral("IP-CIDR,10.0.0.0/8,DIRECT,no-resolve")));
}

void TestRuleOverride::disabledIsSkippedButKeptInFile()
{
    const QString kept     = QStringLiteral("DOMAIN,enabled.example,DIRECT");
    const QString disabled = QStringLiteral("DOMAIN,disabled.example,REJECT");
    writeOverride({kept, disabled}, {}, {}, {disabled});

    const QStringList rules = effectiveRules();
    QVERIFY(rules.contains(kept));
    QVERIFY(!rules.contains(disabled));

    // 文件里必须仍然保留停用项：取消勾选时不能丢掉用户写的内容
    bool          ok = false;
    const QString content = safefile::readText(paths::rulePath(profileId()), &ok);
    QVERIFY(ok);
    QVERIFY(content.contains(disabled));
}

void TestRuleOverride::clearingRestoresOriginal()
{
    writeOverride({QStringLiteral("DOMAIN,temp.example,DIRECT")});
    QCOMPARE(effectiveRules().size(), 4);

    // 清空后与订阅原始规则一致（等价于界面上删掉所有自定义规则）
    writeOverride({});
    QCOMPARE(effectiveRules().size(), 3);
    QCOMPARE(effectiveRules().first(), QStringLiteral("DOMAIN-SUFFIX,sub.example.com,DIRECT"));
}

QTEST_GUILESS_MAIN(TestRuleOverride)
#include "test_rule_override.moc"
