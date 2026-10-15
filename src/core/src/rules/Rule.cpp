#include "rules/Rule.h"

#include "common/JsonUtil.h"

#include <QFileInfo>

namespace core {

// ---------------------------------------------------------------------------
//  Metadata
// ---------------------------------------------------------------------------

QHostAddress Metadata::srcAddress() const
{
    if (srcIp.isEmpty()) return QHostAddress();
    const auto parsed = IpNet::parseIp(srcIp);
    return parsed.value_or(QHostAddress());
}

QHostAddress Metadata::dstAddress() const
{
    if (dstIp.isEmpty()) return QHostAddress();
    const auto parsed = IpNet::parseIp(dstIp);
    return parsed.value_or(QHostAddress());
}

// ---------------------------------------------------------------------------
//  Rule 基类
// ---------------------------------------------------------------------------

QString Rule::description() const
{
    const QString typeName = toString(m_type);
    if (m_payload.isEmpty()) return QStringLiteral("%1,%2").arg(typeName, m_target);
    return QStringLiteral("%1,%2,%3").arg(typeName, m_payload, m_target);
}

// ---------------------------------------------------------------------------
//  DOMAIN-SUFFIX
// ---------------------------------------------------------------------------

bool DomainSuffixRule::evaluate(const Metadata &metadata, const GeoLookup *) const
{
    if (metadata.host.isEmpty() || m_suffix.isEmpty()) return false;

    const QString host = metadata.host.trimmed().toLower();
    if (host.size() < m_suffix.size()) return false;
    if (!host.endsWith(m_suffix)) return false;

    // 点边界判断：host 比 suffix 长时，前一个字符必须是 '.'
    // 否则 "notexample.com" 会被 "example.com" 误匹配
    if (host.size() == m_suffix.size()) return true;
    return host.at(host.size() - m_suffix.size() - 1) == QLatin1Char('.');
}

// ---------------------------------------------------------------------------
//  PROCESS-*
// ---------------------------------------------------------------------------

bool ProcessRule::evaluate(const Metadata &metadata, const GeoLookup *) const
{
    if (!m_valid) return false;

    switch (m_type) {
    case RuleType::ProcessName:
        if (metadata.processName.isEmpty()) return false;
        return metadata.processName.compare(m_needle, Qt::CaseInsensitive) == 0
               || QFileInfo(metadata.processName).fileName().compare(m_needle, Qt::CaseInsensitive) == 0;

    case RuleType::ProcessPath:
        if (metadata.processPath.isEmpty()) return false;
        return metadata.processPath.compare(m_needle, Qt::CaseInsensitive) == 0;

    case RuleType::ProcessNameRegex:
        if (metadata.processName.isEmpty()) return false;
        return m_regex.match(metadata.processName).hasMatch();

    case RuleType::ProcessPathRegex:
        if (metadata.processPath.isEmpty()) return false;
        return m_regex.match(metadata.processPath).hasMatch();

    default:
        return false;
    }
}

// ---------------------------------------------------------------------------
//  RULE-SET
// ---------------------------------------------------------------------------

RuleSetRule::RuleSetRule(QString setName, std::shared_ptr<RuleSet> set, QString target)
    : Rule(RuleType::RuleSet, std::move(setName), std::move(target)), m_set(std::move(set))
{
}

bool RuleSetRule::requiresResolution() const
{
    if (!m_set || noResolve()) return false;
    // 只有 IP 类型规则集需要提前解析域名
    return m_set->behavior() == RuleSet::Behavior::IpCidr;
}

bool RuleSetRule::evaluate(const Metadata &metadata, const GeoLookup *geo) const
{
    if (!m_set || m_set->isEmpty()) return false;
    // 命中的出站策略由本条 RULE-SET 规则自身的 target 决定；
    // 规则集条目的 payload 仅用于 classical 行为下的条目级策略覆写，
    // 由 Tunnel 在需要时单独查询，不在此处改变规则的 target。
    return m_set->match(metadata, geo, nullptr);
}

// ---------------------------------------------------------------------------
//  逻辑规则
// ---------------------------------------------------------------------------

bool LogicRule::requiresResolution() const
{
    for (const RulePtr &rule : m_subRules) {
        if (rule && rule->requiresResolution()) return true;
    }
    return false;
}

bool LogicRule::evaluate(const Metadata &metadata, const GeoLookup *geo) const
{
    switch (m_type) {
    case RuleType::And:
        // 空子规则集合按参照实现语义视为不命中，避免配置写错时全量放行
        if (m_subRules.empty()) return false;
        for (const RulePtr &rule : m_subRules) {
            if (!rule || !rule->matches(metadata, geo)) return false;
        }
        return true;

    case RuleType::Or:
        for (const RulePtr &rule : m_subRules) {
            if (rule && rule->matches(metadata, geo)) return true;
        }
        return false;

    case RuleType::Not:
        // NOT 只取第一个子规则，与参照实现一致
        if (m_subRules.empty() || !m_subRules.front()) return false;
        return !m_subRules.front()->matches(metadata, geo);

    default:
        return false;
    }
}

// ---------------------------------------------------------------------------
//  RuleSet
// ---------------------------------------------------------------------------

void RuleSet::addDomain(const QString &domain, const QString &payload)
{
    m_domainTrie.insert(domain, payload);
    ++m_count;
}

void RuleSet::addCidr(const QString &cidr, const QString &payload)
{
    if (m_cidrSet.insert(cidr, payload)) ++m_count;
}

void RuleSet::addRule(RulePtr rule)
{
    if (!rule) return;
    m_rules.push_back(std::move(rule));
    ++m_count;
}

bool RuleSet::match(const Metadata &metadata, const GeoLookup *geo, QString *matchedTarget) const
{
    switch (m_behavior) {
    case Behavior::Domain: {
        if (metadata.host.isEmpty()) return false;
        const auto payload = m_domainTrie.searchSuffix(metadata.host);
        if (!payload) return false;
        if (matchedTarget) *matchedTarget = *payload;
        return true;
    }

    case Behavior::IpCidr: {
        const QHostAddress address = metadata.dstAddress();
        if (address.isNull()) return false;
        const auto payload = m_cidrSet.lookup(address);
        if (!payload) return false;
        if (matchedTarget) *matchedTarget = *payload;
        return true;
    }

    case Behavior::Classical:
        for (const RulePtr &rule : m_rules) {
            if (rule && rule->matches(metadata, geo)) {
                if (matchedTarget) *matchedTarget = rule->target();
                return true;
            }
        }
        return false;
    }
    return false;
}

} // namespace core
