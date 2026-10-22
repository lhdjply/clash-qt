// ---------------------------------------------------------------------------
//  检查更新
//
//  三部分：
//    1. 版本号比较 —— 「有没有更新」完全由它决定，判错会天天提示同一个版本；
//    2. 更新清单 —— 解析、缺文件、坏内容、页面地址只认本仓库；
//    3. 取法 —— 镜像前缀拼接、按顺序换镜像、抖动重试与对冲。
//  网络部分用本地 QTcpServer 冒充镜像，测试不联网、不依赖仓库是否发过版。
// ---------------------------------------------------------------------------

#include <QtTest>

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

        Response() = default;
        Response(QByteArray status_, QByteArray body_)
            : status(std::move(status_)), body(std::move(body_))
        {
        }
    };

    QTcpServer              server;
    QHash<QString, Response> routes;
    QHash<QString, int>     drops;  // 该路径接下来 N 次直接断开连接
    QHash<QString, int>     stalls; // 该路径接下来 N 次只接受连接、不应答
    mutable QHash<QString, int> hits; // 各路径实际收到的请求数

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
                    raw += "Content-Type: application/json\r\n";
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

// 清单里有版本、发布页、正文；不带 assets —— 它本身就是 Release 的资产
const char kManifestJson[] =
    R"({"version":"999.0.0","tag":"v999.0.0","name":"clash-qt v999.0.0",)"
    R"("page":"https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0",)"
    R"("notes":"## 更新内容\n\n- 侧栏改版",)"
    R"("published_at":"2026-10-22T02:30:00Z"})";

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
    void manifestMissingIsReported();
    void serverErrorIsReported();
    void brokenManifestIsNotReportedAsUpToDate();
    void transientFailureIsRetried();
    void stalledRequestIsHedged();
    void manifestGoesThroughMirrorPrefix();
    void mirrorsAreTriedInOrder();
    void manifestRejectsForeignPageUrl();
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
    // 清单里的版本本来就该是正式版；即便带了后缀，也只比数字部分，
    // 不会因为后缀把同一个版本判成「有更新」
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("0.1.0-rc1"),
                                            QStringLiteral("0.1.0")),
             0);
    QCOMPARE(UpdateChecker::compareVersions(QStringLiteral("0.2.0+build.7"),
                                            QStringLiteral("0.2.0")),
             0);
}

// ---------------------------------------------------------------------------
//  清单解析
// ---------------------------------------------------------------------------
void TestUpdateChecker::updateAvailableCarriesReleaseInfo()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kManifestJson)});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({}); // 直接打本地服务
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 5000);

    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(received.tagName, QStringLiteral("v999.0.0"));
    QCOMPARE(received.title, QStringLiteral("clash-qt v999.0.0"));
    QVERIFY(!received.notesIsHtml);
    QVERIFY(received.notes.contains(QStringLiteral("侧栏改版")));
    QCOMPARE(received.pageUrl.toString(),
             QStringLiteral("https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0"));
    QCOMPARE(received.publishedAt, QStringLiteral("2026-10-22T02:30:00Z"));
    QVERIFY(received.hasAssets);
}

void TestUpdateChecker::sameVersionReportsUpToDate()
{
    FakeServer server;
    // 旧版本：不该提示更新
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("200 OK"),
                          QByteArrayLiteral(R"({"version":"0.0.1","notes":"old"})")});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QSignalSpy spy(&checker, &UpdateChecker::upToDate);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 5000);
    QCOMPARE(spy.first().at(0).toString(), QLatin1String(CLASHQT_VERSION));
}

// 老版本 Release 里没有这个文件（或 latest 还没有正式版）：报错，不能当成
// 「已是最新」—— 那会让用户以为检查过了，其实什么都没读到
void TestUpdateChecker::manifestMissingIsReported()
{
    FakeServer server; // 未配置任何路径 → 404
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QSignalSpy upToDate(&checker, &UpdateChecker::upToDate);
    QSignalSpy failed(&checker, &UpdateChecker::failed);

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(failed.count(), 1, 5000);

    QCOMPARE(upToDate.count(), 0);
    QVERIFY2(failed.first().at(0).toString().contains(QStringLiteral("manifest")),
             qPrintable(failed.first().at(0).toString()));
    // 4xx 是确定性失败，单条地址不该反复重发
    QCOMPARE(server.hits.value(QStringLiteral("/latest.json")), 1);
}

void TestUpdateChecker::serverErrorIsReported()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("500 Internal Server Error"), QByteArray()});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QSignalSpy spy(&checker, &UpdateChecker::failed);
    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 8000);
    QVERIFY2(spy.first().at(0).toString().contains(QStringLiteral("500")),
             qPrintable(spy.first().at(0).toString()));
}

// 转发服务出错时回一页 HTML：不能因为「解析不出东西」就当「已是最新」
void TestUpdateChecker::brokenManifestIsNotReportedAsUpToDate()
{
    FakeServer server;
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("200 OK"),
                          QByteArrayLiteral("<html><body>Bad gateway</body></html>")});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QSignalSpy updated(&checker, &UpdateChecker::updateAvailable);
    QSignalSpy upToDate(&checker, &UpdateChecker::upToDate);
    QSignalSpy failed(&checker, &UpdateChecker::failed);

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(updated.count() + upToDate.count() + failed.count(), 1, 8000);

    QCOMPARE(upToDate.count(), 0);
    QCOMPARE(failed.count(), 1);
    QVERIFY2(failed.first().at(0).toString().contains(QStringLiteral("manifest")),
             qPrintable(failed.first().at(0).toString()));
}

// ---------------------------------------------------------------------------
//  取法：镜像、重试、对冲
// ---------------------------------------------------------------------------
// 第一次还没应答就断开 —— 实测 TUN 下 TLS 停滞、Connection closed 都是这种形态，
// 紧接着重试通常就成功，不该让整次检查失败
void TestUpdateChecker::transientFailureIsRetried()
{
    FakeServer server;
    server.drops.insert(QStringLiteral("/latest.json"), 1);
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kManifestJson)});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QSignalSpy  failedSpy(&checker, &UpdateChecker::failed);
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
    QCOMPARE(server.hits.value(QStringLiteral("/latest.json")), 2);
}

// 第一条连接挂住不应答（实测丢包时 TCP 重传会卡 2~5 秒）：应当靠并行的第二条
// 拿到结果，而不是干等超时
void TestUpdateChecker::stalledRequestIsHedged()
{
    FakeServer server;
    server.stalls.insert(QStringLiteral("/latest.json"), 1);
    server.routes.insert(QStringLiteral("/latest.json"),
                         {QByteArrayLiteral("200 OK"), QByteArray(kManifestJson)});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    QElapsedTimer clock;
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

    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(server.hits.value(QStringLiteral("/latest.json")), 2);
    QVERIFY2(clock.elapsed() < 4000,
             qPrintable(QStringLiteral("耗时 %1ms，对冲没有生效").arg(clock.elapsed())));
}

// 镜像是「前缀 + 完整的 github.com 地址」：请求路径里能看到原始 URL
void TestUpdateChecker::manifestGoesThroughMirrorPrefix()
{
    FakeServer mirror;
    const QUrl base(
        QStringLiteral("https://github.com/lhdjply/clash-qt/releases/latest/download/latest.json"));
    const QString proxiedPath = QLatin1Char('/') + base.toString();
    mirror.routes.insert(proxiedPath,
                         {QByteArrayLiteral("200 OK"), QByteArray(kManifestJson)});
    QVERIFY(mirror.start());

    UpdateChecker checker;
    checker.setManifestUrl(base);
    checker.setMirrorPrefixes({mirror.url(QString()).toString()});

    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 8000);

    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QCOMPARE(mirror.hits.value(proxiedPath), 1);
}

// 第一个镜像 5xx、第二个可用：应当换到第二个，而不是把整次检查判失败
void TestUpdateChecker::mirrorsAreTriedInOrder()
{
    const QUrl base(
        QStringLiteral("https://github.com/lhdjply/clash-qt/releases/latest/download/latest.json"));
    const QString proxiedPath = QLatin1Char('/') + base.toString();

    FakeServer first;
    first.routes.insert(proxiedPath,
                        {QByteArrayLiteral("502 Bad Gateway"), QByteArray()});
    QVERIFY(first.start());

    FakeServer second;
    second.routes.insert(proxiedPath,
                         {QByteArrayLiteral("200 OK"), QByteArray(kManifestJson)});
    QVERIFY(second.start());

    UpdateChecker checker;
    checker.setManifestUrl(base);
    checker.setMirrorPrefixes({first.url(QString()).toString(), second.url(QString()).toString()});

    QSignalSpy  failed(&checker, &UpdateChecker::failed);
    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 8000);

    QCOMPARE(failed.count(), 0);
    QCOMPARE(received.version, QStringLiteral("999.0.0"));
    QVERIFY(first.hits.value(proxiedPath) >= 1);
    QVERIFY(second.hits.value(proxiedPath) >= 1);
}

// 清单可能经过第三方转发：页面地址只认本仓库的发布页
void TestUpdateChecker::manifestRejectsForeignPageUrl()
{
    FakeServer server;
    server.routes.insert(
        QStringLiteral("/latest.json"),
        {QByteArrayLiteral("200 OK"),
         QByteArrayLiteral(R"({"version":"999.0.0","tag":"v999.0.0",)"
                           R"("page":"https://example.invalid/clash-qt/download",)"
                           R"("notes":"- 侧栏改版"})")});
    QVERIFY(server.start());

    UpdateChecker checker;
    checker.setMirrorPrefixes({});
    checker.setManifestUrl(server.url(QStringLiteral("/latest.json")));

    ReleaseInfo received;
    int         count = 0;
    QObject::connect(&checker, &UpdateChecker::updateAvailable,
                     [&](const ReleaseInfo &info) {
                         received = info;
                         ++count;
                     });

    QVERIFY(checker.check(0));
    QTRY_COMPARE_WITH_TIMEOUT(count, 1, 8000);

    // 换成按 tag 拼出的发布页，而不是照单全收
    QCOMPARE(received.pageUrl.toString(),
             QStringLiteral("https://github.com/lhdjply/clash-qt/releases/tag/v999.0.0"));
}

QTEST_GUILESS_MAIN(TestUpdateChecker)
#include "test_update_checker.moc"
