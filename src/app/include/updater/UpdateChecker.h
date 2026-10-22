#pragma once

// ---------------------------------------------------------------------------
//  检查更新
//
//  数据源是 GitHub Releases，Atom 订阅优先、API 兜底：
//      https://github.com/lhdjply/clash-qt/releases.atom              （XML）
//      https://api.github.com/repos/lhdjply/clash-qt/releases?per_page=20 （JSON）
//
//  为什么 Atom 在前：未认证的 API 每小时只有 60 次配额，而且按**出口 IP** 计 ——
//  走公共节点时这个额度几乎总是被别人用光，实测每次都拿到 403。Atom 订阅没有
//  这个限制，字段（版本、链接、时间、正文）也够用，正文是 HTML 而非 Markdown，
//  由 UpdateDialog 按 notesIsHtml 选择解析器。
//
//  两条补充措施，针对「TUN 模式下几乎必失败」的实测现象：
//    1. 抖动对冲：到节点这条链路实测有约 20% 的连接会卡 2~5 秒（TCP 重传），
//       单条请求一旦撞上就要等到超时。因此首条请求每 700ms 还没结果就并行
//       再补一条（最多 3 条），谁先成功用谁 —— 尾延迟从「等超时」变成
//       「等对冲」。正常情况（300~500ms）只发一条，不产生额外请求。
//    2. 限流记忆：API 明确返回 403 且 x-ratelimit-remaining 为 0 时，记住
//       重置时间，窗口内不再白跑一次注定 403 的请求。
//
//  只做「查询 + 展示」两件事：不下载、不安装。安装包由用户自己到发布页取 ——
//  在缺少签名校验与包管理器的前提下让程序替换自己的二进制，风险远大于收益。
//
//  第三件要防的事：**tag 一推上去，Atom 里就有条目了**。GitHub 会把还没建 Release
//  的 tag 也挂进 releases.atom，标题就是 tag 名、正文就是 tag 说明；此时 CI 还在
//  编译，安装包一个都没有。这种占位条目不能当更新提示出去 —— 用户点开会看到一篇
//  只有版本号的「更新日志」，发布页上也没有可下的包。因此 Atom 命中版本更新后先
//  判一下形态（见 looksLikePlaceholder），像占位条目就回 API 查这个 tag 有没有真
//  正的 Release：404 说明确实还没建，这一次静默略过。
//
//  网络默认走本机混合端口（HTTP 代理）：GitHub 在很多网络环境直连不通，而
//  用户开着代理时它一定可用；内核没起时退回直连（例如系统级代理已生效）。
//  失败必须给出可读原因，不静默 —— 「没动静」最容易被理解成「没有更新」。
// ---------------------------------------------------------------------------

#include <QDateTime>
#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVector>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

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
    // 是否已经附带安装包。Atom 不提供这个信息，按「有」处理；只有 API 明确给出
    // 空的资产列表时才置 false，用于提示「包还在传」。
    bool    hasAssets = true;
};

class UpdateChecker : public QObject {
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);
    // 指定 API 与 Atom 地址，仅供测试与自建镜像。
    // atomEndpoint 为空表示只走 API（不切源）；endpoint 为空表示只走 Atom。
    UpdateChecker(const QUrl &endpoint, const QUrl &atomEndpoint, QObject *parent = nullptr);
    ~UpdateChecker() override;

    // proxyPort > 0：经 127.0.0.1:<port> 的 HTTP 代理访问；<=0 表示直连。
    // 同一时刻只允许一次检查（含重试与切源的间隔），重复调用直接忽略（返回 false）。
    bool check(int proxyPort);
    // 是否仍在进行中。重试等待期间 m_reply 为空，所以不能只看 m_reply。
    bool busy() const { return m_running; }

    // 比较版本号：只比数字段（0.10.0 > 0.9.9），忽略 v 前缀、预发布后缀与
    // 缺失的段（1.2 == 1.2.0）。返回 >0 表示 a 比 b 新。
    static int compareVersions(const QString &a, const QString &b);

signals:
    void upToDate(const QString &currentVersion);
    void updateAvailable(const ReleaseInfo &release);
    void failed(const QString &reason);

private:
    // 数据来源：Atom 订阅优先，API 兜底
    enum class Source { Atom, Api };

    // 按优先级排好的可用来源：Atom 在前，API 在后
    QVector<Source> sourceOrder() const;
    QUrl            sourceUrl(Source source) const;
    static QString  sourceLabel(Source source);

    // 从第 stage 个来源开始尝试；已无可用来源时返回 false
    bool startSource(int stage);
    // 为当前来源发一条请求。同一来源最多同时有 2 条在飞（原始 + 对冲）
    void sendAttempt();

    // 单条请求结束：成功则收下（并掐掉对冲），失败则按重试/切源规则推进
    void finish(QNetworkReply *reply);
    // 当前来源这一轮失败后的去向：再试 / 换源 / 报错
    void advanceAfterFailure(bool retryable);
    // 放弃当前所有在飞请求（已被采纳的结果会产生重复，也会留下无主连接）
    void cancelInflight();

    // 偶发抖动？4xx 属于确定性失败，不算
    bool isTransient(QNetworkReply *reply, bool timedOut) const;
    // 记录 API 的配额窗口：403 且 remaining 为 0 时按 x-ratelimit-reset 跳过
    void noteRateLimit(QNetworkReply *reply);
    bool apiRateLimited() const;
    QString rateLimitReason() const;

    int  remainingBudgetMs() const;
    QString failureMessage() const;

    // 解析成功并已发出结果时返回 true；内容坏了返回 false，由调用方切下一个来源
    bool parseApi(const QByteArray &payload);
    bool parseAtom(const QByteArray &payload);
    void report(const ReleaseInfo &release);

    // 占位条目（tag 已推、Release 还没建）再回 API 确认一次，确认不到就当没有更新
    void verifyPlaceholder(const ReleaseInfo &info);
    // .../releases?per_page=20 → .../releases/tags/<tag>
    QUrl releaseTagsUrl(const QString &tag) const;

    QUrl                   m_endpoint;     // GitHub API
    QUrl                   m_atomEndpoint; // GitHub Atom 订阅
    Source                 m_source = Source::Atom;
    int                    m_stage = 0;       // 当前是第几个来源
    int                    m_attempts = 0;    // 当前来源已发出的请求数
    bool                   m_running = false;
    QElapsedTimer          m_clock;           // 本次检查的耗时，用于总预算
    QString                m_apiError;        // 各来源的失败原因，最后一起报出
    QString                m_atomError;
    QDateTime              m_apiBlockedUntil; // API 配额恢复时间（UTC）
    QNetworkAccessManager *m_manager = nullptr;
    QList<QNetworkReply *> m_inflight;        // 当前来源在飞的请求（1~2 条）
    QTimer                *m_hedgeTimer = nullptr;
    QNetworkReply         *m_verifyReply = nullptr; // 占位条目的确认请求（独立于上面的状态机）
};

} // namespace clashqt
