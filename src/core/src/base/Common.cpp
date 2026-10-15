#include "base/Common.h"
#include "constant/Constants.h"

#include <QDateTime>
#include <QDebug>
#include <QHostAddress>
#include <QRegularExpression>

#include <cstring>

namespace core {

// ---------------------------------------------------------------------------
//  IpNet
// ---------------------------------------------------------------------------

std::optional<QHostAddress> IpNet::parseIp(const QString &text)
{
    const QString needle = text.trimmed();
    if (needle.isEmpty()) return std::nullopt;

    QHostAddress address;
    if (!address.setAddress(needle)) return std::nullopt;

    // QHostAddress::setAddress 会把非法数字串（如 "999.1.1.1"）解析失败，
    // 但会接受部分宽松写法；这里额外排除 Null 地址族。
    if (address.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
        return std::nullopt;
    }
    return address;
}

std::optional<IpNet> IpNet::parse(const QString &text)
{
    QString needle = text.trimmed();
    if (needle.isEmpty()) return std::nullopt;

    // 去掉 IPv6 的方括号写法
    if (needle.startsWith(QLatin1Char('[')) && needle.endsWith(QLatin1Char(']'))) {
        needle = needle.mid(1, needle.size() - 2);
    }

    int              defaultPrefix = 0;
    QString          addressPart   = needle;
    std::optional<int> explicitPrefix;

    const int slash = needle.lastIndexOf(QLatin1Char('/'));
    if (slash >= 0) {
        addressPart = needle.left(slash);
        const auto prefixValue = strutil::parseInt(needle.mid(slash + 1));
        if (!prefixValue) return std::nullopt;
        explicitPrefix = *prefixValue;
    }

    const auto address = parseIp(addressPart);
    if (!address) return std::nullopt;

    IpNet net;
    net.m_address = *address;
    net.m_family  = (address->protocol() == QAbstractSocket::IPv4Protocol) ? 4 : 6;
    net.m_bytesSize = (net.m_family == 4) ? 4 : 16;
    defaultPrefix   = (net.m_family == 4) ? 32 : 128;

    net.m_prefix = explicitPrefix.value_or(defaultPrefix);
    if (net.m_prefix < 0 || net.m_prefix > defaultPrefix) return std::nullopt;

    // 统一转成字节串再按掩码归零，IPv4-mapped IPv6 也能正确处理
    Q_IPV6ADDR v6 = address->isNull() ? Q_IPV6ADDR{} : QHostAddress(*address).toIPv6Address();
    if (net.m_family == 4) {
        quint32 v4 = address->toIPv4Address();
        std::memcpy(net.m_bytes.data(), &v4, 4);
    } else {
        std::memcpy(net.m_bytes.data(), &v6, 16);
    }

    int wholeBytes = net.m_prefix / 8;
    int restBits   = net.m_prefix % 8;
    if (restBits != 0 && wholeBytes < net.m_bytesSize) {
        static const unsigned char kMask[9] = {0x00, 0x80, 0xC0, 0xE0, 0xF0,
                                               0xF8, 0xFC, 0xFE, 0xFF};
        net.m_bytes[wholeBytes] &= kMask[restBits];
        ++wholeBytes;
    }
    for (int i = wholeBytes; i < net.m_bytesSize; ++i) {
        net.m_bytes[i] = 0;
    }

    net.m_valid = true;
    return net;
}

QString IpNet::toString() const
{
    if (!m_valid) return QString();
    return QStringLiteral("%1/%2").arg(m_address.toString()).arg(m_prefix);
}

bool IpNet::contains(const QHostAddress &address) const
{
    if (!m_valid || address.isNull()) return false;

    const int family = (address.protocol() == QAbstractSocket::IPv4Protocol) ? 4 : 6;
    if (family != m_family) return false;

    std::array<unsigned char, 16> bytes{};
    if (family == 4) {
        quint32 v4 = address.toIPv4Address();
        std::memcpy(bytes.data(), &v4, 4);
    } else {
        Q_IPV6ADDR v6 = address.toIPv6Address();
        std::memcpy(bytes.data(), &v6, 16);
    }

    const int wholeBytes = m_prefix / 8;
    const int restBits   = m_prefix % 8;

    if (wholeBytes > 0 && std::memcmp(bytes.data(), m_bytes.data(), size_t(wholeBytes)) != 0) {
        return false;
    }
    if (restBits != 0 && wholeBytes < m_bytesSize) {
        static const unsigned char kMask[9] = {0x00, 0x80, 0xC0, 0xE0, 0xF0,
                                               0xF8, 0xFC, 0xFE, 0xFF};
        if ((bytes[wholeBytes] & kMask[restBits]) != m_bytes[wholeBytes]) return false;
    }
    return true;
}

bool IpNet::contains(const IpNet &other) const
{
    if (!m_valid || !other.m_valid) return false;
    if (m_family != other.m_family) return false;
    if (other.m_prefix < m_prefix) return false;

    // 前缀更长的网段只要落在本网段内即为真；直接比较归零后的字节
    return contains(other.m_address);
}

// ---------------------------------------------------------------------------
//  字符串工具
// ---------------------------------------------------------------------------

namespace strutil {

QString trimmed(const QString &text) { return text.trimmed(); }

QString lower(const QString &text) { return text.toLower(); }

QStringList splitAndTrim(const QString &text, QChar separator)
{
    QStringList result;
    const QStringList raw = text.split(separator);
    result.reserve(raw.size());
    for (const QString &part : raw) {
        const QString value = part.trimmed();
        if (!value.isEmpty()) result << value;
    }
    return result;
}

bool startsWithIgnoreCase(const QString &text, const QString &prefix)
{
    return text.size() >= prefix.size()
           && text.left(prefix.size()).compare(prefix, Qt::CaseInsensitive) == 0;
}

bool endsWithIgnoreCase(const QString &text, const QString &suffix)
{
    return text.size() >= suffix.size()
           && text.right(suffix.size()).compare(suffix, Qt::CaseInsensitive) == 0;
}

bool equalsIgnoreCase(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) == 0;
}

std::optional<bool> parseBool(const QString &text)
{
    const QString needle = text.trimmed().toLower();
    if (needle.isEmpty()) return std::nullopt;
    if (needle == QLatin1String("true") || needle == QLatin1String("yes")
        || needle == QLatin1String("on") || needle == QLatin1String("1")) {
        return true;
    }
    if (needle == QLatin1String("false") || needle == QLatin1String("no")
        || needle == QLatin1String("off") || needle == QLatin1String("0")) {
        return false;
    }
    return std::nullopt;
}

std::optional<int> parseInt(const QString &text)
{
    const QString needle = text.trimmed();
    if (needle.isEmpty()) return std::nullopt;
    bool ok = false;
    const int value = needle.toInt(&ok, 10);
    if (!ok) return std::nullopt;
    return value;
}

} // namespace strutil

// ---------------------------------------------------------------------------
//  日志
// ---------------------------------------------------------------------------

QString logCategoryName(LogCategory category)
{
    switch (category) {
    case LogCategory::Core:     return QStringLiteral("Core");
    case LogCategory::Adapter:  return QStringLiteral("Adapter");
    case LogCategory::Dns:      return QStringLiteral("Dns");
    case LogCategory::Rule:     return QStringLiteral("Rule");
    case LogCategory::Listener: return QStringLiteral("Listener");
    case LogCategory::Tun:      return QStringLiteral("Tun");
    case LogCategory::Api:      return QStringLiteral("Api");
    case LogCategory::Config:   return QStringLiteral("Config");
    case LogCategory::App:      return QStringLiteral("App");
    }
    return QStringLiteral("Core");
}

namespace {

int levelRank(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:   return 0;
    case LogLevel::Info:    return 1;
    case LogLevel::Warning: return 2;
    case LogLevel::Error:   return 3;
    case LogLevel::Silent:  return 4;
    }
    return 1;
}

// 把字符串等级映射到排序权重，"warn" 等别名已在 parseLogLevel 中归一
LogLevel levelFromName(const QString &name)
{
    return parseLogLevel(name).value_or(LogLevel::Info);
}

} // namespace

Logger &Logger::instance()
{
    // C++11 起函数内静态变量初始化是线程安全的，无需额外加锁
    static Logger logger;
    return logger;
}

void Logger::setLevel(LogLevel level) { m_level = level; }

void Logger::setSink(Sink sink) { m_sink = std::move(sink); }

void Logger::log(LogCategory category, const QString &level, const QString &message)
{
    if (levelRank(levelFromName(level)) < levelRank(m_level)) return;
    if (m_sink) m_sink(category, level, message);
}

void Logger::debug(LogCategory category, const QString &message)
{
    log(category, QStringLiteral("debug"), message);
}

void Logger::info(LogCategory category, const QString &message)
{
    log(category, QStringLiteral("info"), message);
}

void Logger::warn(LogCategory category, const QString &message)
{
    log(category, QStringLiteral("warning"), message);
}

void Logger::error(LogCategory category, const QString &message)
{
    log(category, QStringLiteral("error"), message);
}

} // namespace core
