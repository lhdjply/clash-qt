#include "updater/UpdateChecker.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QTimer>
#include <QXmlStreamReader>

#include <QVector>

namespace clashqt {
namespace {

// 固定仓库：API 给结构化数据（能排除预发布版），Atom 作为兜底
// 取列表而不是 /latest：仓库里还有一个由 CI 维护的 continuous_build 发布，
// 它的 tag 不是版本号；/latest 会把它当最新版，导致永远「查不到版本」。
// 列表里我们自己挑第一条「tag 是版本号、且不是预发布」的发布。
constexpr char kReleasesApiUrl[] =
    "https://api.github.com/repos/lhdjply/clash-qt/releases?per_page=20";
constexpr char kReleasesAtomUrl[] =
    "https://github.com/lhdjply/clash-qt/releases.atom";

// 单次请求的超时。API 失败后还要再试 Atom，所以取 10 秒：最坏 20 秒内有结论。
constexpr int kTimeoutMs = 10000;

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
    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->abort();
        m_reply = nullptr;
    }
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

bool UpdateChecker::check(int proxyPort)
{
    if (m_reply) return false;

    if (!m_manager) m_manager = new QNetworkAccessManager(this);

    // 每次检查都重设代理：内核可能在两次检查之间被启停，端口也可能被改过
    if (proxyPort > 0) {
        m_manager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                                          QStringLiteral("127.0.0.1"), quint16(proxyPort)));
    } else {
        m_manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    }

    m_apiError.clear();
    startRequest(m_endpoint, Source::Api);
    return true;
}

void UpdateChecker::startRequest(const QUrl &url, Source source)
{
    m_source = source;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("clash-qt/%1").arg(QLatin1String(CLASHQT_VERSION)));
    if (source == Source::Api) {
        request.setRawHeader("Accept", "application/vnd.github+json");
        request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    } else {
        request.setRawHeader("Accept", "application/atom+xml");
    }
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    m_reply = m_manager->get(request);
    QNetworkReply *reply = m_reply;

    QTimer::singleShot(kTimeoutMs, this, [this, reply]() {
        if (m_reply == reply) reply->abort();
    });

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (m_reply == reply) m_reply = nullptr;
        finish(reply);
        reply->deleteLater();
    });
}

bool UpdateChecker::retryWithAtom()
{
    if (m_atomEndpoint.isEmpty()) return false;
    startRequest(m_atomEndpoint, Source::Atom);
    return true;
}

void UpdateChecker::finish(QNetworkReply *reply)
{
    // 顺序很重要：Qt 把 4xx/5xx 也当成 error（ContentNotFound / InternalServerError），
    // 先看 HTTP 状态码，否则「仓库还没发过 Release」会被误报成失败。
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    const bool timedOut = reply->error() == QNetworkReply::OperationCanceledError;
    const bool httpOk   = status == 200;
    const bool netOk    = reply->error() == QNetworkReply::NoError;

    if (m_source == Source::Api) {
        // 仓库还没发过 Release：对用户来说就是「没有更新」
        if (status == 404) {
            emit upToDate(QLatin1String(CLASHQT_VERSION));
            return;
        }

        if (!httpOk || !netOk) {
            m_apiError = timedOut ? QStringLiteral("请求超时")
                                  : (status >= 400 ? QStringLiteral("HTTP %1").arg(status)
                                                   : reply->errorString());
            // 403 多半是按出口 IP 计的配额用完了（公共节点很常见），换 Atom 再试
            if (retryWithAtom()) return;
            emit failed(m_apiError);
            return;
        }

        // 只有真正要解析时才读正文：错误/关闭的 reply 上 readAll 会打印
        // "device not open" 之类的警告，纯噪音
        parseApi(reply->readAll());
        return;
    }

    // ---- Atom 兜底 ----
    if (status == 404) {
        emit upToDate(QLatin1String(CLASHQT_VERSION));
        return;
    }
    if (!httpOk || !netOk) {
        const QString reason = timedOut ? QStringLiteral("请求超时")
                                        : (status >= 400 ? QStringLiteral("HTTP %1").arg(status)
                                                         : reply->errorString());
        emit failed(QStringLiteral("GitHub API %1，备用源也失败：%2")
                        .arg(m_apiError.isEmpty() ? QStringLiteral("不可用") : m_apiError,
                             reason));
        return;
    }

    parseAtom(reply->readAll());
}

void UpdateChecker::parseApi(const QByteArray &payload)
{
    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
    if (!document.isArray() && !document.isObject()) {
        emit failed(QStringLiteral("返回内容不是合法的 JSON：%1").arg(parseError.errorString()));
        return;
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
        return;
    }

    // 一条版本发布都没有（只有持续构建）
    emit upToDate(QLatin1String(CLASHQT_VERSION));
}

void UpdateChecker::parseAtom(const QByteArray &payload)
{
    struct Entry {
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
                if (name == QLatin1String("updated")) {
                    current.updated = xml.readElementText();
                } else if (name == QLatin1String("link")
                           && xml.attributes().value(QLatin1String("rel"))
                                  == QLatin1String("alternate")) {
                    current.link = xml.attributes().value(QLatin1String("href")).toString();
                } else if (name == QLatin1String("content")) {
                    current.content = xml.readElementText(); // 转义后的 HTML，读出即还原
                }
            }
        } else if (xml.isEndElement() && xml.name().toString() == QLatin1String("entry")) {
            if (inEntry) entries.append(current);
            inEntry = false;
        }
    }

    if (xml.hasError()) {
        emit failed(QStringLiteral("备用源内容无法解析：%1").arg(xml.errorString()));
        return;
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
        info.notes       = entry.content;
        info.notesIsHtml = true;
        info.pageUrl     = QUrl(entry.link);
        info.publishedAt = entry.updated;
        report(info);
        return;
    }

    // 只有持续构建之类的非版本发布
    emit upToDate(QLatin1String(CLASHQT_VERSION));
}

void UpdateChecker::report(const ReleaseInfo &release)
{
    if (compareVersions(release.version, QLatin1String(CLASHQT_VERSION)) > 0) {
        emit updateAvailable(release);
    } else {
        emit upToDate(QLatin1String(CLASHQT_VERSION));
    }
}

} // namespace clashqt
