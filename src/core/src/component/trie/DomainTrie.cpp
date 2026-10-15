#include "component/trie/DomainTrie.h"

#include <algorithm>

namespace core {

QStringList DomainTrie::reverseLabels(const QString &domain)
{
    QString normalized = domain.trimmed().toLower();
    while (normalized.startsWith(QLatin1Char('.'))) {
        normalized.remove(0, 1);
    }
    while (normalized.endsWith(QLatin1Char('.'))) {
        normalized.chop(1);
    }
    if (normalized.isEmpty()) return {};

    QStringList labels = normalized.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    std::reverse(labels.begin(), labels.end());
    return labels;
}

void DomainTrie::insert(const QString &domain, const QString &payload)
{
    const QStringList labels = reverseLabels(domain);
    if (labels.isEmpty()) return;

    Node *node = &m_root;
    for (const QString &label : labels) {
        auto it = node->children.find(label);
        if (it == node->children.end()) {
            auto child = std::make_unique<Node>();
            Node *raw = child.get();
            node->children.emplace(label, std::move(child));
            node = raw;
        } else {
            node = it->second.get();
        }
    }

    if (!node->terminal) {
        node->terminal = true;
        ++m_size;
    }
    node->payload = payload;
}

std::optional<QString> DomainTrie::searchExact(const QString &domain) const
{
    const QStringList labels = reverseLabels(domain);
    if (labels.isEmpty()) return std::nullopt;

    const Node *node = &m_root;
    for (const QString &label : labels) {
        const auto it = node->children.find(label);
        if (it == node->children.end()) return std::nullopt;
        node = it->second.get();
    }
    if (!node->terminal) return std::nullopt;
    return node->payload;
}

std::optional<QString> DomainTrie::searchSuffix(const QString &domain) const
{
    const QStringList labels = reverseLabels(domain);
    if (labels.isEmpty()) return std::nullopt;

    const Node *node = &m_root;
    std::optional<QString> best;

    // 根节点也可能是终止节点（例如插入了空域名，正常不会发生）
    if (node->terminal) best = node->payload;

    for (const QString &label : labels) {
        const auto it = node->children.find(label);
        if (it == node->children.end()) break;
        node = it->second.get();
        // 越深的后缀越具体，直接覆盖上一层结果
        if (node->terminal) best = node->payload;
    }
    return best;
}

void DomainTrie::clear()
{
    m_root.children.clear();
    m_root.payload.clear();
    m_root.terminal = false;
    m_size = 0;
}

} // namespace core
