#pragma once

// ---------------------------------------------------------------------------
//  DNS 解析器
//
//  对应参照实现的 component/resolver + dns/ 及其 dns 配置段。
//  能力范围：
//    * hosts 静态映射（含通配 "*." 前缀）
//    * nameserver / default-nameserver / fallback 多服务器查询（UDP、TCP）
//    * DoH（https:// 前缀）通过 QNetworkAccessManager 查询
//    * 结果缓存与 TTL 过期
//    * enhanced-mode: fake-ip 的地址池分配与反查
//    * nameserver-policy 按域名前缀选择服务器组
//
//  线程模型：resolve 系列方法内部运行局部事件循环（QEventLoop）等待应答，
//  因此必须在工作线程中调用，不能在 GUI 线程直接调用——这与每个连接由
//  独立工作线程处理的监听器设计一致（见 listener/）。fake-ip 命中时不发起
//  网络查询，可直接返回。
//
//  ---- 线程安全（重要）------------------------------------------------------
//  同一个 Resolver 会被下面这些线程**同时**访问：
//    · TUN 数据面线程：DNS 53 劫持 → mappingFakeIp / fakeIpToHost
//    · 每个连接一个的拨号线程：DIRECT / MATCH 兜底 → resolveReal → 写缓存
//    · GUI 线程：applyDnsConfig（换订阅 / 改设置）、fake-ip 判断
//  内部容器（缓存 QHash、fake-ip 双向表、hosts、配置字符串）**全部**由
//  m_mutex 保护。缺了这把锁时的表现是：两个线程同时往 QHash 里插入触发
//  rehash，堆被写坏 → 随机时刻 SIGSEGV / SIGABRT 闪退（TUN 开着跑一会儿
//  就崩，且栈上看不到任何业务代码）。锁的粒度只覆盖容器读写，**绝不**
//  包括网络等待，否则一次 DoH 超时会把数据面卡死 5 秒。
// ---------------------------------------------------------------------------

#include "constant/Constants.h"

#include <QHash>
#include <QHostAddress>
#include <QJsonObject>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QString>
#include <QStringList>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

class QNetworkAccessManager;

namespace core {

// ---------------------------------------------------------------------------
//  DNS 报文编解码（内部使用，暴露出来便于单元测试）
// ---------------------------------------------------------------------------

namespace dns {

struct Question {
    QString name;
    quint16 type = 1; // 1=A 28=AAAA
    quint16 cls  = 1; // IN
};

struct Answer {
    QString     name;
    quint16     type = 0;
    quint32     ttl  = 0;
    QHostAddress address;
    QString      text; // CNAME 等文本记录
};

struct Message {
    quint16             id = 0;
    bool                isResponse = false;
    bool                truncated = false;
    int                 rcode = 0;
    QList<Question>     questions;
    QList<Answer>       answers;

    bool isValid() const { return m_valid; }
    void setValid(bool valid) { m_valid = valid; }

private:
    bool m_valid = false;
};

// 构造 A / AAAA 查询报文
QByteArray buildQuery(quint16 id, const QString &name, quint16 type);

// 构造应答报文：id / 问题段按查询原样回带（问题名用指针压缩），
// answers 里的 A / AAAA / CNAME 会被写入应答段，其余类型忽略。
// 供 DnsServer 与 TUN 的 53 端口劫持使用。
QByteArray buildResponse(const Message &query, const QList<Answer> &answers, int rcode = 0);

// 解析应答报文
std::optional<Message> parseMessage(const QByteArray &payload);

// 生成随机查询 id
quint16 randomId();

} // namespace dns

// ---------------------------------------------------------------------------
//  FakeIp 地址池
// ---------------------------------------------------------------------------

class FakeIpPool {
public:
    FakeIpPool() = default;

    // 从 fake-ip-range 初始化；非法时使用 198.18.0.1/16
    void configure(const QString &range);

    // 为域名分配（或复用）一个假 IP
    QHostAddress allocate(const QString &host);
    // 由假 IP 反查域名
    std::optional<QString> reverseLookup(const QHostAddress &address) const;
    // 判断地址是否落在池内
    bool contains(const QHostAddress &address) const;
    // 该地址是否是本池分配的假 IP
    bool isFakeIp(const QHostAddress &address) const { return contains(address); }

    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    int  size() const { return m_forward.size(); }
    void clear();

private:
    bool   m_enabled = false;
    quint32 m_base    = 0;
    quint32 m_mask    = 0;
    quint32 m_next    = 0;
    quint32 m_end     = 0;

    // 域名 <-> 假 IP 双向映射
    QHash<QString, quint32> m_forward;
    QHash<quint32, QString> m_reverse;
};

// ---------------------------------------------------------------------------
//  Resolver
// ---------------------------------------------------------------------------

class Resolver : public QObject {
    Q_OBJECT

public:
    explicit Resolver(QObject *parent = nullptr);
    ~Resolver() override;

    // 依据配置树中的 dns 段应用设置。参数是参照实现 dns 配置的对象表示。
    void applyDnsConfig(const QJsonObject &dnsConfig);

    // ---- 解析 -------------------------------------------------------------

    // 返回解析出的地址列表；失败返回空列表
    QList<QHostAddress> resolve(const QString &host,
                                int timeoutMs = 5000,
                                bool ipv6 = false);

    // 只做真实解析，不做 fake-ip 映射。
    // 用于 DIRECT 出站等「必须拿到真实 IP」的场景：fake-ip 只是给入站客户端
    // 的占位地址，把它当成真实目标地址去连接会失败。
    QList<QHostAddress> resolveReal(const QString &host,
                                    int timeoutMs = 5000,
                                    bool ipv6 = false);

    // 只取第一个地址（常用路径）
    std::optional<QHostAddress> resolveOne(const QString &host,
                                           int timeoutMs = 5000,
                                           bool ipv6 = false);

    // ---- fake-ip ----------------------------------------------------------

    // 命中 fake-ip 时返回假 IP（不发网络请求）；否则返回 nullopt
    std::optional<QHostAddress> mappingFakeIp(const QString &host);
    std::optional<QString>      fakeIpToHost(const QHostAddress &address) const;
    bool                        isFakeIp(const QHostAddress &address) const;

    // ---- hosts ------------------------------------------------------------

    // 查询 hosts 静态映射；unqualified 表示 host 本身就是 IP 文本
    std::optional<QHostAddress> lookupHosts(const QString &host, bool ipv6 = false) const;

    // 该域名是否被 fake-ip-filter 排除（排除项走真实解析）
    bool shouldSkipFakeIp(const QString &host) const;

    // DNS 上游自身用到的域名（如 DoH 的 doh.pub）。这些名字必须走**引导解析**
    // 并绕过 fake-ip：否则内核解析 DoH 服务器地址时会拿到假 IP，再连回自己的
    // TUN，形成自锁（界面正常但网页全打不开）。
    QSet<QString> upstreamHosts() const;

    // 代理节点自身的服务器域名（如 unamecf.example.com）。TUN 模式下它们如果被
    // 分配假 IP，连节点这一步就会连回本机 TUN —— 表现为「所有代理流量都不通」。
    // 由 Hub 在装载配置时告知。
    void setServerHosts(const QStringList &hosts);

    // ---- 缓存 -------------------------------------------------------------

    void clearCache();
    int  cacheSize() const;

signals:
    // 内核日志（DNS 查询详情），由 Hub 转发到日志流
    void logMessage(const QString &level, const QString &message);

private:
    struct CacheEntry {
        QList<QHostAddress> addresses;
        qint64              expireAtMs = 0;
    };

    struct Server {
        QString host;
        int     port = 53;
        bool    tls  = false;
        QString httpsUrl; // 非空表示 DoH
        bool    tcp = false;
    };

    static std::optional<Server> parseServer(const QString &spec);
    QStringList selectNameservers(const QString &host, bool fallback) const;

    // ---- 以下 *Locked / cacheXxx 辅助函数内部自己加锁，
    //      供 resolveImpl 这类「锁外跑网络、锁内读表」的路径使用 ----
    bool shouldSkipFakeIpLocked(const QString &host) const; // 调用方已持锁
    bool isUpstreamHost(const QString &host) const;         // 内部加锁
    std::optional<CacheEntry> cacheLookup(const QString &key) const; // 内部加锁，已过期返回空
    void cacheStore(const QString &key, const CacheEntry &entry);    // 内部加锁
    // 取配置副本：查询过程中容器可能被 applyDnsConfig 整体替换
    QStringList defaultNameservers() const;
    QStringList fallbackServers() const;

    // 解析实现；allowFakeIp 控制是否启用 fake-ip 映射
    QList<QHostAddress> resolveImpl(const QString &host, int timeoutMs, bool ipv6,
                                    bool allowFakeIp);

    // 单次查询：向一个服务器发起 A/AAAA 查询
    QList<QHostAddress> queryServer(const Server &server,
                                    const QString &host,
                                    quint16 type,
                                    int timeoutMs);

    QList<QHostAddress> queryUdp(const Server &server, const QString &host,
                                 quint16 type, int timeoutMs);
    QList<QHostAddress> queryTcp(const Server &server, const QString &host,
                                 quint16 type, int timeoutMs);
    QList<QHostAddress> queryHttps(const Server &server, const QString &host,
                                   quint16 type, int timeoutMs);

    // QNetworkAccessManager 有线程亲和性：在 A 线程创建、B 线程使用会让 Qt 报
    // 「Cannot create children for a parent that is in a different thread」，
    // 甚至崩溃。TUN 的 DNS 解析跑在工作线程，所以按线程各留一个。
    static QNetworkAccessManager *networkForCurrentThread();

    void remember(const QString &host, int type, const QList<QHostAddress> &addresses, quint32 ttl);

    bool m_ipv6 = false;
    DnsEnhancedMode m_enhancedMode = DnsEnhancedMode::Normal;

    QStringList m_defaultNameservers;
    QStringList m_nameservers;
    QStringList m_fallback;
    QStringList m_proxyServerNameservers;
    QStringList m_directNameservers;

    // nameserver-policy: 域名后缀 -> 服务器列表
    QList<QPair<QString, QStringList>> m_nameserverPolicy;

    // hosts 段：域名 -> 地址（保留通配前缀信息）
    QHash<QString, QString> m_hosts;
    // fake-ip-filter
    QStringList m_fakeIpFilter;
    QSet<QString> m_upstreamHosts;
    QSet<QString> m_serverHosts;
    FilterMode  m_fakeIpFilterMode = FilterMode::Blacklist;

    FakeIpPool m_fakeIp;

    QHash<QString, CacheEntry> m_cache;

    // 保护上面所有容器与配置字段（见头部的线程安全说明）。
    // 用 QMutex 而非 QRecursiveMutex 是刻意的：加锁路径只做容器读写，
    // 一旦出现「持锁再调公开方法」的写法就会直接死锁暴露出来。
    mutable QMutex m_mutex;
};

} // namespace core
