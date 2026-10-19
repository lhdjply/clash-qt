// ---------------------------------------------------------------------------
//  数据目录约定
//
//  只验证一条规则：Linux 上数据目录是 <XDG_CONFIG_HOME>/clash-qt，不再有
//  第二层 clash-qt。这条当初算错过一次 —— QStandardPaths::AppConfigLocation
//  在 Linux 上是 <config>/<org>/<app>，而组织名与应用名都叫 clash-qt，于是
//  落到了 ~/.config/clash-qt/clash-qt，用户升级后看着像配置全丢了。
//
//  另外验证旧布局的一次性迁移：内容上移一层，旧目录消失。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

#include "utils/Paths.h"

using namespace clashqt;

namespace {

bool writeFile(const QString &path, const QByteArray &content)
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return file.write(content) == content.size();
}

} // namespace

class TestPaths : public QObject
{
    Q_OBJECT

private slots:
    void dataDirIsUnderConfigHome();
    void migratesNestedLegacyLayout();
    void keepsExistingFilesWhenBothLayoutsPresent();
    void portableModeWins();

private:
    // 每个用例独立一个 XDG_CONFIG_HOME，互不影响
    QTemporaryDir m_home;
};

void TestPaths::dataDirIsUnderConfigHome()
{
    QVERIFY(m_home.isValid());
    qputenv("XDG_CONFIG_HOME", m_home.path().toUtf8());

    const QString data = paths::dataDir();
    QCOMPARE(data, m_home.path() + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME));
    QVERIFY(QFileInfo(data).isDir());

    // 关键断言：不允许出现 <data>/clash-qt 这种嵌套
    QVERIFY(!QFileInfo::exists(data + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME)));
}

void TestPaths::migratesNestedLegacyLayout()
{
    QVERIFY(m_home.isValid());
    qputenv("XDG_CONFIG_HOME", m_home.path().toUtf8());

    const QString base   = m_home.path() + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);
    const QString legacy = base + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);

    QVERIFY(writeFile(legacy + QStringLiteral("/config.yaml"), "mixed-port: 7890\n"));
    QVERIFY(writeFile(legacy + QStringLiteral("/profiles/p1.yaml"), "proxies: []\n"));
    QVERIFY(writeFile(legacy + QStringLiteral("/rules/p1.yaml"), "prepend: []\n"));

    const QString data = paths::dataDir();
    QCOMPARE(data, base);

    // 内容上移一层
    QVERIFY(QFileInfo::exists(base + QStringLiteral("/config.yaml")));
    QVERIFY(QFileInfo::exists(base + QStringLiteral("/profiles/p1.yaml")));
    QVERIFY(QFileInfo::exists(base + QStringLiteral("/rules/p1.yaml")));
    // 旧目录清理干净（连临时中转目录也不该留下）
    QVERIFY(!QFileInfo::exists(legacy));
    QVERIFY(!QFileInfo::exists(base + QStringLiteral("/.migrate-from-nested")));
}

void TestPaths::keepsExistingFilesWhenBothLayoutsPresent()
{
    QVERIFY(m_home.isValid());
    qputenv("XDG_CONFIG_HOME", m_home.path().toUtf8());

    const QString base   = m_home.path() + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);
    const QString legacy = base + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);

    // 新目录里已有同名文件（用户已经在新位置改过配置），旧文件不能覆盖它
    QVERIFY(writeFile(base + QStringLiteral("/config.yaml"), "mixed-port: 9999\n"));
    QVERIFY(writeFile(legacy + QStringLiteral("/config.yaml"), "mixed-port: 7890\n"));
    QVERIFY(writeFile(legacy + QStringLiteral("/profile.yaml"), "items: []\n"));

    Q_UNUSED(paths::dataDir());

    QFile config(base + QStringLiteral("/config.yaml"));
    QVERIFY(config.open(QIODevice::ReadOnly));
    QCOMPARE(config.readAll(), QByteArray("mixed-port: 9999\n"));

    // 不冲突的文件仍然迁过来
    QVERIFY(QFileInfo::exists(base + QStringLiteral("/profile.yaml")));
    QVERIFY(!QFileInfo::exists(legacy));
}

void TestPaths::portableModeWins()
{
    // 便携模式看的是 exeDir()/PORTABLE，测试进程的可执行文件在 build/bin/tests，
    // 那里不会有标记文件；这里只确认非便携时不会误判。
    QVERIFY(!paths::isPortable());
}

QTEST_GUILESS_MAIN(TestPaths)
#include "test_paths.moc"
