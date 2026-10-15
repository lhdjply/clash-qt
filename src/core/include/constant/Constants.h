#pragma once

// ---------------------------------------------------------------------------
//  内核常量定义（对标参照实现的 constant 包）
//
//  对应 Go 侧：
//    constant/adapter.go   AdapterType / ProxyType
//    constant/rule.go      RuleType
//    constant/dns.go       DNSMode / FilterMode
//    constant/tunnel.go    TunStack / OutboundMode
// ---------------------------------------------------------------------------

#include <QString>
#include <QStringList>

#include <optional>

namespace core {

// 出站模式（对应参照实现的 mode: rule / global / direct）
enum class OutboundMode {
    Rule,
    Global,
    Direct,
};

// 日志等级（对应 log-level）
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Silent,
};

// TUN 栈实现（对应 tun.stack）
enum class TunStack {
    Gvisor,
    System,
    Mixed,
    Mips,
};

// DNS 增强模式（对应 dns.enhanced-mode）
enum class DnsEnhancedMode {
    Normal,
    FakeIp,
    RedirHost,
    Hosts,
};

// fake-ip-filter-mode / 域名过滤模式
enum class FilterMode {
    Blacklist,
    Whitelist,
    Rule,
};

// 进程匹配模式（对应 find-process-mode）
enum class FindProcessMode {
    Off,
    Strict,
    Always,
};

// 代理组类型（对应 proxy-groups[].type）
enum class GroupType {
    Selector,
    URLTest,
    Fallback,
    LoadBalance,
    Relay,
    Unknown,
};

// 出站适配器类型（对应参照实现 adapter.AdapterType，取常用子集 + 全量枚举）
enum class AdapterType {
    Direct,
    Reject,
    RejectDrop,
    Pass,
    Compatible,
    Dns,

    Socks5,
    Http,
    Ssh,

    Shadowsocks,
    ShadowsocksR,
    Snell,
    Vmess,
    Vless,
    Trojan,
    Hysteria,
    Hysteria2,
    Tuic,
    WireGuard,
    Mieru,
    AnyTLS,
    Sudoku,
    Masque,
    TrustTunnel,
    ShadowQuic,
    ZeroTier,
    Tailscale,
    OpenVpn,
    GostRelay,
    EasyTier,
    Mptcp,

    Unknown,
};

// 规则类型（对应参照实现 rules 全量聚合类型）
enum class RuleType {
    Domain,
    DomainSuffix,
    DomainKeyword,
    DomainRegex,
    DomainWildcard,
    GeoSite,
    GeoIp,
    SrcGeoIp,
    IpCidr,
    SrcIpCidr,
    IpCidr6,
    SrcIpCidr6,
    IpSuffix,
    SrcIpSuffix,
    DstPort,
    SrcPort,
    InPort,
    InUser,
    InName,
    InType,
    Network,
    Dscp,
    ProcessName,
    ProcessPath,
    ProcessNameRegex,
    ProcessPathRegex,
    Uid,
    RuleSet,
    SubRule,
    IpAsn,
    SrcIpAsn,
    And,
    Or,
    Not,
    Match,
    Unknown,
};

// ---------------------------------------------------------------------------
//  枚举 <-> 字符串双向映射
//  对齐 Go 侧各 stringer / ParseXXX 函数；解析失败时返回 nullopt 而不是抛异常，
//  以便配置解析可以按“未知值降级”处理，与原实现的行为保持一致。
// ---------------------------------------------------------------------------

QString    toString(OutboundMode mode);
std::optional<OutboundMode> parseOutboundMode(const QString &text);

QString    toString(LogLevel level);
std::optional<LogLevel> parseLogLevel(const QString &text);
QStringList logLevelNames();

QString    toString(TunStack stack);
std::optional<TunStack> parseTunStack(const QString &text);

QString    toString(DnsEnhancedMode mode);
std::optional<DnsEnhancedMode> parseDnsEnhancedMode(const QString &text);

QString    toString(FilterMode mode);
std::optional<FilterMode> parseFilterMode(const QString &text);

QString    toString(FindProcessMode mode);
std::optional<FindProcessMode> parseFindProcessMode(const QString &text);

QString    toString(GroupType type);
std::optional<GroupType> parseGroupType(const QString &text);

QString    toString(AdapterType type);
std::optional<AdapterType> parseAdapterType(const QString &text);

QString    toString(RuleType type);
std::optional<RuleType> parseRuleType(const QString &text);
bool       ruleTypeHasPayload(RuleType type);

// ---------------------------------------------------------------------------
//  AdapterType 的能力位，对齐 Adapter 接口的部分默认实现
// ---------------------------------------------------------------------------
QStringList adapterSupportList(AdapterType type);

} // namespace core
