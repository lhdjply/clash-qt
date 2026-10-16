#include "adapter/AdapterRegistry.h"

#include "adapter/Adapters.h"
#include "adapter/Hysteria2Adapter.h"
#include "adapter/VlessAdapter.h"
#include "base/Common.h"
#include "common/JsonUtil.h"

#include <QJsonArray>

namespace core {

AdapterRegistry::AdapterRegistry() { buildBuiltins(); }

AdapterRegistry::~AdapterRegistry() = default;

void AdapterRegistry::buildBuiltins()
{
    m_direct     = std::make_shared<DirectAdapter>(QStringLiteral("DIRECT"));
    m_reject     = std::make_shared<RejectAdapter>(QStringLiteral("REJECT"), false);
    m_rejectDrop = std::make_shared<RejectAdapter>(QStringLiteral("REJECT-DROP"), true);
    // PASS 在参照实现中表示「沿用到入站/上层的出站」，在应用层等价于直连，
    // 但语义上保留独立名称，避免规则文本被改写。
    m_pass = std::make_shared<DirectAdapter>(QStringLiteral("PASS"));

    // 内置策略必须同时进入名称表：代理组的成员列表经常写作
    // proxies: [REJECT, DIRECT]，成员解析走的是 find()，
    // 若只有 resolvePolicy 的特判，这类组会因为「成员不存在」而整组被跳过。
    m_adapters.insert_or_assign(m_direct->name(), m_direct);
    m_adapters.insert_or_assign(m_reject->name(), m_reject);
    m_adapters.insert_or_assign(m_rejectDrop->name(), m_rejectDrop);
    m_adapters.insert_or_assign(m_pass->name(), m_pass);
}

void AdapterRegistry::clear()
{
    m_adapters.clear();
    m_groups.clear();
    m_unsupported.clear();
    m_warnings.clear();
    buildBuiltins();
}

std::shared_ptr<Adapter> AdapterRegistry::buildNode(const ProxyConfig &config)
{
    switch (config.type) {
    case AdapterType::Direct:
        return std::make_shared<DirectAdapter>(config.name);
    case AdapterType::Reject:
        return std::make_shared<RejectAdapter>(config.name, false);
    case AdapterType::RejectDrop:
        return std::make_shared<RejectAdapter>(config.name, true);
    case AdapterType::Http:
        return std::make_shared<HttpProxyAdapter>(config);
    case AdapterType::Socks5:
        return std::make_shared<Socks5ProxyAdapter>(config);
    case AdapterType::Vless:
        return std::make_shared<VlessAdapter>(config);
    case AdapterType::Hysteria2:
        return std::make_shared<Hysteria2Adapter>(config);
    default:
        // 其余协议尚未实现：构造占位适配器并登记，界面据此提示「暂不支持」。
        // 关键点是绝不能退化为直连——那会让用户流量在无感知的情况下绕过代理。
        m_unsupported.append(config.name);
        return std::make_shared<UnsupportedAdapter>(
            config, QStringLiteral("该协议尚未实现"));
    }
}

void AdapterRegistry::loadFromConfig(const RawConfig &config)
{
    clear();

    // ---- 第一阶段：出站节点 -------------------------------------------------
    const QJsonArray proxies = config.proxies();
    for (const QJsonValue &value : proxies) {
        if (!value.isObject()) {
            m_warnings << QStringLiteral("proxies 中存在非映射条目，已跳过");
            continue;
        }
        const QJsonObject object = value.toObject();
        const auto parsed = ProxyConfig::parse(object);
        if (!parsed) {
            const QString name = jsonutil::stringValue(object, QStringLiteral("name"));
            const QString type = jsonutil::stringValue(object, QStringLiteral("type"));
            m_unsupported.append(name.isEmpty() ? type : name);
            m_warnings << QStringLiteral("无法识别的节点：%1（type=%2）")
                              .arg(name.isEmpty() ? QStringLiteral("<未命名>") : name, type);
            continue;
        }
        if (m_adapters.count(parsed->name) > 0) {
            m_warnings << QStringLiteral("节点名重复，后出现的定义被忽略：%1").arg(parsed->name);
            continue;
        }
        m_adapters.insert_or_assign(parsed->name, buildNode(*parsed));
    }

    // ---- 第二阶段：代理组 ---------------------------------------------------
    // 先收集全部组定义，再按依赖顺序构建（组可以引用其他组）。
    const QJsonArray groupArray = config.proxyGroups();
    std::vector<ProxyGroupConfig>     pendingGroups;
    std::map<QString, QJsonObject>    groupObjects;

    for (const QJsonValue &value : groupArray) {
        if (!value.isObject()) continue;
        const QJsonObject object = value.toObject();
        const auto parsed = ProxyGroupConfig::parse(object);
        if (!parsed) {
            const QString message =
                QStringLiteral("无法识别的代理组：%1（type=%2）")
                    .arg(jsonutil::stringValue(object, QStringLiteral("name")),
                         jsonutil::stringValue(object, QStringLiteral("type")));
            m_warnings.append(message);
            Logger::instance().warn(LogCategory::Adapter, message);
            continue;
        }
        pendingGroups.push_back(*parsed);
        groupObjects.insert_or_assign(parsed->name, object);
    }

    // 迭代构建：每轮把「所有成员都已就绪」的组建出来，直到不再有进展。
    // 这样天然支持任意深度的组嵌套，也能检测出循环引用。
    bool progressed = true;
    while (!pendingGroups.empty() && progressed) {
        progressed = false;

        for (auto it = pendingGroups.begin(); it != pendingGroups.end();) {
            std::vector<AdapterPtr> members;
            bool                    ready = true;

            for (const QString &memberName : it->proxies) {
                const AdapterPtr member = find(memberName);
                if (!member) {
                    // 成员可能属于尚未构建的组
                    ready = false;
                    break;
                }
                members.push_back(member);
            }

            if (!ready) {
                ++it;
                continue;
            }

            // proxy-provider 引用的节点由资源层在注册前展开并写入 proxies 列表，
            // 到这里时 `use` 只用于界面展示 provider 归属，不参与成员解析。
            if (members.empty() && it->proxies.isEmpty() && it->use.isEmpty()) {
                m_warnings << QStringLiteral("代理组 %1 没有任何成员").arg(it->name);
            }

            auto group = std::make_shared<ProxyGroup>(*it, std::move(members));
            m_adapters.insert_or_assign(it->name, group);
            m_groups.push_back(group);

            it = pendingGroups.erase(it);
            progressed = true;
        }
    }

    for (const ProxyGroupConfig &leftover : pendingGroups) {
        const QString message =
            QStringLiteral("代理组 %1 的成员无法解析（引用了不存在的节点，"
                           "或存在循环引用），已跳过；成员：%2")
                .arg(leftover.name, leftover.proxies.join(QStringLiteral(", ")));
        m_warnings.append(message);
        Logger::instance().warn(LogCategory::Adapter, message);
    }

    Logger::instance().info(
        LogCategory::Adapter,
        QStringLiteral("适配器加载完成：节点/组共 %1 个，未实现协议 %2 个，告警 %3 条")
            .arg(m_adapters.size())
            .arg(m_unsupported.size())
            .arg(m_warnings.size()));
}

AdapterPtr AdapterRegistry::find(const QString &name) const
{
    const auto it = m_adapters.find(name);
    if (it != m_adapters.end()) return it->second;
    return nullptr;
}

AdapterPtr AdapterRegistry::resolvePolicy(const QString &name) const
{
    const QString needle = name.trimmed();
    if (needle.isEmpty()) return nullptr;

    if (needle.compare(QLatin1String("DIRECT"), Qt::CaseInsensitive) == 0) return m_direct;
    if (needle.compare(QLatin1String("REJECT"), Qt::CaseInsensitive) == 0) return m_reject;
    if (needle.compare(QLatin1String("REJECT-DROP"), Qt::CaseInsensitive) == 0) {
        return m_rejectDrop;
    }
    if (needle.compare(QLatin1String("PASS"), Qt::CaseInsensitive) == 0) return m_pass;

    // GLOBAL 组由 Tunnel 依据出站模式特殊处理，这里按普通组查找
    return find(needle);
}

QJsonObject AdapterRegistry::allProxiesStatus() const
{
    QJsonObject proxies;
    for (const auto &[name, adapter] : m_adapters) {
        if (adapter) proxies.insert(name, adapter->status());
    }
    // 补上内置出站，保证 /proxies 的返回结构与前端期望一致
    proxies.insert(QStringLiteral("DIRECT"), m_direct->status());
    proxies.insert(QStringLiteral("REJECT"), m_reject->status());

    QJsonObject result;
    result.insert(QStringLiteral("proxies"), proxies);
    return result;
}

std::vector<std::shared_ptr<ProxyGroup>> AdapterRegistry::groups() const
{
    return m_groups;
}

void AdapterRegistry::clearDelays()
{
    for (const auto &[name, adapter] : m_adapters) {
        Q_UNUSED(name);
        if (adapter) adapter->clearHistory();
    }
}

} // namespace core
