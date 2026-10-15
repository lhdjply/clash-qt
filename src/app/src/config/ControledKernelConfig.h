#pragma once

// ---------------------------------------------------------------------------
//  受控内核配置（存放于 config.yaml 的 kernel 段）
//
//  「受控配置」是应用对内核配置中「由本应用统一管理的那部分」的覆盖层；
//  它与订阅（profile）内容合并后生成最终内核配置。这样做的目的：
//  无论订阅如何变化，端口、DNS、TUN、嗅探等由用户在本应用里设定的项目
//  始终保持一致。
//
//  存储：内核为项目自研，不再单独维护一份受控配置文件；本对象直接读写
//  应用配置 config.yaml 中的 kernel 键（旧版本的 kernel.yaml / mihomo.yaml
//  会在首次加载时自动并入并删除）。
// ---------------------------------------------------------------------------

#include <QJsonObject>
#include <QObject>
#include <QStringList>

namespace clashqt {

class AppConfig;

class ControledKernelConfig : public QObject {
    Q_OBJECT

public:
    // appConfig：受控段寄存在其 kernel 键下，不可为空
    explicit ControledKernelConfig(AppConfig *appConfig, QObject *parent = nullptr);

    // 默认受控配置（对齐 defaultControledKernelConfig）
    static QJsonObject defaults();

    bool load();
    bool save();
    void patch(const QJsonObject &patch, bool persist = true);

    const QJsonObject &object() const { return m_object; }

    // ---- 常用访问 ---------------------------------------------------------
    QString mode() const;                     // rule / global / direct
    void    setMode(const QString &mode);

    QString logLevel() const;

    int  mixedPort() const;
    int  socksPort() const;
    int  httpPort() const;
    int  redirPort() const;
    int  tproxyPort() const;
    void setMixedPort(int port);
    void setSocksPort(int port);
    void setHttpPort(int port);

    bool    ipv6() const;
    bool    allowLan() const;
    QString bindAddress() const;

    // TUN
    bool    tunEnabled() const;
    void    setTunEnabled(bool enabled);
    QString tunStack() const;
    void    setTunStack(const QString &stack);
    QString tunDevice() const;

    // DNS
    bool        dnsEnabled() const;
    void        setDnsEnabled(bool enabled);
    QJsonObject dns() const;
    void        setDns(const QJsonObject &dns);

    // 嗅探
    bool snifferEnabled() const;
    void setSnifferEnabled(bool enabled);

    // 外部控制器（WebUI 面板）
    QString externalController() const;
    void    setExternalController(const QString &address);

    QStringList authentication() const;

signals:
    void configChanged();

private:
    // 旧版本独立受控配置文件（kernel.yaml / mihomo.yaml）的导入：返回其内容
    // 并删除旧文件，无旧文件时返回空对象
    QJsonObject importLegacyFile();

    AppConfig  *m_appConfig = nullptr;
    QJsonObject m_object;
    bool        m_loaded = false;
};

} // namespace clashqt
