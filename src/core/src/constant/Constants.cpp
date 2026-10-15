#include "constant/Constants.h"

#include <QHash>

namespace core {
namespace {

// 单一数据源：所有 枚举<->字符串 映射都走这两张表，避免多处硬编码导致漂移。
struct EnumName {
    int         value;
    const char *name;
};

const char *lookup(const EnumName *table, size_t size, int value)
{
    for (size_t i = 0; i < size; ++i) {
        if (table[i].value == value) return table[i].name;
    }
    return nullptr;
}

template <typename E>
std::optional<E> lookupValue(const EnumName *table, size_t size, const QString &text)
{
    const QString needle = text.trimmed().toLower();
    for (size_t i = 0; i < size; ++i) {
        if (needle == QString::fromLatin1(table[i].name).toLower()) {
            return static_cast<E>(table[i].value);
        }
    }
    return std::nullopt;
}

#define MIHOMO_TABLE(name) name, std::size(name)

constexpr EnumName kOutboundModes[] = {
    {int(OutboundMode::Rule),   "rule"},
    {int(OutboundMode::Global), "global"},
    {int(OutboundMode::Direct), "direct"},
};

constexpr EnumName kLogLevels[] = {
    {int(LogLevel::Debug),   "debug"},
    {int(LogLevel::Info),    "info"},
    {int(LogLevel::Warning), "warning"},
    {int(LogLevel::Error),   "error"},
    {int(LogLevel::Silent),  "silent"},
};

constexpr EnumName kTunStacks[] = {
    {int(TunStack::Gvisor), "gvisor"},
    {int(TunStack::System), "system"},
    {int(TunStack::Mixed),  "mixed"},
    {int(TunStack::Mips),   "mips"},
};

constexpr EnumName kDnsEnhancedModes[] = {
    {int(DnsEnhancedMode::Normal),    "normal"},
    {int(DnsEnhancedMode::FakeIp),    "fake-ip"},
    {int(DnsEnhancedMode::RedirHost), "redir-host"},
    {int(DnsEnhancedMode::Hosts),     "hosts"},
};

constexpr EnumName kFilterModes[] = {
    {int(FilterMode::Blacklist), "blacklist"},
    {int(FilterMode::Whitelist), "whitelist"},
    {int(FilterMode::Rule),      "rule"},
};

constexpr EnumName kFindProcessModes[] = {
    {int(FindProcessMode::Off),    "off"},
    {int(FindProcessMode::Strict), "strict"},
    {int(FindProcessMode::Always), "always"},
};

constexpr EnumName kGroupTypes[] = {
    {int(GroupType::Selector),    "Selector"},
    {int(GroupType::URLTest),     "URLTest"},
    {int(GroupType::Fallback),    "Fallback"},
    {int(GroupType::LoadBalance), "LoadBalance"},
    {int(GroupType::Relay),       "Relay"},
};

constexpr EnumName kAdapterTypes[] = {
    {int(AdapterType::Direct),       "Direct"},
    {int(AdapterType::Reject),       "Reject"},
    {int(AdapterType::RejectDrop),   "RejectDrop"},
    {int(AdapterType::Pass),         "Pass"},
    {int(AdapterType::Compatible),   "Compatible"},
    {int(AdapterType::Dns),          "Dns"},
    {int(AdapterType::Socks5),       "Socks5"},
    {int(AdapterType::Http),         "Http"},
    {int(AdapterType::Ssh),          "Ssh"},
    {int(AdapterType::Shadowsocks),  "Shadowsocks"},
    {int(AdapterType::ShadowsocksR), "ShadowsocksR"},
    {int(AdapterType::Snell),        "Snell"},
    {int(AdapterType::Vmess),        "Vmess"},
    {int(AdapterType::Vless),        "Vless"},
    {int(AdapterType::Trojan),       "Trojan"},
    {int(AdapterType::Hysteria),     "Hysteria"},
    {int(AdapterType::Hysteria2),    "Hysteria2"},
    {int(AdapterType::Tuic),         "Tuic"},
    {int(AdapterType::WireGuard),    "WireGuard"},
    {int(AdapterType::Mieru),        "Mieru"},
    {int(AdapterType::AnyTLS),       "AnyTLS"},
    {int(AdapterType::Sudoku),       "Sudoku"},
    {int(AdapterType::Masque),       "Masque"},
    {int(AdapterType::TrustTunnel),  "TrustTunnel"},
    {int(AdapterType::ShadowQuic),   "ShadowQuic"},
    {int(AdapterType::ZeroTier),     "ZeroTier"},
    {int(AdapterType::Tailscale),    "Tailscale"},
    {int(AdapterType::OpenVpn),      "OpenVpn"},
    {int(AdapterType::GostRelay),    "GostRelay"},
    {int(AdapterType::EasyTier),     "EasyTier"},
    {int(AdapterType::Mptcp),        "Mptcp"},
};

constexpr EnumName kRuleTypes[] = {
    {int(RuleType::Domain),           "DOMAIN"},
    {int(RuleType::DomainSuffix),     "DOMAIN-SUFFIX"},
    {int(RuleType::DomainKeyword),    "DOMAIN-KEYWORD"},
    {int(RuleType::DomainRegex),      "DOMAIN-REGEX"},
    {int(RuleType::DomainWildcard),   "DOMAIN-WILDCARD"},
    {int(RuleType::GeoSite),          "GEOSITE"},
    {int(RuleType::GeoIp),            "GEOIP"},
    {int(RuleType::SrcGeoIp),         "SRC-GEOIP"},
    {int(RuleType::IpCidr),           "IP-CIDR"},
    {int(RuleType::SrcIpCidr),        "SRC-IP-CIDR"},
    {int(RuleType::IpCidr6),          "IP-CIDR6"},
    {int(RuleType::SrcIpCidr6),       "SRC-IP-CIDR6"},
    {int(RuleType::IpSuffix),         "IP-SUFFIX"},
    {int(RuleType::SrcIpSuffix),      "SRC-IP-SUFFIX"},
    {int(RuleType::DstPort),          "DST-PORT"},
    {int(RuleType::SrcPort),          "SRC-PORT"},
    {int(RuleType::InPort),           "IN-PORT"},
    {int(RuleType::InUser),           "IN-USER"},
    {int(RuleType::InName),           "IN-NAME"},
    {int(RuleType::InType),           "IN-TYPE"},
    {int(RuleType::Network),          "NETWORK"},
    {int(RuleType::Dscp),             "DSCP"},
    {int(RuleType::ProcessName),      "PROCESS-NAME"},
    {int(RuleType::ProcessPath),      "PROCESS-PATH"},
    {int(RuleType::ProcessNameRegex), "PROCESS-NAME-REGEX"},
    {int(RuleType::ProcessPathRegex), "PROCESS-PATH-REGEX"},
    {int(RuleType::Uid),              "UID"},
    {int(RuleType::RuleSet),          "RULE-SET"},
    {int(RuleType::SubRule),          "SUB-RULE"},
    {int(RuleType::IpAsn),            "IP-ASN"},
    {int(RuleType::SrcIpAsn),         "SRC-IP-ASN"},
    {int(RuleType::And),              "AND"},
    {int(RuleType::Or),               "OR"},
    {int(RuleType::Not),              "NOT"},
    {int(RuleType::Match),            "MATCH"},
};

} // namespace

QString toString(OutboundMode mode)
{
    const char *name = lookup(MIHOMO_TABLE(kOutboundModes), int(mode));
    return name ? QString::fromLatin1(name) : QStringLiteral("rule");
}

std::optional<OutboundMode> parseOutboundMode(const QString &text)
{
    return lookupValue<OutboundMode>(MIHOMO_TABLE(kOutboundModes), text);
}

QString toString(LogLevel level)
{
    const char *name = lookup(MIHOMO_TABLE(kLogLevels), int(level));
    return name ? QString::fromLatin1(name) : QStringLiteral("info");
}

std::optional<LogLevel> parseLogLevel(const QString &text)
{
    // 兼容参照实现接受但未在 UI 暴露的别名，避免旧配置被判定为非法
    const QString needle = text.trimmed().toLower();
    if (needle == QLatin1String("warn")) return LogLevel::Warning;
    if (needle == QLatin1String("fatal")) return LogLevel::Error;
    if (needle == QLatin1String("panic")) return LogLevel::Error;
    return lookupValue<LogLevel>(MIHOMO_TABLE(kLogLevels), needle);
}

QStringList logLevelNames()
{
    QStringList names;
    names.reserve(int(std::size(kLogLevels)));
    for (const auto &entry : kLogLevels) {
        names << QString::fromLatin1(entry.name);
    }
    return names;
}

QString toString(TunStack stack)
{
    const char *name = lookup(MIHOMO_TABLE(kTunStacks), int(stack));
    return name ? QString::fromLatin1(name) : QStringLiteral("gvisor");
}

std::optional<TunStack> parseTunStack(const QString &text)
{
    return lookupValue<TunStack>(MIHOMO_TABLE(kTunStacks), text);
}

QString toString(DnsEnhancedMode mode)
{
    const char *name = lookup(MIHOMO_TABLE(kDnsEnhancedModes), int(mode));
    return name ? QString::fromLatin1(name) : QStringLiteral("normal");
}

std::optional<DnsEnhancedMode> parseDnsEnhancedMode(const QString &text)
{
    return lookupValue<DnsEnhancedMode>(MIHOMO_TABLE(kDnsEnhancedModes), text);
}

QString toString(FilterMode mode)
{
    const char *name = lookup(MIHOMO_TABLE(kFilterModes), int(mode));
    return name ? QString::fromLatin1(name) : QStringLiteral("blacklist");
}

std::optional<FilterMode> parseFilterMode(const QString &text)
{
    return lookupValue<FilterMode>(MIHOMO_TABLE(kFilterModes), text);
}

QString toString(FindProcessMode mode)
{
    const char *name = lookup(MIHOMO_TABLE(kFindProcessModes), int(mode));
    return name ? QString::fromLatin1(name) : QStringLiteral("strict");
}

std::optional<FindProcessMode> parseFindProcessMode(const QString &text)
{
    return lookupValue<FindProcessMode>(MIHOMO_TABLE(kFindProcessModes), text);
}

QString toString(GroupType type)
{
    const char *name = lookup(MIHOMO_TABLE(kGroupTypes), int(type));
    return name ? QString::fromLatin1(name) : QStringLiteral("Selector");
}

std::optional<GroupType> parseGroupType(const QString &text)
{
    // 组的 type 大小写不敏感。Clash 生态存在多种写法，必须全部覆盖：
    //   select      —— Clash / Clash.Meta 配置的事实标准写法（机场订阅最常见）
    //   selector    —— 部分面板与文档使用
    //   url-test / load-balance / fallback / relay —— 各变体
    //   smart       —— smart 组，本内核按 URLTest 语义降级处理
    const QString needle = text.trimmed().toLower();
    if (needle == QLatin1String("select") || needle == QLatin1String("selector")) {
        return GroupType::Selector;
    }
    if (needle == QLatin1String("url-test") || needle == QLatin1String("urltest")) {
        return GroupType::URLTest;
    }
    if (needle == QLatin1String("load-balance") || needle == QLatin1String("loadbalance")) {
        return GroupType::LoadBalance;
    }
    if (needle == QLatin1String("fallback")) return GroupType::Fallback;
    if (needle == QLatin1String("relay")) return GroupType::Relay;
    if (needle == QLatin1String("smart")) return GroupType::URLTest;
    return std::nullopt;
}

QString toString(AdapterType type)
{
    const char *name = lookup(MIHOMO_TABLE(kAdapterTypes), int(type));
    return name ? QString::fromLatin1(name) : QStringLiteral("Unknown");
}

std::optional<AdapterType> parseAdapterType(const QString &text)
{
    // 配置里协议名大小写不统一（"ss" / "Shadowsocks" / "vmess" / "Vmess"）
    const QString needle = text.trimmed().toLower();

    static const QHash<QString, AdapterType> aliases = {
        {QStringLiteral("ss"),          AdapterType::Shadowsocks},
        {QStringLiteral("shadowsocks"), AdapterType::Shadowsocks},
        {QStringLiteral("ssr"),         AdapterType::ShadowsocksR},
        {QStringLiteral("shadowsocksr"),AdapterType::ShadowsocksR},
        {QStringLiteral("vmess"),       AdapterType::Vmess},
        {QStringLiteral("vless"),       AdapterType::Vless},
        {QStringLiteral("trojan"),      AdapterType::Trojan},
        {QStringLiteral("hysteria"),    AdapterType::Hysteria},
        {QStringLiteral("hysteria2"),   AdapterType::Hysteria2},
        {QStringLiteral("hy2"),         AdapterType::Hysteria2},
        {QStringLiteral("tuic"),        AdapterType::Tuic},
        {QStringLiteral("wireguard"),   AdapterType::WireGuard},
        {QStringLiteral("wg"),          AdapterType::WireGuard},
        {QStringLiteral("socks5"),      AdapterType::Socks5},
        {QStringLiteral("socks"),       AdapterType::Socks5},
        {QStringLiteral("http"),        AdapterType::Http},
        {QStringLiteral("https"),       AdapterType::Http},
        {QStringLiteral("ssh"),         AdapterType::Ssh},
        {QStringLiteral("snell"),       AdapterType::Snell},
        {QStringLiteral("mieru"),       AdapterType::Mieru},
        {QStringLiteral("anytls"),      AdapterType::AnyTLS},
        {QStringLiteral("sudoku"),      AdapterType::Sudoku},
        {QStringLiteral("masque"),      AdapterType::Masque},
        {QStringLiteral("trusttunnel"), AdapterType::TrustTunnel},
        {QStringLiteral("shadowquic"),  AdapterType::ShadowQuic},
        {QStringLiteral("zerotier"),    AdapterType::ZeroTier},
        {QStringLiteral("tailscale"),   AdapterType::Tailscale},
        {QStringLiteral("openvpn"),     AdapterType::OpenVpn},
        {QStringLiteral("gost-relay"),  AdapterType::GostRelay},
        {QStringLiteral("easytier"),    AdapterType::EasyTier},
        {QStringLiteral("mptcp"),       AdapterType::Mptcp},
        {QStringLiteral("direct"),      AdapterType::Direct},
        {QStringLiteral("reject"),      AdapterType::Reject},
        {QStringLiteral("rejectdrop"),  AdapterType::RejectDrop},
        {QStringLiteral("pass"),        AdapterType::Pass},
        {QStringLiteral("compatible"),  AdapterType::Compatible},
        {QStringLiteral("dns"),         AdapterType::Dns},
    };

    const auto it = aliases.constFind(needle);
    if (it != aliases.constEnd()) return it.value();
    return std::nullopt;
}

QString toString(RuleType type)
{
    const char *name = lookup(MIHOMO_TABLE(kRuleTypes), int(type));
    return name ? QString::fromLatin1(name) : QStringLiteral("MATCH");
}

std::optional<RuleType> parseRuleType(const QString &text)
{
    return lookupValue<RuleType>(MIHOMO_TABLE(kRuleTypes), text);
}

bool ruleTypeHasPayload(RuleType type)
{
    // MATCH / AND / OR / NOT 在规则文本中没有负载字段，其余规则都至少需要一个匹配值。
    switch (type) {
    case RuleType::Match:
    case RuleType::And:
    case RuleType::Or:
    case RuleType::Not:
        return false;
    default:
        return true;
    }
}

QStringList adapterSupportList(AdapterType type)
{
    switch (type) {
    case AdapterType::Direct:
    case AdapterType::Reject:
    case AdapterType::RejectDrop:
        return {QStringLiteral("TCP"), QStringLiteral("UDP")};
    case AdapterType::Shadowsocks:
    case AdapterType::ShadowsocksR:
    case AdapterType::Vmess:
    case AdapterType::Vless:
    case AdapterType::Trojan:
    case AdapterType::Hysteria2:
    case AdapterType::Tuic:
    case AdapterType::AnyTLS:
    case AdapterType::Snell:
        return {QStringLiteral("TCP"), QStringLiteral("UDP")};
    case AdapterType::Http:
    case AdapterType::Socks5:
        return {QStringLiteral("TCP"), QStringLiteral("UDP")};
    default:
        return {QStringLiteral("TCP")};
    }
}

} // namespace core
