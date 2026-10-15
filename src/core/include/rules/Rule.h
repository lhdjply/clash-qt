#pragma once

// ---------------------------------------------------------------------------
//  规则引擎
//
//  对应参照实现 rules/ 与 rules/common/。结构对应关系：
//
//    参照实现                              clash-qt
//    ----------------------------------  ------------------------------------
//    C.Metadata（连接元数据）             Metadata
//    rules.Rule 接口                      Rule
//    rules/common/domain.go 等            DomainRule / DomainSuffixRule / ...
//    rules/logic/logic.go                 LogicRule
//    rules/common/rule_set.go             RuleSetRule + RuleSet
//    config.parseRules                    RuleParser::parse
//
//  一条规则由「匹配条件 + 目标策略名」组成；规则列表按顺序求值，首个命中者生效，
//  最后的 MATCH 规则作为兜底。ADAPTER 名在 Tunnel 中解析为具体适配器。
//
//  关于 GEOIP / GEOSITE：二者需要 geoip.metadb / geosite.dat 数据文件。
//  数据加载属于独立的资源层工作，这里通过 GeoLookup 抽象接口预留挂点，
//  未装载数据库时相关规则永不命中（而不是抛错），保证无数据时引擎仍可用。
// ---------------------------------------------------------------------------

#include "component/cidr/IpCidrSet.h"
#include "component/trie/DomainTrie.h"
#include "constant/Constants.h"

#include <QHostAddress>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <memory>
#include <vector>

namespace core {

// ---------------------------------------------------------------------------
//  连接元数据
// ---------------------------------------------------------------------------

struct Metadata {
    // 传输层
    QString network; // "tcp" / "udp"

    QString srcIp;
    QString dstIp;
    int     srcPort = 0;
    int     dstPort = 0;

    // 目标域名（CONNECT 请求 / SOCKS5 域名形式 / 嗅探结果）
    QString host;
    // 是否来自嗅探（用于区分「域名规则是否可立即判定」）
    bool hostFromSniffing = false;

    // 入站信息
    QString inboundType; // "HTTP" / "SOCKS5" / "TUN" / "MIXED" ...
    QString inboundName;
    int     inboundPort = 0;
    QString inboundUser;

    // 进程信息（Linux 通过 /proc 获取）
    QString processPath;
    QString processName;
    int     uid = -1;

    // DSCP（TUN 入站可能携带）
    int dscp = 0;

    QHostAddress srcAddress() const;
    QHostAddress dstAddress() const;

    // 判定目标是否为字面 IP（无域名），等价参照实现的 !metadata.Host 分支
    bool hasHost() const { return !host.isEmpty(); }
};

// ---------------------------------------------------------------------------
//  GEO 数据查询接口（由资源层实现并注入）
// ---------------------------------------------------------------------------

class GeoLookup {
public:
    virtual ~GeoLookup() = default;

    // 查询 IP 所属国家/地区代码（大写，如 "CN"）；未命中返回空
    virtual QString geoIpCountry(const QHostAddress &address) const = 0;
    // 查询域名是否属于指定 geosite 分类（如 "cn" / "geolocation-!cn"）
    virtual bool matchesGeoSite(const QString &code, const QString &domain) const = 0;
    // ASN 查询
    virtual QString ipAsn(const QHostAddress &address) const = 0;
    // 数据库是否已装载
    virtual bool isReady() const = 0;
};

// ---------------------------------------------------------------------------
//  规则基类
// ---------------------------------------------------------------------------

class Rule {
public:
    Rule(RuleType type, QString payload, QString target)
        : m_type(type), m_payload(std::move(payload)), m_target(std::move(target)) {}

    virtual ~Rule() = default;

    RuleType      type() const { return m_type; }
    QString       payload() const { return m_payload; }
    // 命中的出站策略名（节点名 / 组名 / DIRECT / REJECT）
    QString       target() const { return m_target; }
    void          setTarget(const QString &target) { m_target = target; }

    // 规则文本中的额外参数（no-resolve / src / dst 等）
    QStringList params() const { return m_params; }
    void        setParams(const QStringList &params) { m_params = params; }

    // 运行期开关（对应 /rules/disable 接口）
    bool disabled() const { return m_disabled; }
    void setDisabled(bool disabled) { m_disabled = disabled; }

    // 是否需要先做 DNS 解析才能判定；返回 true 的规则在 host 为域名时
    // 需要 Tunnel 先解析出 dstIp 再重试匹配（对应参照实现的 shouldResolveIP）
    virtual bool requiresResolution() const { return false; }

    // 匹配；disabled 由基类统一短路
    bool matches(const Metadata &metadata, const GeoLookup *geo) const
    {
        if (m_disabled) return false;
        return evaluate(metadata, geo);
    }

    // 用于 /rules 接口输出的规则描述
    virtual QString description() const;

    // 规则文本中是否声明了 no-resolve
    bool noResolve() const { return m_params.contains(QStringLiteral("no-resolve"), Qt::CaseInsensitive); }

protected:
    virtual bool evaluate(const Metadata &metadata, const GeoLookup *geo) const = 0;

    RuleType    m_type;
    QString     m_payload;
    QString     m_target;
    QStringList m_params;
    bool        m_disabled = false;
};

using RulePtr = std::shared_ptr<Rule>;

// ---------------------------------------------------------------------------
//  具体规则实现
// ---------------------------------------------------------------------------

// DOMAIN：精确匹配，大小写不敏感
class DomainRule : public Rule {
public:
    DomainRule(QString payload, QString target) : Rule(RuleType::Domain, std::move(payload), std::move(target))
    {
        m_needle = m_payload.trimmed().toLower();
    }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        if (metadata.host.isEmpty()) return false;
        return metadata.host.trimmed().toLower() == m_needle;
    }

private:
    QString m_needle;
};

// DOMAIN-SUFFIX：后缀匹配，含点边界判断（example.com 匹配 a.example.com，
// 但不匹配 notexample.com）
class DomainSuffixRule : public Rule {
public:
    DomainSuffixRule(QString payload, QString target)
        : Rule(RuleType::DomainSuffix, std::move(payload), std::move(target))
    {
        m_suffix = m_payload.trimmed().toLower();
        while (m_suffix.startsWith(QLatin1Char('.'))) m_suffix.remove(0, 1);
    }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override;

private:
    QString m_suffix;
};

// DOMAIN-KEYWORD：子串匹配
class DomainKeywordRule : public Rule {
public:
    DomainKeywordRule(QString payload, QString target)
        : Rule(RuleType::DomainKeyword, std::move(payload), std::move(target))
    {
        m_keyword = m_payload.trimmed().toLower();
    }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        if (metadata.host.isEmpty()) return false;
        return metadata.host.toLower().contains(m_keyword);
    }

private:
    QString m_keyword;
};

// DOMAIN-REGEX：正则匹配
class DomainRegexRule : public Rule {
public:
    DomainRegexRule(QString payload, QString target)
        : Rule(RuleType::DomainRegex, std::move(payload), std::move(target))
    {
        m_regex.setPattern(m_payload);
        m_regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        m_valid = m_regex.isValid();
    }

    bool isValid() const { return m_valid; }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        if (!m_valid || metadata.host.isEmpty()) return false;
        return m_regex.match(metadata.host).hasMatch();
    }

private:
    QRegularExpression m_regex;
    bool               m_valid = false;
};

// GEOSITE：依赖 geosite 数据
class GeoSiteRule : public Rule {
public:
    GeoSiteRule(QString payload, QString target)
        : Rule(RuleType::GeoSite, std::move(payload), std::move(target))
    {
        m_code = m_payload.trimmed().toLower();
    }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *geo) const override
    {
        if (!geo || !geo->isReady() || metadata.host.isEmpty()) return false;
        return geo->matchesGeoSite(m_code, metadata.host);
    }

private:
    QString m_code;
};

// IP-CIDR / SRC-IP-CIDR / IP-CIDR6 / SRC-IP-CIDR6
class IpCidrRule : public Rule {
public:
    IpCidrRule(RuleType type, QString payload, QString target, bool sourceSide)
        : Rule(type, std::move(payload), std::move(target)), m_sourceSide(sourceSide)
    {
        // 解析失败时以「无效规则」形式保留，由 RuleParser 记录告警后丢弃
        m_net = IpNet::parse(m_payload);
    }

    bool isValid() const { return m_net.has_value(); }
    bool requiresResolution() const override { return !m_sourceSide && !noResolve(); }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        if (!m_net) return false;
        const QHostAddress address = m_sourceSide ? metadata.srcAddress() : metadata.dstAddress();
        if (address.isNull()) return false;
        return m_net->contains(address);
    }

private:
    std::optional<IpNet> m_net;
    bool                 m_sourceSide;
};

// GEOIP / SRC-GEOIP：依赖 mmdb 数据
class GeoIpRule : public Rule {
public:
    GeoIpRule(QString code, QString target, bool sourceSide)
        : Rule(sourceSide ? RuleType::SrcGeoIp : RuleType::GeoIp, std::move(code), std::move(target)),
          m_sourceSide(sourceSide)
    {
        m_code = m_payload.trimmed().toUpper();
    }

    bool requiresResolution() const override { return !m_sourceSide && !noResolve(); }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *geo) const override
    {
        if (!geo || !geo->isReady()) return false;
        const QHostAddress address = m_sourceSide ? metadata.srcAddress() : metadata.dstAddress();
        if (address.isNull()) return false;
        return geo->geoIpCountry(address) == m_code;
    }

private:
    QString m_code;
    bool    m_sourceSide;
};

// IP-ASN / SRC-IP-ASN，支持单值、列表与 "asn,asn" 形式
class IpAsnRule : public Rule {
public:
    IpAsnRule(QString payload, QString target, bool sourceSide)
        : Rule(sourceSide ? RuleType::SrcIpAsn : RuleType::IpAsn, std::move(payload),
               std::move(target)),
          m_sourceSide(sourceSide)
    {
        // 参照实现支持 "13335" 与 "13335,13336" 两种写法
        const QStringList asns = m_payload.split(QLatin1Char(','), Qt::SkipEmptyParts);
        for (const QString &asn : asns) m_asns << asn.trimmed();
    }

    bool requiresResolution() const override { return !m_sourceSide && !noResolve(); }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *geo) const override
    {
        if (!geo || !geo->isReady()) return false;
        const QHostAddress address = m_sourceSide ? metadata.srcAddress() : metadata.dstAddress();
        if (address.isNull()) return false;
        const QString asn = geo->ipAsn(address);
        return !asn.isEmpty() && m_asns.contains(asn, Qt::CaseInsensitive);
    }

private:
    QStringList m_asns;
    bool        m_sourceSide;
};

// DST-PORT / SRC-PORT / IN-PORT，支持单端口、区间 "8080-8090" 与列表
class PortRule : public Rule {
public:
    enum Target { Dst, Src, Inbound };

    PortRule(RuleType type, QString payload, QString target, Target which)
        : Rule(type, std::move(payload), std::move(target)), m_which(which)
    {
        const QStringList items = m_payload.split(QLatin1Char(','), Qt::SkipEmptyParts);
        for (const QString &item : items) {
            const QString text = item.trimmed();
            const int dash = text.indexOf(QLatin1Char('-'));
            if (dash > 0) {
                const auto from = strutil::parseInt(text.left(dash));
                const auto to   = strutil::parseInt(text.mid(dash + 1));
                if (from && to) m_ranges.emplace_back(*from, *to);
            } else {
                const auto port = strutil::parseInt(text);
                if (port) m_ranges.emplace_back(*port, *port);
            }
        }
    }

    bool isValid() const { return !m_ranges.empty(); }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        int port = 0;
        switch (m_which) {
        case Dst:     port = metadata.dstPort; break;
        case Src:     port = metadata.srcPort; break;
        case Inbound: port = metadata.inboundPort; break;
        }
        for (const auto &range : m_ranges) {
            if (port >= range.first && port <= range.second) return true;
        }
        return false;
    }

private:
    std::vector<std::pair<int, int>> m_ranges;
    Target                           m_which;
};

// NETWORK：tcp / udp
class NetworkRule : public Rule {
public:
    NetworkRule(QString payload, QString target)
        : Rule(RuleType::Network, std::move(payload), std::move(target))
    {
        m_network = m_payload.trimmed().toLower();
    }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        return metadata.network.compare(m_network, Qt::CaseInsensitive) == 0;
    }

private:
    QString m_network;
};

// PROCESS-NAME / PROCESS-PATH / PROCESS-*-REGEX
class ProcessRule : public Rule {
public:
    ProcessRule(RuleType type, QString payload, QString target)
        : Rule(type, std::move(payload), std::move(target))
    {
        if (type == RuleType::ProcessNameRegex || type == RuleType::ProcessPathRegex) {
            m_regex.setPattern(m_payload);
            m_regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            m_valid = m_regex.isValid();
        } else {
            m_valid = true;
            m_needle = m_payload.trimmed();
        }
    }

    bool isValid() const { return m_valid; }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override;

private:
    QRegularExpression m_regex;
    QString            m_needle;
    bool               m_valid = false;
};

// UID 规则，支持单值与范围
class UidRule : public Rule {
public:
    UidRule(QString payload, QString target)
        : Rule(RuleType::Uid, std::move(payload), std::move(target))
    {
        const QString text = m_payload.trimmed();
        const int dash = text.indexOf(QLatin1Char('-'));
        if (dash > 0) {
            const auto from = strutil::parseInt(text.left(dash));
            const auto to   = strutil::parseInt(text.mid(dash + 1));
            if (from && to) m_range = {*from, *to};
        } else {
            const auto uid = strutil::parseInt(text);
            if (uid) m_range = {*uid, *uid};
        }
    }

    bool isValid() const { return m_range.has_value(); }

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *) const override
    {
        if (!m_range || metadata.uid < 0) return false;
        return metadata.uid >= m_range->first && metadata.uid <= m_range->second;
    }

private:
    std::optional<std::pair<int, int>> m_range;
};

// RULE-SET：引用规则集，命中规则集内任一条（或按逻辑聚合）即为真
class RuleSet;
class RuleSetRule : public Rule {
public:
    RuleSetRule(QString setName, std::shared_ptr<RuleSet> set, QString target);

    bool requiresResolution() const override;

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *geo) const override;

private:
    std::shared_ptr<RuleSet> m_set;
};

// AND / OR / NOT 逻辑规则
class LogicRule : public Rule {
public:
    LogicRule(RuleType type, QString payload, QString target,
              std::vector<RulePtr> subRules)
        : Rule(type, std::move(payload), std::move(target)), m_subRules(std::move(subRules))
    {
    }

    const std::vector<RulePtr> &subRules() const { return m_subRules; }
    bool requiresResolution() const override;

protected:
    bool evaluate(const Metadata &metadata, const GeoLookup *geo) const override;

private:
    std::vector<RulePtr> m_subRules;
};

// MATCH：兜底规则，永远命中
class MatchRule : public Rule {
public:
    explicit MatchRule(QString target) : Rule(RuleType::Match, QString(), std::move(target)) {}

protected:
    bool evaluate(const Metadata &, const GeoLookup *) const override { return true; }
};

// ---------------------------------------------------------------------------
//  规则集（RULE-SET 的载体，对应参照实现 rules/provider/rule_set.go）
// ---------------------------------------------------------------------------

class RuleSet {
public:
    enum class Behavior { Domain, IpCidr, Classical };

    RuleSet(QString name, Behavior behavior) : m_name(std::move(name)), m_behavior(behavior) {}

    const QString &name() const { return m_name; }
    Behavior       behavior() const { return m_behavior; }

    // 按 behavior 选择存储结构：
    //   Domain    -> 域名树
    //   IpCidr    -> CIDR 集合
    //   Classical -> 完整规则列表（顺序求值）
    void addDomain(const QString &domain, const QString &payload);
    void addCidr(const QString &cidr, const QString &payload);
    void addRule(RulePtr rule);

    // payload 用于实现参照实现的「规则集内规则可覆写出站」：
    // 命中后若规则集项自身带目标策略，则优先使用它
    bool match(const Metadata &metadata, const GeoLookup *geo, QString *matchedTarget) const;

    int  count() const { return m_count; }
    bool isEmpty() const { return m_count == 0; }

private:
    QString                 m_name;
    Behavior                m_behavior;
    DomainTrie              m_domainTrie;
    IpCidrSet               m_cidrSet;
    std::vector<RulePtr>    m_rules;
    int                     m_count = 0;
};

} // namespace core
