// ---------------------------------------------------------------------------
//  检查更新
//
//  四部分：
//    1. 版本号比较 —— 「有没有更新」完全由它决定，判错会天天提示同一个版本；
//    2. GitHub API 的 JSON 解析与状态码处理；
//    3. 来源顺序 —— Atom 订阅优先（不受未认证 API 配额限制），API 兜底；
//    4. 失败处理 —— 偶发抖动重试一次；API 配额用尽后不再白跑。
//  网络部分用本地 QTcpServer 冒充 GitHub，测试不联网、不依赖仓库是否发过版。
// ---------------------------------------------------------------------------

#include <QtTest>

#include <QDateTime>
#include <QElapsedTimer>
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
        QByteArray headers; // 额外的响应头，形如 "X-Test: 1\r\n"

        Response() = default;
        Response(QByteArray status_, QByteArray body_, QByteArray headers_ = QByteArray())
            : status(std::move(status_)), body(std::move(body_)), headers(std::move(headers_))
        {
        }
    };

    QTcpServer                        server;
    QHash<QString, Response>          routes;
    QHash<QString, int>               drops;  // 该路径接下来 N 次直接断开连接
    QHash<QString, int>               stalls; // 该路径接下来 N 次只接受连接、不应答
    mutable QHash<QString, int>       hits;  // 各路径实际收到的请求数

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

                    hits[path] += 1;

                    // 模拟对端还没应答就断开：TLS 停滞 / Connection closed 都长这样
                    if (drops.value(path) > 0) {
                        drops[path] -= 1;
                        socket->abort();
                        return;
                    }

                    // 模拟「连接建起来了但一直不回应」：丢包重传卡住时就是这样，
                    // 唯一能救的是另开一条连接，而不是干等超时
                    if (stalls.value(path) > 0) {
                        stalls[path] -= 1;
                        return;
                    }

                    Response response;
                    if (routes.contains(path)) {
                        response = routes.value(path);
                    } else {
                        response.status = QByteArrayLiteral("404 Not Found");
                    }

                    QByteArray raw = "HTTP/1.1 " + response.status + "\r\n";
                    raw += "Content-Type: application/octet-stream\r\n";
                    raw += response.headers;
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
    void atomIsPreferredOverApi();
    void apiIsUsedWhenAtomFails();
    void transientFailureIsRetried();
    void stalledRequestIsHedged();
    void rateLimitedApiIsSkipped();
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

void TestUpdateChecker::atomIsPreferredOverApi()
{
    FakeServer server;
    // 两条来源都可用时应当走 Atom：API 的 60 次/小时配额按出口 IP 计，
    // 走公共节点几乎必然已被别人用光（实测每次都 403）。
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleasesAtom)});
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("200 OK"),
                          QByteArrayLiteral(R"([{"tag_name":"v1000.0.0","body":"api"}])")});
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

    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QVERIFY(received.notesIsHtml);
    QCOMPARE(received.notes, QStringLiteral("<p>修复了托盘图标</p>"));
    // 关键：API 一次都没被打扰 —— 那正是会撞上配额的那条路径
    QCOMPARE(server.hits.value(QStringLiteral("/latest")), 0);
}

void TestUpdateChecker::apiIsUsedWhenAtomFails()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("500 Internal Server Error"), QByteArray()});
    server.routes.insert(QStringLiteral("/latest"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleaseJson)});
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
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 8000);

    // 兜底过来的结果是 Markdown，对话框要按 notesIsHtml 选解析器
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QVERIFY(!received.notesIsHtml);
    QCOMPARE(received.notes, QStringLiteral("- 新增检查更新"));
    QCOMPARE(server.hits.value(QStringLiteral("/latest")), 1);
}

void TestUpdateChecker::transientFailureIsRetried()
{
    FakeServer server;
    // 第一次还没应答就断开 —— 实测 TUN 下 TLS 停滞、Connection closed 都是这种形态，
    // 紧接着重试通常就成功，不该让整次检查失败
    server.drops.insert(QStringLiteral("/releases.atom"), 1);
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleasesAtom)});
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")),
                          server.url(QStringLiteral("/releases.atom")));
    QSignalSpy failedSpy(&checker, &UpdateChecker::failed);
    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 8000);

    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(server.hits.value(QStringLiteral("/releases.atom")), 2);
}

void TestUpdateChecker::stalledRequestIsHedged()
{
    FakeServer server;
    // 第一条连接挂住不应答（实测丢包时 TCP 重传会卡 2~5 秒）
    server.stalls.insert(QStringLiteral("/releases.atom"), 1);
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kReleasesAtom)});
    QVERIFY(server.start());

    UpdateChecker  checker(server.url(QStringLiteral("/latest")),
                           server.url(QStringLiteral("/releases.atom")));
    QElapsedTimer  clock;
    clock.start();
    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 6000);

    // 关键：结果来自并行的第二条连接，而不是等第一条超时（8 秒）
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(server.hits.value(QStringLiteral("/releases.atom")), 2);
    QVERIFY2(clock.elapsed() < 4000,
             qPrintable(QStringLiteral("耗时 %1ms，对冲没有生效").arg(clock.elapsed())));
}

void TestUpdateChecker::rateLimitedApiIsSkipped()
{
    FakeServer server;
    // Atom 也挂了，才会轮到 API；API 的配额已经用尽
    server.routes.insert(QStringLiteral("/releases.atom"),
                         {QByteArrayLiteral("500 Internal Server Error"), QByteArray()});
    const QByteArray reset =
        QByteArray::number(QDateTime::currentSecsSinceEpoch() + 600);
    FakeServer::Response forbidden;
    forbidden.status  = QByteArrayLiteral("403 Forbidden");
    forbidden.body    = QByteArrayLiteral(R"({"message":"API rate limit exceeded"})");
    forbidden.headers = "x-ratelimit-limit: 60\r\n"
                        "x-ratelimit-remaining: 0\r\n"
                        "x-ratelimit-reset: " + reset + "\r\n";
    server.routes.insert(QStringLiteral("/latest"), forbidden);
    QVERIFY(server.start());

    UpdateChecker checker(server.url(QStringLiteral("/latest")),
                          server.url(QStringLiteral("/releases.atom")));

    QSignalSpy first(&checker, &UpdateChecker::failed);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(first.count(), 1, 8000);
    QVERIFY2(first.first().at(0).toString().contains(QStringLiteral("403")),
             qPrintable(first.first().at(0).toString()));
    QCOMPARE(server.hits.value(QStringLiteral("/latest")), 1);

    // 配额窗口内再查一次：不该再发一次注定 403 的请求
    QSignalSpy second(&checker, &UpdateChecker::failed);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(second.count(), 1, 8000);
    QVERIFY2(second.first().at(0).toString().contains(QStringLiteral("Rate limit exhausted")),
             qPrintable(second.first().at(0).toString()));
    QCOMPARE(server.hits.value(QStringLiteral("/latest")), 1);
}

QTEST_GUILESS_MAIN(TestUpdateChecker)
#include "test_update_checker.moc"
