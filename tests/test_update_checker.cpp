// ---------------------------------------------------------------------------
//  检查更新
//
//  三部分：
//    1. 版本号比较 —— 「有没有更新」完全由它决定，判错会天天提示同一个版本；
//    2. GitHub API 的 JSON 解析与状态码处理；
//    3. API 被限流（403）时切换到 Atom 备用源 —— 实测走公共节点常撞上。
//  网络部分用本地 QTcpServer 冒充 GitHub，测试不联网、不依赖仓库是否发过版。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QHash>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>

#include "updater/UpdateChecker.h"

using namespace clashqt;

namespace {

// 按路径返回预设状态码与正文的本地 HTTP 服务；未配置的路径一律 404
struct FakeServer {
    struct Response {
        QByteArray status = QByteArrayLiteral("200 OK");
        QByteArray body;
    };

    QTcpServer               server;
    QHash<QString, Response> routes;

    bool start()
    {
        QObject::connect(&server, &QTcpServer::newConnection, &server, [this]() {
            while (QTcpSocket *socket = server.nextPendingConnection()) {
                QObject::connect(socket, &QTcpSocket::readyRead, socket, [this, socket]() {
                    if (socket->property("replied").toBool()) return;

                    QByteArray buffer = socket->property("buffer").toByteArray();
                    buffer += socket->readAll();
                    socket->setProperty("buffer", buffer);

                    const int headerEnd = buffer.indexOf("\r\n\r\n");
                    if (headerEnd < 0) return; // 请求还没收全

                    socket->setProperty("replied", true);
                    const QByteArray requestLine = buffer.left(buffer.indexOf("\r\n"));
                    const QList<QByteArray> parts = requestLine.split(' ');
                    const QString path =
                        parts.size() > 1 ? QString::fromLatin1(parts.at(1)) : QString();

                    Response response;
                    if (routes.contains(path)) {
                        response = routes.value(path);
                    } else {
                        response.status = QByteArrayLiteral("404 Not Found");
                    }

                    QByteArray raw = "HTTP/1.1 " + response.status + "\r\n";
                    raw += "Content-Type: application/octet-stream\r\n";
                    raw += "Content-Length: " + QByteArray::number(response.body.size()) + "\r\n";
                    raw += "Connection: close\r\n\r\n";
                    raw += response.body;
                    socket->write(raw);
                    socket->disconnectFromHost();
                });
                QObject::connect(socket, &QTcpSocket::disconnected, socket,
                                 &QObject::deleteLater);
            }
        });
        return server.listen(QHostAddress::LocalHost);
    }

    QUrl url(const QString &path) const
    {
        return QUrl(QStringLiteral("http://127.0.0.1:%1%2").arg(server.serverPort()).arg(path));
    }
};

// 与真实响应同形：数组，且第一条可能是 CI 的持续构建（tag 不是版本号）
const char kReleaseJson[] =
    "[{\"tag_name\":\"continuous_build\",\"name\":\"持续构建\","
    "\"body\":\"由 CI 自动生成\","
    "\"html_url\":\"https://github.com/lhdjply/clash-qt/releases/tag/continuous_build\","
    "\"published_at\":\"2026-10-19T06:00:00Z\"},"
    "{\"tag_name\":\"v999.0.0\",\"name\":\"999.0.0\","
    "\"body\":\"- 新增检查更新\","
    "\"html_url\":\"https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0\","
    "\"published_at\":\"2026-10-19T05:00:00Z\"}]";

const char kReleasesAtom[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<feed xmlns=\"http://www.w3.org/2005/Atom\">\n"
    "  <title>Release notes from clash-qt</title>\n"
    "  <entry>\n"
    "    <id>tag:github.com,2008:Repository/1/continuous_build</id>\n"
    "    <updated>2026-10-19T06:00:00Z</updated>\n"
    "    <link rel=\"alternate\" type=\"text/html\""
    " href=\"https://github.com/lhdjply/clash-qt/releases/tag/continuous_build\"/>\n"
    "    <title>持续构建</title>\n"
    "    <content type=\"html\">&lt;p&gt;由 CI 自动生成&lt;/p&gt;</content>\n"
    "  </entry>\n"
    "  <entry>\n"
    "    <id>tag:github.com,2008:Repository/1/v999.0.0</id>\n"
    "    <updated>2026-10-19T05:00:00Z</updated>\n"
    "    <link rel=\"alternate\" type=\"text/html\""
    " href=\"https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0\"/>\n"
    "    <title>v999.0.0</title>\n"
    "    <content type=\"html\">&lt;p&gt;修复了托盘图标&lt;/p&gt;</content>\n"
    "  </entry>\n"
    "</feed>\n";

} // namespace

class TestUpdateChecker : public QObject
{
    Q_OBJECT

private slots:
    void newerTagIsDetected();
    void sameVersionWithPrefix();
    void numericOrderNotLexical();
    void missingSegmentsAreZero();
    void prereleaseSuffixIsIgnored();

    void updateAvailableCarriesReleaseInfo();
    void sameVersionReportsUpToDate();
    void noReleasesCountsAsUpToDate();
    void serverErrorIsReported();
    void forbiddenFallsBackToAtom();
};

// ---------------------------------------------------------------------------
//  版本号比较
// ---------------------------------------------------------------------------
void TestUpdateChecker::newerTagIsDetected()
{
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("0.2.0"), QStringLiteral("0.1.0")) > 0);
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("0.1.0"), QStringLiteral("0.2.0")) < 0);
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("0.1.0"), QStringLiteral("0.1.0")) == 0);
}

void TestUpdateChecker::sameVersionWithPrefix()
{
    // 发布 tag 常写成 v0.1.0，而 VERSION 文件里没有前缀
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("v0.1.0"), QStringLiteral("0.1.0")), 0);
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("V0.2.0"), QStringLiteral("0.1.0")) > 0);
}

void TestUpdateChecker::numericOrderNotLexical()
{
    // 按字符串比会把 "0.10.0" 判成比 "0.9.9" 旧
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("0.10.0"), QStringLiteral("0.9.9")) > 0);
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("1.0.0"), QStringLiteral("0.99.99")) > 0);
}

void TestUpdateChecker::missingSegmentsAreZero()
{
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("1.2"), QStringLiteral("1.2.0")), 0);
    QVERIFY(UpdateChecker::compareVersions(QStringLiteral("0.1"), QStringLiteral("0.1.1")) < 0);
}

void TestUpdateChecker::prereleaseSuffixIsIgnored()
{
    // latest 端点本身不含预发布版；即便 tag 带了后缀，也只比数字部分，
    // 不会因为后缀把同一个版本判成「有更新」
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("0.1.0-rc1"),
                                            QStringLiteral("0.1.0")),
             0);
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("0.2.0+build.7"),
                                            QStringLiteral("0.2.0")),
             0);
}

// ---------------------------------------------------------------------------
//  GitHub 响应
// ---------------------------------------------------------------------------
void TestUpdateChecker::updateAvailableCarriesReleaseInfo()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleaseJson)});
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")), QUrl());
    ReleaseInfo  received;
    int          count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 5000);

    QCOMPARE(received.tagName, QStringLiteral("v999.0.0"));
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(received.notes, QStringLiteral("- 新增检查更新"));
    QVERIFY(!received.notesIsHtml);
    QCOMPARE(received.pageUrl.toString(),
             QStringLiteral("https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0"));
    QCOMPARE(received.publishedAt, QStringLiteral("2026-10-19T05:00:00Z"));
}

void TestUpdateChecker::sameVersionReportsUpToDate()
{
    FakeServer server;
    // 旧版本：不该提示更新
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("200 OK"),
                          QByteArrayLiteral(R"({"tag_name":"v0.0.1","body":"old"})")});
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")), QUrl());
    QSignalSpy    spy(&checker, &UpdateChecker::upToDate);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 5000);
    QCOMPARE(spy.first().at(0).toString(), QLatin1String(CLASHQT_VERSION));
}

void TestUpdateChecker::noReleasesCountsAsUpToDate()
{
    FakeServer server; // 未配置任何路径 → 404
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")), QUrl());
    QSignalSpy    spy(&checker, &UpdateChecker::upToDate);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 5000);
}

void TestUpdateChecker::serverErrorIsReported()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("500 Internal Server Error"), QByteArray()});
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")), QUrl());
    QSignalSpy    spy(&checker, &UpdateChecker::failed);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 5000);
    QVERIFY2(spy.first().at(0).toString().contains(QStringLiteral("500")),
             qPrintable(spy.first().at(0).toString()));
}

void TestUpdateChecker::forbiddenFallsBackToAtom()
{
    FakeServer server;
    // 未认证 API 配额耗尽时 GitHub 返回 403 —— 实测走公共节点很常见
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("403 Forbidden"),
                          QByteArrayLiteral(R"({"message":"API rate limit exceeded"})")});
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleasesAtom)});
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")),
                          server.url(QStringLiteral("/releases.atom")));
    ReleaseInfo  received;
    int          count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 5000);

    QCOMPARE(received.tagName, QStringLiteral("v999.0.0"));
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QVERIFY(received.notesIsHtml);
    QCOMPARE(received.notes, QStringLiteral("<p>修复了托盘图标</p>"));
    QCOMPARE(received.pageUrl.toString(),
             QStringLiteral("https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0"));
}

QTEST_GUILESS_MAIN(TestUpdateChecker)
#include "test_update_checker.moc"
