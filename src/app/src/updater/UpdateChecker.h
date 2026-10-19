#pragma once

// ---------------------------------------------------------------------------
//  检查更新
//
//  数据源是 GitHub Releases，API 优先、Atom 兜底：
//      https://api.github.com/repos/lhdjply/clash-qt/releases/latest   （JSON）
//      https://github.com/lhdjply/clash-qt/releases.atom              （XML）
//  为什么要兜底：未认证的 API 每小时只有 60 次配额，而且是**按出口 IP** 计的 ——
//  走公共节点时经常直接拿到 403（实测就撞上了）。Atom 订阅没有这个限制，
//  只是正文是 HTML、且不区分预发布版，因此只在前者不可用时才用。
//
//  只做「查询 + 展示」两件事：不下载、不安装。安装包由用户自己到发布页取 ——
//  在缺少签名校验与包管理器的前提下让程序替换自己的二进制，风险远大于收益。
//
//  网络默认走本机混合端口（HTTP 代理）：GitHub 在很多网络环境直连不通，而
//  用户开着代理时它一定可用；内核没起时退回直连（例如系统级代理已生效）。
//  失败必须给出可读原因，不静默 —— 「没动静」最容易被理解成「没有更新」。
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

namespace clashqt {

// 一条 Release 里本功能用得到的字段
struct ReleaseInfo {
    QString version;     // 规范化后的版本号（去掉 tag 的 v 前缀与预发布后缀）
    QString tagName;     // 原始 tag，展示时保留作者的写法（如 v0.2.0）
    QString title;       // Release 标题，可能为空
    QString notes;       // 更新日志正文：API 是 Markdown，Atom 是 HTML
    bool    notesIsHtml = false;
    QUrl    pageUrl;     // html_url：浏览器打开后可看到安装包
    QString publishedAt; // ISO 8601 时间
};

class UpdateChecker : public QObject {
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);
    // 指定 API 与备用（Atom）地址，仅供测试与自建镜像。
    // atomEndpoint 为空表示失败后不兜底。
    UpdateChecker(const QUrl &endpoint, const QUrl &atomEndpoint, QObject *parent = nullptr);
    ~UpdateChecker() override;

    // proxyPort > 0：经 127.0.0.1:<port> 的 HTTP 代理访问；<=0 表示直连。
    // 同一时刻只允许一个请求，重复调用直接忽略（返回 false）。
    bool check(int proxyPort);
    bool busy() const { return m_reply != nullptr; }

    // 比较版本号：只比数字段（0.10.0 > 0.9.9），忽略 v 前缀、预发布后缀与
    // 缺失的段（1.2 == 1.2.0）。返回 >0 表示 a 比 b 新。
    static int compareVersions(const QString &a, const QString &b);

signals:
    void upToDate(const QString &currentVersion);
    void updateAvailable(const ReleaseInfo &release);
    void failed(const QString &reason);

private:
    // 数据来源：先问 API，失败再退到 Atom
    enum class Source { Api, Atom };

    void startRequest(const QUrl &url, Source source);
    void finish(QNetworkReply *reply);
    void parseApi(const QByteArray &payload);
    void parseAtom(const QByteArray &payload);
    void report(const ReleaseInfo &release);
    bool retryWithAtom();

    QUrl                   m_endpoint;
    QUrl                   m_atomEndpoint; // 为空表示不兜底（测试用的自定义地址）
    QString                m_apiError;    // API 失败原因，兜底也失败时一起报出
    Source                 m_source = Source::Api;
    QNetworkAccessManager *m_manager = nullptr;
    QNetworkReply         *m_reply   = nullptr;
};

} // namespace clashqt
