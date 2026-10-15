#include "component/cidr/IpCidrSet.h"

namespace core {

bool IpCidrSet::insert(const QString &cidr, const QString &payload)
{
    const auto net = IpNet::parse(cidr);
    if (!net) {
        Logger::instance().warn(
            LogCategory::Rule,
            QStringLiteral("忽略非法的 CIDR 规则项：%1").arg(cidr));
        return false;
    }
    return insert(*net, payload);
}

bool IpCidrSet::insert(const IpNet &net, const QString &payload)
{
    if (!net.isValid()) return false;

    Entry entry{net, payload};
    if (net.family() == 4) {
        m_v4Buckets[net.prefixLength()].push_back(std::move(entry));
    } else {
        m_v6Buckets[net.prefixLength()].push_back(std::move(entry));
    }
    ++m_size;
    return true;
}

std::optional<QString> IpCidrSet::lookup(const QHostAddress &address) const
{
    if (address.isNull()) return std::nullopt;

    const bool isV4 = address.protocol() == QAbstractSocket::IPv4Protocol;

    // 前缀越长匹配越具体，因此从最长前缀倒序扫描，命中即可返回。
    if (isV4) {
        for (int prefix = 32; prefix >= 0; --prefix) {
            for (const Entry &entry : m_v4Buckets[prefix]) {
                if (entry.net.contains(address)) return entry.payload;
            }
        }
        return std::nullopt;
    }

    for (int prefix = 128; prefix >= 0; --prefix) {
        for (const Entry &entry : m_v6Buckets[prefix]) {
            if (entry.net.contains(address)) return entry.payload;
        }
    }
    return std::nullopt;
}

std::optional<QString> IpCidrSet::lookup(const QString &address) const
{
    const auto parsed = IpNet::parseIp(address);
    if (!parsed) return std::nullopt;
    return lookup(*parsed);
}

void IpCidrSet::clear()
{
    for (auto &bucket : m_v4Buckets) bucket.clear();
    for (auto &bucket : m_v6Buckets) bucket.clear();
    m_size = 0;
}

} // namespace core
