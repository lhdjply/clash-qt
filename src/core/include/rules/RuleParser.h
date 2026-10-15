#pragma once

// ---------------------------------------------------------------------------
//  规则解析
//
//  对应参照实现 config/parser.go 中的 parseRules / parseRule 与
//  rules/common/… 的 provider 解析。
//
//  规则文本格式（与 Clash 兼容）：
//      TYPE,PAYLOAD,TARGET[,PARAM]...
//      MATCH,TARGET
//      AND,((DOMAIN,a),(NETWORK,tcp)),TARGET
//
//  逻辑规则的 payload 是一组用括号包裹的子规则表达式；子规则不携带 target，
//  统一继承外层规则的目标策略。
// ---------------------------------------------------------------------------

#include "rules/Rule.h"

#include <QJsonArray>
#include <QString>
#include <QStringList>

#include <map>
#include <memory>

namespace core {

// 规则集注册表：RULE-SET 规则通过名称引用，对应 rule-providers 配置段
class RuleSetRegistry {
public:
    void registerSet(const QString &name, std::shared_ptr<RuleSet> set);
    std::shared_ptr<RuleSet> lookup(const QString &name) const;
    const std::map<QString, std::shared_ptr<RuleSet>> &sets() const { return m_sets; }
    void clear() { m_sets.clear(); }
    bool isEmpty() const { return m_sets.empty(); }

private:
    std::map<QString, std::shared_ptr<RuleSet>> m_sets;
};

class RuleParser {
public:
    // 解析单条规则文本。失败返回 nullptr，并把原因写入 errorMessage。
    // ruleSets 用于解析 RULE-SET 引用；为空时 RULE-SET 规则会被跳过。
    static RulePtr parse(const QString &text,
                         const RuleSetRegistry *ruleSets = nullptr,
                         QString *errorMessage = nullptr);

    // 按顺序解析整个 rules 数组；无法解析的条目记入 warnings 并跳过，
    // 与参照实现忽略非法规则但继续加载的行为一致。
    static std::vector<RulePtr> parseAll(const QJsonArray &rules,
                                         const RuleSetRegistry *ruleSets = nullptr,
                                         QStringList *warnings = nullptr);

    // 解析规则集内容（domain / ipcidr / classical 三种 behavior）。
    // payload 支持字符串数组（每项一行/一条）。
    static std::shared_ptr<RuleSet> parseRuleSet(const QString &name,
                                                 RuleSet::Behavior behavior,
                                                 const QJsonArray &payload,
                                                 QStringList *warnings = nullptr);

    // 按 "domain" / "ipcidr" / "classical" 文本解析 behavior
    static std::optional<RuleSet::Behavior> parseBehavior(const QString &text);

    // 按顶层逗号切分，括号内的逗号不参与切分（逻辑规则需要）
    static QStringList splitTopLevel(const QString &text, QChar separator);

private:
    // 解析逻辑规则的子规则集合
    static std::vector<RulePtr> parseSubRules(const QString &payload,
                                              const QString &target,
                                              const RuleSetRegistry *ruleSets,
                                              QStringList *warnings);
};

} // namespace core
