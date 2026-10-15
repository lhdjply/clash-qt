#include "config/RawConfig.h"

namespace core {
namespace {

// 参照实现的配置键含连字符，集中在这里定义避免拼写漂移
constexpr auto kMode              = "mode";
constexpr auto kLogLevel          = "log-level";
constexpr auto kMixedPort         = "mixed-port";
constexpr auto kSocksPort         = "socks-port";
constexpr auto kHttpPort          = "port";
constexpr auto kRedirPort         = "redir-port";
constexpr auto kTproxyPort        = "tproxy-port";
constexpr auto kAllowLan          = "allow-lan";
constexpr auto kIpv6              = "ipv6";
constexpr auto kUnifiedDelay      = "unified-delay";
constexpr auto kTcpConcurrent     = "tcp-concurrent";
constexpr auto kFindProcessMode   = "find-process-mode";
constexpr auto kBindAddress       = "bind-address";
constexpr auto kAuthentication    = "authentication";
constexpr auto kSkipAuthPrefixes  = "skip-auth-prefixes";
constexpr auto kExternalControl   = "external-controller";
constexpr auto kExternalUi        = "external-ui";
constexpr auto kExternalUiUrl     = "external-ui-url";
constexpr auto kSecret            = "secret";
constexpr auto kDns               = "dns";
constexpr auto kTun               = "tun";
constexpr auto kSniffer           = "sniffer";
constexpr auto kProfile           = "profile";
constexpr auto kHosts             = "hosts";
constexpr auto kProxies           = "proxies";
constexpr auto kProxyGroups       = "proxy-groups";
constexpr auto kRules             = "rules";
constexpr auto kRuleProviders     = "rule-providers";
constexpr auto kProxyProviders    = "proxy-providers";

int readPort(const QJsonObject &object, const char *key, int fallback)
{
    const QJsonValue value = object.value(QLatin1String(key));
    if (value.isDouble()) {
        const int port = static_cast<int>(value.toDouble());
        return port > 0 ? port : fallback;
    }
    if (value.isString()) {
        bool ok = false;
        const int port = value.toString().toInt(&ok, 10);
        if (ok && port > 0) return port;
    }
    return fallback;
}

void writePort(QJsonObject &object, const char *key, int port)
{
    // 0 表示关闭该入站端口，与参照实现语义一致
    object.insert(QLatin1String(key), port);
}

QString readString(const QJsonObject &object, const char *key, const QString &fallback = {})
{
    const QJsonValue value = object.value(QLatin1String(key));
    if (value.isString()) return value.toString();
    if (value.isDouble()) {
        const double number = value.toDouble();
        if (number == static_cast<long long>(number)) {
            return QString::number(static_cast<long long>(number));
        }
        return QString::number(number);
    }
    return fallback;
}

} // namespace

std::optional<RawConfig> RawConfig::fromYaml(const QString &text, QString *errorMessage)
{
    const auto parsed = Yaml::parse(text, errorMessage);
    if (!parsed) return std::nullopt;
    if (parsed->isNull() || parsed->isUndefined()) return RawConfig(QJsonObject());
    if (!parsed->isObject()) {
        if (errorMessage) *errorMessage = QStringLiteral("配置根节点必须是映射（mapping）");
        return std::nullopt;
    }
    return RawConfig(parsed->toObject());
}

QString RawConfig::toYaml() const
{
    return Yaml::stringify(m_object);
}

// ---- 通用字段 -------------------------------------------------------------

OutboundMode RawConfig::mode() const
{
    return parseOutboundMode(readString(m_object, kMode)).value_or(OutboundMode::Rule);
}

void RawConfig::setMode(OutboundMode value)
{
    m_object.insert(QLatin1String(kMode), toString(value));
}

LogLevel RawConfig::logLevel() const
{
    return parseLogLevel(readString(m_object, kLogLevel)).value_or(LogLevel::Info);
}

void RawConfig::setLogLevel(LogLevel value)
{
    m_object.insert(QLatin1String(kLogLevel), toString(value));
}

int  RawConfig::mixedPort() const { return readPort(m_object, kMixedPort, 0); }
void RawConfig::setMixedPort(int port) { writePort(m_object, kMixedPort, port); }
int  RawConfig::socksPort() const { return readPort(m_object, kSocksPort, 0); }
void RawConfig::setSocksPort(int port) { writePort(m_object, kSocksPort, port); }
int  RawConfig::httpPort() const { return readPort(m_object, kHttpPort, 0); }
void RawConfig::setHttpPort(int port) { writePort(m_object, kHttpPort, port); }
int  RawConfig::redirPort() const { return readPort(m_object, kRedirPort, 0); }
void RawConfig::setRedirPort(int port) { writePort(m_object, kRedirPort, port); }
int  RawConfig::tproxyPort() const { return readPort(m_object, kTproxyPort, 0); }
void RawConfig::setTproxyPort(int port) { writePort(m_object, kTproxyPort, port); }

bool RawConfig::allowLan() const
{
    return jsonutil::boolValue(m_object, QLatin1String(kAllowLan), false);
}

void RawConfig::setAllowLan(bool allow)
{
    m_object.insert(QLatin1String(kAllowLan), allow);
}

bool RawConfig::ipv6() const
{
    return jsonutil::boolValue(m_object, QLatin1String(kIpv6), true);
}

void RawConfig::setIpv6(bool enable) { m_object.insert(QLatin1String(kIpv6), enable); }

bool RawConfig::unifiedDelay() const
{
    return jsonutil::boolValue(m_object, QLatin1String(kUnifiedDelay), true);
}

void RawConfig::setUnifiedDelay(bool enable)
{
    m_object.insert(QLatin1String(kUnifiedDelay), enable);
}

bool RawConfig::tcpConcurrent() const
{
    return jsonutil::boolValue(m_object, QLatin1String(kTcpConcurrent), false);
}

void RawConfig::setTcpConcurrent(bool enable)
{
    m_object.insert(QLatin1String(kTcpConcurrent), enable);
}

FindProcessMode RawConfig::findProcessMode() const
{
    return parseFindProcessMode(readString(m_object, kFindProcessMode))
        .value_or(FindProcessMode::Strict);
}

void RawConfig::setFindProcessMode(FindProcessMode value)
{
    m_object.insert(QLatin1String(kFindProcessMode), toString(value));
}

QString RawConfig::bindAddress() const
{
    return readString(m_object, kBindAddress, QStringLiteral("*"));
}

void RawConfig::setBindAddress(const QString &address)
{
    m_object.insert(QLatin1String(kBindAddress), address);
}

QStringList RawConfig::authentication() const
{
    return jsonutil::stringArrayValue(m_object, QLatin1String(kAuthentication));
}

void RawConfig::setAuthentication(const QStringList &credentials)
{
    m_object.insert(QLatin1String(kAuthentication), QJsonArray::fromStringList(credentials));
}

QStringList RawConfig::skipAuthPrefixes() const
{
    return jsonutil::stringArrayValue(m_object, QLatin1String(kSkipAuthPrefixes));
}

void RawConfig::setSkipAuthPrefixes(const QStringList &prefixes)
{
    m_object.insert(QLatin1String(kSkipAuthPrefixes), QJsonArray::fromStringList(prefixes));
}

QString RawConfig::externalController() const
{
    return readString(m_object, kExternalControl);
}

void RawConfig::setExternalController(const QString &address)
{
    m_object.insert(QLatin1String(kExternalControl), address);
}

QString RawConfig::externalUi() const { return readString(m_object, kExternalUi); }

void RawConfig::setExternalUi(const QString &path)
{
    m_object.insert(QLatin1String(kExternalUi), path);
}

QString RawConfig::externalUiUrl() const { return readString(m_object, kExternalUiUrl); }

void RawConfig::setExternalUiUrl(const QString &url)
{
    m_object.insert(QLatin1String(kExternalUiUrl), url);
}

QString RawConfig::secret() const { return readString(m_object, kSecret); }

void RawConfig::setSecret(const QString &secret)
{
    m_object.insert(QLatin1String(kSecret), secret);
}

// ---- 子配置树 -------------------------------------------------------------

QJsonObject RawConfig::dns() const { return m_object.value(QLatin1String(kDns)).toObject(); }

void RawConfig::setDns(const QJsonObject &dns)
{
    m_object.insert(QLatin1String(kDns), dns);
}

QJsonObject RawConfig::tun() const { return m_object.value(QLatin1String(kTun)).toObject(); }

void RawConfig::setTun(const QJsonObject &tun)
{
    m_object.insert(QLatin1String(kTun), tun);
}

QJsonObject RawConfig::sniffer() const
{
    return m_object.value(QLatin1String(kSniffer)).toObject();
}

void RawConfig::setSniffer(const QJsonObject &sniffer)
{
    m_object.insert(QLatin1String(kSniffer), sniffer);
}

QJsonObject RawConfig::profile() const
{
    return m_object.value(QLatin1String(kProfile)).toObject();
}

void RawConfig::setProfile(const QJsonObject &profile)
{
    m_object.insert(QLatin1String(kProfile), profile);
}

QJsonObject RawConfig::hosts() const { return m_object.value(QLatin1String(kHosts)).toObject(); }

void RawConfig::setHosts(const QJsonObject &hosts)
{
    m_object.insert(QLatin1String(kHosts), hosts);
}

// ---- 出站与规则 -----------------------------------------------------------

QJsonArray RawConfig::proxies() const
{
    return m_object.value(QLatin1String(kProxies)).toArray();
}

void RawConfig::setProxies(const QJsonArray &proxies)
{
    m_object.insert(QLatin1String(kProxies), proxies);
}

QJsonArray RawConfig::proxyGroups() const
{
    return m_object.value(QLatin1String(kProxyGroups)).toArray();
}

void RawConfig::setProxyGroups(const QJsonArray &groups)
{
    m_object.insert(QLatin1String(kProxyGroups), groups);
}

QJsonArray RawConfig::rules() const { return m_object.value(QLatin1String(kRules)).toArray(); }

void RawConfig::setRules(const QJsonArray &rules)
{
    m_object.insert(QLatin1String(kRules), rules);
}

QJsonObject RawConfig::ruleProviders() const
{
    return m_object.value(QLatin1String(kRuleProviders)).toObject();
}

QJsonObject RawConfig::proxyProviders() const
{
    return m_object.value(QLatin1String(kProxyProviders)).toObject();
}

void RawConfig::removeKey(const QString &key)
{
    m_object.remove(key);
}

// ---- 变更 -----------------------------------------------------------------

RawConfig RawConfig::merged(const QJsonObject &patch, bool isOverride) const
{
    return RawConfig(jsonutil::deepMerge(m_object, patch, isOverride));
}

void RawConfig::applyPatch(const QJsonObject &patch, bool isOverride)
{
    m_object = jsonutil::deepMerge(m_object, patch, isOverride);
}

} // namespace core
