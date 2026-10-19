#include "updater/UpdateChecker.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QTimeZone>
#include <QTimer>
#include <QXmlStreamReader>

namespace clashqt {
namespace {

// 固定仓库。两条来源都取「列表」而不是 /latest：仓库里还有一个由 CI 维护的
// continuous_build 发布，它的 tag 不是版本号；/latest 会把它当最新版，导致永远
// 「查不到版本」。列表里我们自己挑第一条「tag 是版本号、且不是预发布」的发布。
constexpr char kReleasesApiUrl[] =
    "https://api.github.com/repos/lhdjply/clash-qt/releases?per_page=20";
constexpr char kReleasesAtomUrl[] =
    "https://github.com/lhdjply/clash-qt/releases.atom";

// 单次尝试的硬上限。正常响应 300~500ms，6 秒足以覆盖一次重传造成的 2~3 秒
// 停顿；再长也只是让用户多等 —— 已经有三条并行连接，不必再给单条更多耐心。
constexpr int kTimeoutMs = 6000;

// 对冲延迟：首条请求超过这个时间还没有任何结果，就并行再发一条。
// 取值介于「正常完成」与「明显卡住」之间 —— 正常情况不会多花一个请求。
constexpr int kHedgeDelayMs = 700;

// 一次检查的总预算：对冲、重试与切源都受它约束，避免「超时 × 尝试 × 来源」
// 叠加成用户无法接受的长尾。
constexpr int kTotalBudgetMs = 9000;

// 剩余预算低于此值就不再发起新的尝试（连 TCP 都不够握完）
constexpr int kMinBudgetMs = 2000;

// 每个来源最多发出的请求数：正常 1 条；卡住时每 700ms 补一条对冲，最多 3 条。
constexpr int kMaxAttempts = 3;

// "v0.2.0-rc1" / "0.2.0+build" → "0.2.0"：只留数字与点
QString normalizeVersion(QString version)
{
    version = version.trimmed();
    if (version.startsWith(QLatin1Char('v'), Qt::CaseInsensitive)) version.remove(0, 1);

    const int cut = version.indexOf(QRegularExpression(QStringLiteral("[^0-9.]")));
    if (cut >= 0) version = version.left(cut);
    while (version.endsWith(QLatin1Char('.'))) version.chop(1);
    return version;
}

QVector<int> versionParts(const QString &version)
{
    QVector<int> parts;
    const QStringList fields = version.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    parts.reserve(fields.size());
    for (const QString &field : fields) {
        bool ok = false;
        const int value = field.toInt(&ok);
        parts.append(ok ? value : 0);
    }
    return parts;
}

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : UpdateChecker(QUrl(QString::fromLatin1(kReleasesApiUrl)),
                    QUrl(QString::fromLatin1(kReleasesAtomUrl)), parent)
{
}

UpdateChecker::UpdateChecker(const QUrl &endpoint, const QUrl &atomEndpoint, QObject *parent)
    : QObject(parent), m_endpoint(endpoint), m_atomEndpoint(atomEndpoint)
{
}

UpdateChecker::~UpdateChecker()
{
    // 析构时还在飞：先掐断，避免 finished 落到已销毁的对象上
    m_running = false;
    cancelInflight();
}

int UpdateChecker::compareVersions(const QString &a, const QString &b)
{
    const QVector<int> left  = versionParts(normalizeVersion(a));
    const QVector<int> right = versionParts(normalizeVersion(b));

    const int count = qMax(left.size(), right.size());
    for (int i = 0; i < count; ++i) {
        const int l = i < left.size() ? left.at(i) : 0;
        const int r = i < right.size() ? right.at(i) : 0;
        if (l != r) return l > r ? 1 : -1;
    }
    return 0;
}

QVector<UpdateChecker::Source> UpdateChecker::sourceOrder() const
{
    QVector<Source> order;
    // Atom 在前：不受未认证 API 的 60 次/小时配额限制（该配额按出口 IP 计，
    // 走公共节点几乎必然已被别人用光，实测每次都 403）。
    if (!m_atomEndpoint.isEmpty()) order.append(Source::Atom);
    if (!m_endpoint.isEmpty()) order.append(Source::Api);
    return order;
}

QUrl UpdateChecker::sourceUrl(Source source) const
{
    return source == Source::Api ? m_endpoint : m_atomEndpoint;
}

QString UpdateChecker::sourceLabel(Source source)
{
    return source == Source::Api ? QStringLiteral("API") : QStringLiteral("Atom 订阅源");
}

bool UpdateChecker::check(int proxyPort)
{
    if (m_running) return false;

    if (!m_manager) m_manager = new QNetworkAccessManager(this);

    if (!m_hedgeTimer) {
        m_hedgeTimer = new QTimer(this);
        m_hedgeTimer->setSingleShot(true);
        // 每过一拍仍有请求没回来，就再并行补一条（上限见 kMaxAttempts）。
        // 实测链路是「单条连接偶尔卡 2~5 秒」，多条独立连接同时卡的概率低得多，
        // 因此补齐比死等一条更划算。正常情况首条在这一拍内就完成，不会补发。
        QObject::connect(m_hedgeTimer, &QTimer::timeout, this, [this]() {
            if (!m_running) return;
            if (m_attempts >= kMaxAttempts) return;
            if (remainingBudgetMs() < kMinBudgetMs) return;

            sendAttempt();
            if (m_attempts < kMaxAttempts) m_hedgeTimer->start(kHedgeDelayMs);
        });
    }

    // 每次检查都重设代理：内核可能在两次检查之间被启停，端口也可能被改过
    if (proxyPort > 0) {
        m_manager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                                          QStringLiteral("127.0.0.1"), quint16(proxyPort)));
    } else {
        m_manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    }

    m_apiError.clear();
    m_atomError.clear();
    m_stage   = 0;
    m_running = true;
    m_clock.start();
    cancelInflight(); // 防御性清理：正常情况下这里不该有残留

    if (!startSource(0)) {
        // 一个来源都发不出去（配额窗口 + 预算归零）：仍然异步回报，保持
        // 「check() 返回后结果通过信号到达」的一致行为
        m_running = false;
        const QString reason = failureMessage();
        QTimer::singleShot(0, this, [this, reason]() { emit failed(reason); });
        return true;
    }
    return true;
}

bool UpdateChecker::startSource(int stage)
{
    const QVector<Source> order = sourceOrder();

    for (int i = stage; i < order.size(); ++i) {
        const Source source = order.at(i);

        // 配额窗口内不必再白跑一次注定 403 的请求
        if (source == Source::Api && apiRateLimited()) {
            m_apiError = rateLimitReason();
            continue;
        }
        if (remainingBudgetMs() < kMinBudgetMs) {
            const QString skipped = QStringLiteral("总耗时超限，未尝试");
            if (source == Source::Api) m_apiError = skipped;
            else m_atomError = skipped;
            continue;
        }

        m_stage     = i;
        m_source    = source;
        m_attempts  = 0;

        sendAttempt();
        if (m_hedgeTimer) m_hedgeTimer->start(kHedgeDelayMs);
        return true;
    }
    return false;
}

void UpdateChecker::sendAttempt()
{
    if (!m_manager || m_attempts >= kMaxAttempts) return;
    ++m_attempts;

    QNetworkRequest request(sourceUrl(m_source));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("clash-qt/%1").arg(QLatin1String(CLASHQT_VERSION)));
    if (m_source == Source::Api) {
        request.setRawHeader("Accept", "application/vnd.github+json");
        request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    } else {
        // 只收 Atom：GitHub 会按 Accept 在 HTML 与订阅之间切换
        request.setRawHeader("Accept", "application/atom+xml");
    }
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = m_manager->get(request);
    m_inflight.append(reply);

    // 预算见底时用剩余时间当超时，避免最后一次尝试把总耗时拖出预算
    const int timeout = qBound(1000, remainingBudgetMs(), kTimeoutMs);
    QTimer::singleShot(timeout, this, [this, reply]() {
        if (m_inflight.contains(reply)) reply->abort();
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        // 只处理仍在名单里的请求：被 cancelInflight 放弃的那条不再进入状态机
        const bool alive = m_inflight.removeOne(reply);
        if (alive) finish(reply);
        reply->deleteLater();
    });
}

void UpdateChecker::cancelInflight()
{
    const QList<QNetworkReply *> replies = m_inflight;
    m_inflight.clear();
    if (m_hedgeTimer) m_hedgeTimer->stop();

    for (QNetworkReply *reply : replies) {
        if (!reply) continue;
        reply->disconnect(this); // 断开 finished，避免继续驱动状态机
        reply->abort();
        reply->deleteLater();
    }
}

void UpdateChecker::finish(QNetworkReply *reply)
{
    if (!m_running) return;

    // 顺序很重要：Qt 把 4xx/5xx 也当成 error（ContentNotFound / InternalServerError），
    // 先看 HTTP 状态码，否则「仓库还没发过 Release」会被误报成失败。
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    const bool timedOut = reply->error() == QNetworkReply::OperationCanceledError
                          || reply->error() == QNetworkReply::TimeoutError;
    const bool httpOk = status == 200;
    const bool netOk  = reply->error() == QNetworkReply::NoError;

    const Source source = m_source;

    // 仓库还没发过 Release / 没有订阅：对用户来说就是「没有更新」
    if (status == 404) {
        cancelInflight();
        m_running = false;
        emit upToDate(QLatin1String(CLASHQT_VERSION));
        return;
    }

    if (httpOk && netOk) {
        // 只有真正要解析时才读正文：错误/关闭的 reply 上 readAll 会打印
        // "device not open" 之类的警告，纯噪音
        const QByteArray payload = reply->readAll();

        // 结果已到手，对冲的那条不再需要
        cancelInflight();

        const bool done = (source == Source::Api) ? parseApi(payload) : parseAtom(payload);
        if (done) return;

        // 正文坏了（被截断、被劫持）：允许再试一次
        advanceAfterFailure(true);
        return;
    }

    if (source == Source::Api) noteRateLimit(reply);

    const QString reason = timedOut
                               ? QStringLiteral("请求超时")
                               : (status >= 400 ? QStringLiteral("HTTP %1").arg(status)
                                                : reply->errorString());
    if (source == Source::Api) m_apiError = reason;
    else m_atomError = reason;

    // 对冲请求还在飞：等它的结果，不要因为一条失败就判整个来源失败
    if (!m_inflight.isEmpty()) return;

    advanceAfterFailure(isTransient(reply, timedOut));
}

void UpdateChecker::advanceAfterFailure(bool retryable)
{
    // 抖动且还有余量：再发一条（4xx 这类确定性失败重发只会得到同样的结果）
    if (retryable && m_attempts < kMaxAttempts && remainingBudgetMs() >= kMinBudgetMs) {
        sendAttempt();
        return;
    }

    // 还有下一个来源就继续（Atom 失败 → API）；都试完了才报错
    if (startSource(m_stage + 1)) return;

    m_running = false;
    emit failed(failureMessage());
}

bool UpdateChecker::isTransient(QNetworkReply *reply, bool timedOut) const
{
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status >= 400 && status < 500) return false; // 确定性失败：限流、找不到、被拒
    if (status >= 500) return true;                  // 网关/服务端抖动

    if (timedOut) return true;

    switch (reply->error()) {
    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::NetworkSessionFailedError:
    case QNetworkReply::UnknownNetworkError:
    case QNetworkReply::SslHandshakeFailedError:
    case QNetworkReply::ProxyConnectionClosedError:
    case QNetworkReply::ProxyTimeoutError:
        return true;
    default:
        return false;
    }
}

void UpdateChecker::noteRateLimit(QNetworkReply *reply)
{
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 403) return;
    if (reply->rawHeader("x-ratelimit-remaining").trimmed() != QByteArrayLiteral("0")) return;

    const qint64 reset = reply->rawHeader("x-ratelimit-reset").trimmed().toLongLong();
    const QDateTime now = QDateTime::currentDateTimeUtc();

    // reset 是 UTC 的秒级时间戳；用 QTimeZone::UTC 而不是已弃用的 Qt::TimeSpec 重载
    QDateTime until =
        reset > 0 ? QDateTime::fromSecsSinceEpoch(reset, QTimeZone::UTC) : QDateTime();
    // 时钟偏差或上游给的时间已过期：保守地只跳过 1 分钟
    if (!until.isValid() || until <= now) until = now.addSecs(60);
    // 配额窗口是 1 小时，明显超出这个范围的重置时间不可信
    const QDateTime cap = now.addSecs(3600);
    if (until > cap) until = cap;

    m_apiBlockedUntil = until;
}

bool UpdateChecker::apiRateLimited() const
{
    return m_apiBlockedUntil.isValid() && QDateTime::currentDateTimeUtc() < m_apiBlockedUntil;
}

QString UpdateChecker::rateLimitReason() const
{
    const qint64 seconds =
        qMax<qint64>(1, QDateTime::currentDateTimeUtc().secsTo(m_apiBlockedUntil));
    // 配额按出口 IP 计：换节点后出口 IP 变了，额度可能又有了，所以别说得太绝对
    return QStringLiteral("配额已用尽（约 %1 分钟后重置，当前节点可能共享该出口 IP）")
        .arg((seconds + 59) / 60);
}

int UpdateChecker::remainingBudgetMs() const
{
    if (!m_clock.isValid()) return kTotalBudgetMs;
    return kTotalBudgetMs - int(m_clock.elapsed());
}

QString UpdateChecker::failureMessage() const
{
    const bool hasAtom = !m_atomEndpoint.isEmpty();
    const bool hasApi  = !m_endpoint.isEmpty();

    if (hasAtom && hasApi) {
        return QStringLiteral("%1：%2；%3：%4")
            .arg(sourceLabel(Source::Atom),
                 m_atomError.isEmpty() ? QStringLiteral("未尝试") : m_atomError,
                 sourceLabel(Source::Api),
                 m_apiError.isEmpty() ? QStringLiteral("未尝试") : m_apiError);
    }
    if (hasAtom) return m_atomError;
    return m_apiError.isEmpty() ? QStringLiteral("没有可用的更新源") : m_apiError;
}

bool UpdateChecker::parseApi(const QByteArray &payload)
{
    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
    if (!document.isArray() && !document.isObject()) {
        m_apiError = QStringLiteral("返回内容无法解析：%1").arg(parseError.errorString());
        return false;
    }

    // /releases 返回数组；单条对象（/latest）也兼容，便于换端点
    QList<QJsonObject> releases;
    if (document.isArray()) {
        for (const QJsonValue &value : document.array()) {
            if (value.isObject()) releases.append(value.toObject());
        }
    } else {
        releases.append(document.object());
    }

    for (const QJsonObject &object : releases) {
        if (object.value(QStringLiteral("draft")).toBool()) continue;
        if (object.value(QStringLiteral("prerelease")).toBool()) continue;

        const QString tag = object.value(QStringLiteral("tag_name")).toString();
        const QString version = normalizeVersion(tag);
        if (version.isEmpty()) continue; // continuous_build 之类：不是版本发布

        ReleaseInfo info;
        info.tagName     = tag;
        info.version     = version;
        info.title       = object.value(QStringLiteral("name")).toString();
        info.notes       = object.value(QStringLiteral("body")).toString();
        info.notesIsHtml = false;
        info.pageUrl     = QUrl(object.value(QStringLiteral("html_url")).toString());
        info.publishedAt = object.value(QStringLiteral("published_at")).toString();
        report(info);
        return true;
    }

    // 一条版本发布都没有（只有持续构建）
    m_running = false;
    emit upToDate(QLatin1String(CLASHQT_VERSION));
    return true;
}

bool UpdateChecker::parseAtom(const QByteArray &payload)
{
    struct Entry {
        QString title;
        QString updated;
        QString link;
        QString content;
    };

    QXmlStreamReader xml(payload);
    QList<Entry>     entries;
    Entry            current;
    bool             inEntry = false;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.isStartElement()) {
            const QString name = xml.name().toString();
            if (name == QLatin1String("entry")) {
                current = Entry{};
                inEntry = true;
            } else if (inEntry) {
                if (name == QLatin1String("title")) {
                    current.title = xml.readElementText();
                } else if (name == QLatin1String("updated")) {
                    current.updated = xml.readElementText();
                } else if (name == QLatin1String("link")
                           && xml.attributes().value(QLatin1String("rel"))
                                  == QLatin1String("alternate")) {
                    current.link = xml.attributes().value(QLatin1String("href")).toString();
                } else if (name == QLatin1String("content")) {
                    // type="html" 的正文是转义后的 HTML，readElementText 读出即还原
                    current.content = xml.readElementText();
                }
            }
        } else if (xml.isEndElement() && xml.name().toString() == QLatin1String("entry")) {
            if (inEntry) entries.append(current);
            inEntry = false;
        }
    }

    if (xml.hasError()) {
        m_atomError = QStringLiteral("内容无法解析：%1").arg(xml.errorString());
        return false;
    }

    // 与 API 路径同一套规则：tag 取自链接末尾（.../releases/tag/v0.2.0），
    // 不是版本号的（continuous_build）与预发布（v0.2.0-rc1）都跳过。
    for (const Entry &entry : entries) {
        const QString tag = entry.link.section(QLatin1Char('/'), -1).trimmed();
        if (tag.isEmpty() || tag.contains(QLatin1Char('-'))) continue;

        const QString version = normalizeVersion(tag);
        if (version.isEmpty()) continue;

        ReleaseInfo info;
        info.tagName     = tag;
        info.version     = version;
        info.title       = entry.title;
        info.notes       = entry.content;
        info.notesIsHtml = true;
        info.pageUrl     = QUrl(entry.link);
        info.publishedAt = entry.updated;
        report(info);
        return true;
    }

    // 只有持续构建之类的非版本发布
    m_running = false;
    emit upToDate(QLatin1String(CLASHQT_VERSION));
    return true;
}

void UpdateChecker::report(const ReleaseInfo &release)
{
    m_running = false;
    if (compareVersions(release.version, QLatin1String(CLASHQT_VERSION)) > 0) {
        emit updateAvailable(release);
    } else {
        emit upToDate(QLatin1String(CLASHQT_VERSION));
    }
}

} // namespace clashqt
