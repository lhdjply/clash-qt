#include "rules/RuleParser.h"

#include "config/ProxyConfig.h"
#include "constant/Constants.h"

#include <QJsonValue>

namespace core {

// ---------------------------------------------------------------------------
//  RuleSetRegistry
// ---------------------------------------------------------------------------

void RuleSetRegistry::registerSet(const QString &name, std::shared_ptr<RuleSet> set)
{
    if (!set) return;
    m_sets.insert_or_assign(name, std::move(set));
}

std::shared_ptr<RuleSet> RuleSetRegistry::lookup(const QString &name) const
{
    const auto it = m_sets.find(name);
    return it == m_sets.end() ? nullptr : it->second;
}

// ---------------------------------------------------------------------------
//  RuleParser
// ---------------------------------------------------------------------------

QStringList RuleParser::splitTopLevel(const QString &text, QChar separator)
{
    QStringList parts;
    QString     current;
    int         depth = 0;

    for (const QChar ch : text) {
        if (ch == QLatin1Char('(') || ch == QLatin1Char('[')) {
            ++depth;
        } else if (ch == QLatin1Char(')') || ch == QLatin1Char(']')) {
            if (depth > 0) --depth;
        }

        if (ch == separator && depth == 0) {
            parts << current;
            current.clear();
            continue;
        }
        current.append(ch);
    }
    parts << current;
    return parts;
}

std::optional<RuleSet::Behavior> RuleParser::parseBehavior(const QString &text)
{
    const QString needle = text.trimmed().toLower();
    if (needle == QLatin1String("domain")) return RuleSet::Behavior::Domain;
    if (needle == QLatin1String("ipcidr")) return RuleSet::Behavior::IpCidr;
    if (needle == QLatin1String("classical")) return RuleSet::Behavior::Classical;
    return std::nullopt;
}

std::vector<RulePtr> RuleParser::parseSubRules(const QString &payload,
                                               const QString &target,
                                               const RuleSetRegistry *ruleSets,
                                               QStringList *warnings)
{
    std::vector<RulePtr> subRules;

    QString body = payload.trimmed();
    // 去掉最外层括号
    if (body.startsWith(QLatin1Char('(')) && body.endsWith(QLatin1Char(')'))) {
        body = body.mid(1, body.size() - 2);
    }

    const QStringList items = splitTopLevel(body, QLatin1Char(','));
    for (const QString &item : items) {
        QString expression = item.trimmed();
        if (expression.isEmpty()) continue;
        // 每个子规则自身被括号包裹
        if (expression.startsWith(QLatin1Char('(')) && expression.endsWith(QLatin1Char(')'))) {
            expression = expression.mid(1, expression.size() - 2);
        }

        // 逻辑规则的子规则不带目标策略（如 "AND,((DST-PORT,443),(NETWORK,UDP)),REJECT"
        // 中的 "DST-PORT,443"）。parse() 要求「类型,匹配值,策略」三段，
        // 因此这里补一个占位策略，解析完成后再统一改写为外层目标。
        const QStringList fields = splitTopLevel(expression, QLatin1Char(','));
        if (fields.size() < 2) {
            if (warnings) {
                *warnings << QStringLiteral("逻辑规则子项格式不正确：%1").arg(expression);
            }
            continue;
        }
        QString normalized = expression;
        if (fields.size() == 2) normalized = expression + QStringLiteral(",__subrule__");

        QString error;
        RulePtr subRule = parse(normalized, ruleSets, &error);
        if (!subRule) {
            if (warnings) {
                *warnings << QStringLiteral("逻辑规则子项解析失败：%1（%2）").arg(expression, error);
            }
            continue;
        }
        // 子规则继承外层目标策略
        subRule->setTarget(target);
        subRules.push_back(std::move(subRule));
    }
    return subRules;
}

RulePtr RuleParser::parse(const QString &text, const RuleSetRegistry *ruleSets,
                          QString *errorMessage)
{
    const QString trimmedText = text.trimmed();
    if (trimmedText.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("规则为空");
        return nullptr;
    }

    const QStringList fields = splitTopLevel(trimmedText, QLatin1Char(','));
    if (fields.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("规则为空");
        return nullptr;
    }

    const QString typeText = fields.at(0).trimmed();
    const auto    ruleType = parseRuleType(typeText);
    if (!ruleType) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("未知规则类型：%1").arg(typeText);
        }
        return nullptr;
    }

    // MATCH 只有目标策略，没有 payload
    if (*ruleType == RuleType::Match) {
        if (fields.size() < 2) {
            if (errorMessage) *errorMessage = QStringLiteral("MATCH 规则缺少目标策略");
            return nullptr;
        }
        return std::make_shared<MatchRule>(fields.at(1).trimmed());
    }

    if (fields.size() < 3) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("规则字段不足（至少需要 类型,匹配值,策略）：%1")
                                .arg(trimmedText);
        }
        return nullptr;
    }

    const QString payload = fields.at(1).trimmed();
    const QString target  = fields.at(2).trimmed();

    QStringList params;
    for (int i = 3; i < fields.size(); ++i) {
        const QString param = fields.at(i).trimmed();
        if (!param.isEmpty()) params << param;
    }

    RulePtr rule;

    switch (*ruleType) {
    case RuleType::Domain:
        rule = std::make_shared<DomainRule>(payload, target);
        break;

    case RuleType::DomainSuffix:
        rule = std::make_shared<DomainSuffixRule>(payload, target);
        break;

    case RuleType::DomainKeyword:
        rule = std::make_shared<DomainKeywordRule>(payload, target);
        break;

    case RuleType::DomainRegex: {
        auto concrete = std::make_shared<DomainRegexRule>(payload, target);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法的域名正则：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::GeoSite:
        rule = std::make_shared<GeoSiteRule>(payload, target);
        break;

    case RuleType::IpCidr:
    case RuleType::IpCidr6:
    case RuleType::IpSuffix: {
        auto concrete = std::make_shared<IpCidrRule>(*ruleType, payload, target, false);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法的 IP 段：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::SrcIpCidr:
    case RuleType::SrcIpCidr6:
    case RuleType::SrcIpSuffix: {
        auto concrete = std::make_shared<IpCidrRule>(*ruleType, payload, target, true);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法的源 IP 段：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::GeoIp:
        rule = std::make_shared<GeoIpRule>(payload, target, false);
        break;

    case RuleType::SrcGeoIp:
        rule = std::make_shared<GeoIpRule>(payload, target, true);
        break;

    case RuleType::IpAsn:
        rule = std::make_shared<IpAsnRule>(payload, target, false);
        break;

    case RuleType::SrcIpAsn:
        rule = std::make_shared<IpAsnRule>(payload, target, true);
        break;

    case RuleType::DstPort: {
        auto concrete = std::make_shared<PortRule>(*ruleType, payload, target, PortRule::Dst);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法端口：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::SrcPort: {
        auto concrete = std::make_shared<PortRule>(*ruleType, payload, target, PortRule::Src);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法源端口：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::InPort: {
        auto concrete = std::make_shared<PortRule>(*ruleType, payload, target, PortRule::Inbound);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法入站端口：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::Network:
        rule = std::make_shared<NetworkRule>(payload, target);
        break;

    case RuleType::ProcessName:
    case RuleType::ProcessPath:
    case RuleType::ProcessNameRegex:
    case RuleType::ProcessPathRegex: {
        auto concrete = std::make_shared<ProcessRule>(*ruleType, payload, target);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法的进程匹配表达式：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::Uid: {
        auto concrete = std::make_shared<UidRule>(payload, target);
        if (!concrete->isValid()) {
            if (errorMessage) *errorMessage = QStringLiteral("非法的 UID：%1").arg(payload);
            return nullptr;
        }
        rule = concrete;
        break;
    }

    case RuleType::RuleSet: {
        std::shared_ptr<RuleSet> set;
        if (ruleSets) set = ruleSets->lookup(payload);
        if (!set) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("引用了未定义的规则集：%1").arg(payload);
            }
            return nullptr;
        }
        rule = std::make_shared<RuleSetRule>(payload, std::move(set), target);
        break;
    }

    case RuleType::And:
    case RuleType::Or:
    case RuleType::Not: {
        QStringList localWarnings;
        auto        subRules = parseSubRules(payload, target, ruleSets, &localWarnings);
        if (subRules.empty()) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("逻辑规则没有可用的子规则：%1").arg(payload);
            }
            return nullptr;
        }
        rule = std::make_shared<LogicRule>(*ruleType, payload, target, std::move(subRules));
        break;
    }

    case RuleType::InUser:
    case RuleType::InName:
    case RuleType::InType:
    case RuleType::Dscp:
        // 这些类型在本阶段尚未实现独立匹配器；返回 nullptr 由上层记录为
        // “暂不支持的规则”，配置仍会原样保留，不会被破坏。
        if (errorMessage) {
            *errorMessage = QStringLiteral("暂不支持的规则类型：%1").arg(typeText);
        }
        return nullptr;

    default:
        if (errorMessage) {
            *errorMessage = QStringLiteral("暂不支持的规则类型：%1").arg(typeText);
        }
        return nullptr;
    }

    if (!rule) {
        if (errorMessage) *errorMessage = QStringLiteral("规则构建失败：%1").arg(trimmedText);
        return nullptr;
    }

    rule->setParams(params);
    return rule;
}

std::vector<RulePtr> RuleParser::parseAll(const QJsonArray &rules,
                                          const RuleSetRegistry *ruleSets,
                                          QStringList *warnings)
{
    std::vector<RulePtr> parsed;
    parsed.reserve(size_t(rules.size()));

    for (const QJsonValue &value : rules) {
        if (!value.isString()) {
            if (warnings) *warnings << QStringLiteral("规则项不是字符串，已跳过");
            continue;
        }
        QString error;
        RulePtr rule = parse(value.toString(), ruleSets, &error);
        if (!rule) {
            if (warnings) *warnings << QStringLiteral("%1：%2").arg(value.toString(), error);
            continue;
        }
        parsed.push_back(std::move(rule));
    }
    return parsed;
}

std::shared_ptr<RuleSet> RuleParser::parseRuleSet(const QString &name,
                                                  RuleSet::Behavior behavior,
                                                  const QJsonArray &payload,
                                                  QStringList *warnings)
{
    auto set = std::make_shared<RuleSet>(name, behavior);

    for (const QJsonValue &value : payload) {
        if (!value.isString()) continue;
        const QString line = value.toString().trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) continue;

        switch (behavior) {
        case RuleSet::Behavior::Domain: {
            // domain 行为下每行就是一个域名，允许 "+.example.com" 写法
            QString domain = line;
            if (domain.startsWith(QLatin1String("+."))) domain = domain.mid(2);
            set->addDomain(domain, QString());
            break;
        }

        case RuleSet::Behavior::IpCidr:
            set->addCidr(line, QString());
            break;

        case RuleSet::Behavior::Classical: {
            // classical 行为下每行是完整规则，但目标策略可省略（继承外层）
            QString expression = line;
            const QStringList fields = splitTopLevel(expression, QLatin1Char(','));
            if (fields.size() == 2) {
                // 补一个占位目标，解析后再清空
                expression = QStringLiteral("%1,__inherit__").arg(line);
            }
            QString error;
            RulePtr rule = parse(expression, nullptr, &error);
            if (!rule) {
                if (warnings) {
                    *warnings << QStringLiteral("规则集 %1 中的条目无法解析：%2").arg(name, line);
                }
                continue;
            }
            if (rule->target() == QLatin1String("__inherit__")) rule->setTarget(QString());
            set->addRule(std::move(rule));
            break;
        }
        }
    }

    return set;
}

} // namespace core
