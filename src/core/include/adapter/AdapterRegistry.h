#pragma once

// ---------------------------------------------------------------------------
//  适配器注册表
//
//  对应参照实现 adapter/parser.go 的 ParseProxies / ParseProxyGroup 以及
//  hub 中「策略名 -> 适配器」的解析。职责：
//    * 从配置的 proxies / proxy-groups 段构建适配器集合
//    * 记录因协议未实现而无法构建的节点，供界面提示
//    * 把规则中的策略名（含 DIRECT / REJECT 内置值）解析为适配器
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"
#include "config/RawConfig.h"

#include <QJsonObject>
#include <QString>
#include <QStringList>

#include <map>
#include <memory>
#include <vector>

namespace core {

class AdapterRegistry {
public:
    AdapterRegistry();
    ~AdapterRegistry();

    // 依据配置重建全部适配器。proxy-groups 的成员解析允许引用后续定义，
    // 因此内部按两阶段构建（先节点、后组）。
    void loadFromConfig(const RawConfig &config);

    void clear();

    // 按名称查找；未注册返回 nullptr
    AdapterPtr find(const QString &name) const;

    // 解析规则目标：DIRECT / REJECT / REJECT-DROP / PASS 等内置值也在此处理
    AdapterPtr resolvePolicy(const QString &name) const;

    // /proxies 接口的数据源：名称 -> 状态对象
    QJsonObject allProxiesStatus() const;

    // 代理组列表（顺序与配置一致）
    std::vector<std::shared_ptr<ProxyGroup>> groups() const;

    const QStringList &unsupportedNodes() const { return m_unsupported; }
    const QStringList          &loadWarnings() const { return m_warnings; }

    // 全部出站节点的延迟测试结果清空
    void clearDelays();

private:
    void buildBuiltins();
    std::shared_ptr<Adapter> buildNode(const ProxyConfig &config);
    // 合成内核固定的 GLOBAL 组（配置里没有定义时）。nodeOrder 为配置中
    // 节点的书写顺序，决定 GLOBAL 的成员顺序与默认出口。
    void ensureGlobalGroup(const QStringList &nodeOrder);

    std::map<QString, AdapterPtr> m_adapters;
    std::vector<std::shared_ptr<ProxyGroup>> m_groups;
    QStringList                   m_unsupported;
    QStringList                   m_warnings;

    AdapterPtr m_direct;
    AdapterPtr m_reject;
    AdapterPtr m_rejectDrop;
    AdapterPtr m_pass;
};

} // namespace core
