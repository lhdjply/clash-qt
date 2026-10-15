#pragma once

// ---------------------------------------------------------------------------
//  域名树（DomainTrie）
//
//  对应参照实现 component/trie/domain.go。用于 DOMAIN / DOMAIN-SUFFIX 规则与
//  GEOSITE 规则集的快速匹配：把域名按标签从右向左插入，查询时逐级回溯，
//  命中深度最大的节点即为最具体的后缀匹配。
//
//  同时保存每个节点的「完整匹配值」，使 DOMAIN（精确）与 DOMAIN-SUFFIX（后缀）
//  可以共用同一棵树：精确匹配只认深度等于域名标签数的节点。
// ---------------------------------------------------------------------------

#include <QString>
#include <QStringList>

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace core {

class DomainTrie {
public:
    // 插入一个域名及其附加负载（负载通常是出站策略名或规则集 id）。
    // domain 大小写不敏感，前导 '.' 会被忽略（".example.com" 等价于 "example.com"）。
    void insert(const QString &domain, const QString &payload);

    // 精确匹配：仅当整条域名被显式插入过才命中
    std::optional<QString> searchExact(const QString &domain) const;

    // 后缀匹配：返回最长的已插入后缀对应的负载
    std::optional<QString> searchSuffix(const QString &domain) const;

    // 是否包含任何条目（用于快速短路）
    bool isEmpty() const { return m_size == 0; }

    // 条目数量（不含根）
    int size() const { return m_size; }

    void clear();

private:
    struct Node {
        // 使用 std::map 而非 QHash：QHash 要求值类型可拷贝，这里存放的是
        // 独占所有权的 unique_ptr；std::map 支持只移动类型，且键有序便于
        // 走查输出。每层标签数量很小，log n 的查找开销可忽略。
        std::map<QString, std::unique_ptr<Node>> children;
        QString                                  payload;
        bool                                     terminal = false;
    };

    // 把域名切成标签并反转（最右标签在最前）
    static QStringList reverseLabels(const QString &domain);

    Node m_root;
    int  m_size = 0;
};

} // namespace core
