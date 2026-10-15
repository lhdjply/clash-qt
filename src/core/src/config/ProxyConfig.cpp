#include "config/ProxyConfig.h"

#include "common/JsonUtil.h"

#include <QJsonArray>

namespace core {
namespace {

// 这些顶层字段已被 ProxyConfig 结构化持有，其余字段原样进 extra
const char *kKnownProxyKeys[] = {
    "name", "type", "server", "port", "udp", "tfo", "xudp", "uot", "mptcp", "smux",
    "interface-name", "routing-mark", "provider-name", "dialer-proxy",
};

const char *kKnownGroupKeys[] = {
    "name", "type", "proxies", "use", "url", "interval", "tolerance", "lazy",
    "hidden", "icon",
};

bool isKnownKey(const QString &key, const char *const *keys, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        if (key == QLatin1String(keys[i])) return true;
    }
    return false;
}

QJsonObject extractExtra(const QJsonObject &object, const char *const *known, size_t count)
{
    QJsonObject extra;
    for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
        if (!isKnownKey(it.key(), known, count)) extra.insert(it.key(), it.value());
    }
    return extra;
}

QStringList readStringList(const QJsonValue &value)
{
    QStringList result;
    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (const auto &item : array) {
            if (item.isString()) result << item.toString();
        }
    } else if (value.isString()) {
        result << value.toString();
    }
    return result;
}

} // namespace

std::optional<ProxyConfig> ProxyConfig::parse(const QJsonObject &object)
{
    ProxyConfig config;
    config.name    = jsonutil::stringValue(object, QStringLiteral("name"));
    config.rawType = jsonutil::stringValue(object, QStringLiteral("type"));
    config.server  = jsonutil::stringValue(object, QStringLiteral("server"));
    config.port    = jsonutil::intValue(object, QStringLiteral("port"));

    const auto adapterType = parseAdapterType(config.rawType);
    if (!adapterType) {
        // 类型未知：返回 nullopt 由上层登记为“不支持”，并原样保留配置，
        // 避免删除用户节点。这与参照实现在解析失败时报错但保留原始配置条目
        // 的处理方向一致。
        return std::nullopt;
    }
    config.type = *adapterType;

    config.udp           = jsonutil::boolValue(object, QStringLiteral("udp"), false);
    config.tfo           = jsonutil::boolValue(object, QStringLiteral("tfo"), false);
    config.xudp          = jsonutil::boolValue(object, QStringLiteral("xudp"), false);
    config.uot           = jsonutil::boolValue(object, QStringLiteral("uot"), false);
    config.mptcp         = jsonutil::boolValue(object, QStringLiteral("mptcp"), false);
    config.smux          = jsonutil::boolValue(object, QStringLiteral("smux"), false);
    config.interfaceName = jsonutil::stringValue(object, QStringLiteral("interface-name"));
    config.routingMark   = jsonutil::intValue(object, QStringLiteral("routing-mark"));
    config.providerName  = jsonutil::stringValue(object, QStringLiteral("provider-name"));
    config.dialerProxy   = jsonutil::stringValue(object, QStringLiteral("dialer-proxy"));

    config.extra = extractExtra(object, kKnownProxyKeys, std::size(kKnownProxyKeys));
    return config;
}

QJsonObject ProxyConfig::toJson() const
{
    QJsonObject object = extra;
    object.insert(QStringLiteral("name"), name);
    object.insert(QStringLiteral("type"), rawType.isEmpty() ? toString(type) : rawType);

    if (!server.isEmpty()) object.insert(QStringLiteral("server"), server);
    if (port > 0) object.insert(QStringLiteral("port"), port);
    if (udp) object.insert(QStringLiteral("udp"), true);
    if (tfo) object.insert(QStringLiteral("tfo"), true);
    if (xudp) object.insert(QStringLiteral("xudp"), true);
    if (uot) object.insert(QStringLiteral("uot"), true);
    if (mptcp) object.insert(QStringLiteral("mptcp"), true);
    if (smux) object.insert(QStringLiteral("smux"), true);
    if (!interfaceName.isEmpty()) object.insert(QStringLiteral("interface-name"), interfaceName);
    if (routingMark != 0) object.insert(QStringLiteral("routing-mark"), routingMark);
    if (!providerName.isEmpty()) object.insert(QStringLiteral("provider-name"), providerName);
    if (!dialerProxy.isEmpty()) object.insert(QStringLiteral("dialer-proxy"), dialerProxy);
    return object;
}

std::optional<ProxyGroupConfig> ProxyGroupConfig::parse(const QJsonObject &object)
{
    ProxyGroupConfig config;
    config.name    = jsonutil::stringValue(object, QStringLiteral("name"));
    config.rawType = jsonutil::stringValue(object, QStringLiteral("type"));

    const auto groupType = parseGroupType(config.rawType);
    if (!groupType) return std::nullopt;
    config.type = *groupType;

    config.proxies = readStringList(object.value(QStringLiteral("proxies")));
    config.use     = readStringList(object.value(QStringLiteral("use")));

    config.url       = jsonutil::stringValue(object, QStringLiteral("url"));
    config.interval  = jsonutil::intValue(object, QStringLiteral("interval"));
    config.tolerance = jsonutil::intValue(object, QStringLiteral("tolerance"));
    config.lazy      = jsonutil::boolValue(object, QStringLiteral("lazy"), false);
    config.hidden    = jsonutil::boolValue(object, QStringLiteral("hidden"), false);
    config.icon      = jsonutil::stringValue(object, QStringLiteral("icon"));

    config.extra = extractExtra(object, kKnownGroupKeys, std::size(kKnownGroupKeys));
    return config;
}

QJsonObject ProxyGroupConfig::toJson() const
{
    QJsonObject object = extra;
    object.insert(QStringLiteral("name"), name);
    object.insert(QStringLiteral("type"), rawType.isEmpty() ? toString(type) : rawType);
    object.insert(QStringLiteral("proxies"), QJsonArray::fromStringList(proxies));
    if (!use.isEmpty()) object.insert(QStringLiteral("use"), QJsonArray::fromStringList(use));
    if (!url.isEmpty()) object.insert(QStringLiteral("url"), url);
    if (interval > 0) object.insert(QStringLiteral("interval"), interval);
    if (tolerance > 0) object.insert(QStringLiteral("tolerance"), tolerance);
    if (lazy) object.insert(QStringLiteral("lazy"), true);
    if (hidden) object.insert(QStringLiteral("hidden"), true);
    if (!icon.isEmpty()) object.insert(QStringLiteral("icon"), icon);
    return object;
}

} // namespace core
