#pragma once

// ---------------------------------------------------------------------------
//  IP CIDR 集合（IpCidrSet）
//
//  对应参照实现 component/cidr。用于 IP-CIDR / SRC-IP-CIDR / GEOIP 规则。
//
//  实现说明：参照实现使用前缀树（radix trie）做 O(位长) 匹配。这里为 0.8 万
//  量级规则规模采用「按前缀长度分桶 + 线性比较」的方式：把同一前缀长度的
//  网络前缀归入同一桶，查询时按前缀长度从长到短检查，先命中即为最具体匹配。
//  这与前缀树的匹配结果等价，且实现更易于验证；后续如需支持十万级 CIDR
//  （大型 GEOIP 库），可替换为真正的 radix trie 而不影响调用方。
// ---------------------------------------------------------------------------

#include "base/Common.h"

#include <QHostAddress>
#include <QString>

#include <optional>
#include <vector>

namespace core {

class IpCidrSet {
public:
    // 插入一条 CIDR；返回 false 表示文本非法
    bool insert(const QString &cidr, const QString &payload = QString());
    bool insert(const IpNet &net, const QString &payload = QString());

    // 查询地址命中的负载；未命中返回 nullopt
    std::optional<QString> lookup(const QHostAddress &address) const;
    std::optional<QString> lookup(const QString &address) const;

    bool isEmpty() const { return m_size == 0; }
    int  size() const { return m_size; }
    void clear();

private:
    struct Entry {
        IpNet   net;
        QString payload;
    };

    std::vector<Entry> m_v4Buckets[33];  // 下标 = IPv4 前缀长度 0..32
    std::vector<Entry> m_v6Buckets[129]; // 下标 = IPv6 前缀长度 0..128
    int                m_size = 0;
};

} // namespace core
