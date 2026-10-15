#pragma once

// ---------------------------------------------------------------------------
//  基础公共设施
//
//  对应 Go 侧 common/ 下的工具函数与 log/ 包。这里提供：
//    * 字符串工具（大小写、分割、前缀后缀判断）
//    * IP / CIDR 的轻量解析与表示
//    * 单例日志门面，支持等级过滤与回调（供 App 层落到日志文件 / 界面）
// ---------------------------------------------------------------------------

#include "constant/Constants.h"

#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

#include <array>
#include <functional>
#include <optional>
#include <vector>

namespace core {

// ---------------------------------------------------------------------------
//  IP / CIDR
// ---------------------------------------------------------------------------

// 支持 IPv4 与 IPv6 的网络前缀表示。
class IpNet {
public:
    IpNet() = default;

    // 解析 "192.168.1.0/24"、"10.0.0.1"（无掩码按 /32 或 /128 处理）
    static std::optional<IpNet> parse(const QString &text);
    // 解析文本形式的 IP，返回地址族；与参照实现 common/net 的 ParseIP 对应
    static std::optional<QHostAddress> parseIp(const QString &text);

    bool isValid() const { return m_valid; }
    // 地址族：4 或 6
    int family() const { return m_family; }
    QHostAddress address() const { return m_address; }
    int prefixLength() const { return m_prefix; }
    QString toString() const;

    bool contains(const QHostAddress &address) const;
    // 判断两个 CIDR 是否有包含关系（用于规则集折叠与去重）
    bool contains(const IpNet &other) const;

private:
    bool         m_valid = false;
    int          m_family = 0;
    QHostAddress m_address;
    int          m_prefix = 0;

    // 与掩码按位与后的规范化网络地址，统一以 4 字节(IPv4)/16 字节(IPv6) 存放
    std::array<unsigned char, 16> m_bytes{};
    int                           m_bytesSize = 0;
};

// ---------------------------------------------------------------------------
//  字符串工具
// ---------------------------------------------------------------------------

namespace strutil {

QStringList splitAndTrim(const QString &text, QChar separator);
QString     trimmed(const QString &text);
QString     lower(const QString &text);
bool        startsWithIgnoreCase(const QString &text, const QString &prefix);
bool        endsWithIgnoreCase(const QString &text, const QString &suffix);
bool        equalsIgnoreCase(const QString &a, const QString &b);
// 解析 yes/no/true/false/1/0/on/off，与 yaml 布尔字面量兼容
std::optional<bool> parseBool(const QString &text);
// 解析端口或 "80-443" 形式的端口区间
std::optional<int>  parseInt(const QString &text);

} // namespace strutil

// ---------------------------------------------------------------------------
//  日志
// ---------------------------------------------------------------------------

enum class LogCategory {
    Core,
    Adapter,
    Dns,
    Rule,
    Listener,
    Tun,
    Api,
    Config,
    App,
};

QString logCategoryName(LogCategory category);

class Logger {
public:
    using Sink = std::function<void(LogCategory, const QString &level,
                                    const QString &message)>;

    static Logger &instance();

    // 设置输出等级；低于该等级的日志被丢弃
    void setLevel(LogLevel level);
    LogLevel level() const { return m_level; }

    // 注册输出回调；应用层用它把内核日志同时写入文件与界面
    void setSink(Sink sink);

    void log(LogCategory category, const QString &level, const QString &message);
    void debug(LogCategory category, const QString &message);
    void info(LogCategory category, const QString &message);
    void warn(LogCategory category, const QString &message);
    void error(LogCategory category, const QString &message);

private:
    Logger() = default;

    LogLevel m_level = LogLevel::Info;
    Sink     m_sink;
};

} // namespace core
