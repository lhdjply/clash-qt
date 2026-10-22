#pragma once

// ---------------------------------------------------------------------------
//  RawConfig —— 参照实现主配置的类型化视图
//
//  对应参照实现的 config/ 及其 IKernelConfig。
//
//  内部以 QJsonObject 持有完整配置树，本类只提供「已知字段的类型化读写」，
//  所有未知字段在读写往返中原样保留（见 Yaml.h 的说明）。
//  字段命名与参照实现的 YAML 键保持一致（含连字符），便于逐项对照。
// ---------------------------------------------------------------------------

#include "constant/Constants.h"
#include "common/JsonUtil.h"
#include "common/Yaml.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

#include <optional>

namespace core {

class RawConfig {
public:
    RawConfig() = default;
    explicit RawConfig(QJsonObject object) : m_object(std::move(object)) {}

    // ---- 序列化 -----------------------------------------------------------
    static std::optional<RawConfig> fromYaml(const QString &text, QString *errorMessage = nullptr);
    static RawConfig               fromJson(const QJsonObject &object) { return RawConfig(object); }

    QString     toYaml() const;
    QJsonObject toJson() const { return m_object; }

    bool isEmpty() const { return m_object.isEmpty(); }

    // ---- 通用字段 ---------------------------------------------------------

    OutboundMode mode() const;
    void         setMode(OutboundMode mode);

    LogLevel logLevel() const;
    void     setLogLevel(LogLevel level);

    int  mixedPort() const;
    void setMixedPort(int port);
    int  socksPort() const;
    void setSocksPort(int port);
    int  httpPort() const;
    void setHttpPort(int port);
    int  redirPort() const;
    void setRedirPort(int port);
    int  tproxyPort() const;
    void setTproxyPort(int port);

    bool allowLan() const;
    void setAllowLan(bool allow);

    bool ipv6() const;
    void setIpv6(bool enable);

    bool unifiedDelay() const;
    void setUnifiedDelay(bool enable);
    bool tcpConcurrent() const;
    void setTcpConcurrent(bool enable);

    FindProcessMode findProcessMode() const;
    void            setFindProcessMode(FindProcessMode mode);

    QString bindAddress() const;
    void    setBindAddress(const QString &address);

    QStringList authentication() const;
    void        setAuthentication(const QStringList &credentials);

    QStringList skipAuthPrefixes() const;
    void        setSkipAuthPrefixes(const QStringList &prefixes);

    // external-controller：空字符串表示不启用外部控制器
    QString externalController() const;
    void    setExternalController(const QString &address);
    QString externalUi() const;
    void    setExternalUi(const QString &path);
    QString externalUiUrl() const;
    void    setExternalUiUrl(const QString &url);
    QString secret() const;
    void    setSecret(const QString &secret);

    // ---- 子配置树 ---------------------------------------------------------
    // 三者都以独立对象返回；不存在时返回空对象，由调用方叠加默认值
    QJsonObject dns() const;
    void        setDns(const QJsonObject &dns);
    QJsonObject tun() const;
    void        setTun(const QJsonObject &tun);
    QJsonObject sniffer() const;
    void        setSniffer(const QJsonObject &sniffer);
    QJsonObject profile() const;
    void        setProfile(const QJsonObject &profile);
    QJsonObject hosts() const;
    void        setHosts(const QJsonObject &hosts);

    // ---- 出站与规则 -------------------------------------------------------
    QJsonArray proxies() const;
    void       setProxies(const QJsonArray &proxies);
    QJsonArray proxyGroups() const;
    void       setProxyGroups(const QJsonArray &groups);
    QJsonArray rules() const;
    void       setRules(const QJsonArray &rules);
    QJsonObject ruleProviders() const;
    QJsonObject proxyProviders() const;

    // 移除键（用于「受控配置」按开关裁剪 dns / sniffer / hosts 等段落）
    void removeKey(const QString &key);

    // ---- 变更 -------------------------------------------------------------
    // 仅在 patch 上做递归合并（语义对齐 deepMerge），返回合并后的新配置
    RawConfig merged(const QJsonObject &patch, bool isOverride = false) const;
    void      applyPatch(const QJsonObject &patch, bool isOverride = false);

    const QJsonObject &object() const { return m_object; }

private:
    QJsonObject m_object;
};

} // namespace core
