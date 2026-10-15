#pragma once

// ---------------------------------------------------------------------------
//  应用配置（config.yaml）
//
//  对应 clash-party 的 src/main/config/app.ts + src/shared/appConfig.ts。
//
//  数据模型：以 QJsonObject 持有完整配置，默认值来自 defaults()。
//  读入时把默认值与文件内容做深合并（deepMerge），因此旧版本配置文件中
//  缺失的新字段会自动获得默认值，而用户已设置的字段不会被覆盖——这与
//  原实现的行为一致。
// ---------------------------------------------------------------------------

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

namespace clashqt {

class AppConfig : public QObject {
    Q_OBJECT

public:
    explicit AppConfig(QObject *parent = nullptr);

    // 默认配置（对齐 clash-party 的 defaultConfig）
    static QJsonObject defaults();

    // 从磁盘加载并与默认值合并；失败时使用默认值
    bool load();
    // 写回磁盘
    bool save();

    // 深合并补丁（语义见 core 的 jsonutil::deepMerge），可选择是否立即落盘
    void patch(const QJsonObject &patch, bool persist = true);

    // 整体替换 kernel 段（受控内核配置）。深合并无法删除键，而受控段需要
    // 「替换」语义才能清除已废弃的键（例如旧版本的 geox-url 下载地址）。
    void setKernelSection(const QJsonObject &section, bool persist = true);

    bool isLoaded() const { return m_loaded; }
    const QJsonObject &object() const { return m_object; }

    // ---- 常用字段的类型化访问 --------------------------------------------
    // 说明：只对界面与内核管理真正需要的字段提供访问器；其余字段通过
    // object()/patch() 以 JSON 方式访问，避免为 130+ 个字段写样板代码。

    bool enableSmartOverride() const;

    bool silentStart() const;
    bool autoCheckUpdate() const;
    bool autoUpdateProfileOnStart() const;

    QString appTheme() const;          // system / light / dark
    QString language() const;          // zh-CN / zh-TW / en-US / ru-RU / fa-IR

    bool disableTray() const;
    bool proxyInTray() const;
    bool showCurrentProxyInTray() const;
    QString trayProxyGroupStyle() const;

    int  maxLogDays() const;
    int  maxLogFileSizeMB() const;
    bool disableAppLog() const;
    bool disableCoreLog() const;
    QString logLevel() const;

    bool controlDns() const;
    void setControlDns(bool enabled);
    bool controlSniff() const;
    void setControlSniff(bool enabled);
    bool useNameserverPolicy() const;

    // 端口展示与启用状态
    int  showMixedPort() const;
    int  showSocksPort() const;
    int  showHttpPort() const;
    bool enableMixedPort() const;
    bool enableSocksPort() const;
    bool enableHttpPort() const;

    // 系统代理
    QJsonObject sysProxy() const;
    bool        sysProxyEnabled() const;
    void        setSysProxyEnabled(bool enabled);
    QString     sysProxyMode() const;
    QStringList sysProxyBypass() const;

    // 出站模式（rule / global / direct）
    QString mode() const;
    void    setMode(const QString &mode);

    // 当前选中的侧栏卡片（用于记住上次位置）
    QString lastSelectedSiderCard() const;
    void    setLastSelectedSiderCard(const QString &key);
    bool    rememberSelectedSiderCard() const;
    int     siderWidth() const;
    void    setSiderWidth(int width);

    // 全局覆写 / 路由开关
    bool diffWorkDir() const;
    bool autoSetDNS() const;
    QString originDNS() const;

    // 各代理组的手动选择（组名 -> 成员名）。
    // 存这里而不是内核配置段：它是界面选择状态，不应下发给内核。
    QJsonObject selectedProxies() const;
    void        setSelectedProxies(const QJsonObject &selections);

    // 延迟测试
    QString delayTestUrl() const;
    int     delayTestTimeout() const;
    int     delayTestConcurrency() const;

    // 订阅请求的 User-Agent；为空时由 AppContext 使用内置默认值
    // （clash-qt/v<版本> (clash.meta)，与 clash-party 的策略一致）
    QString userAgent() const;

    // 供外部在修改完字段后通知界面刷新。
    // Qt 信号是 protected 的，类外无法直接 emit，因此提供这个入口。
    void notifyChanged() { emit configChanged(); }

signals:
    // 配置发生变更（界面据此刷新）
    void configChanged();

private:
    QJsonObject m_object;
    bool        m_loaded = false;
};

} // namespace clashqt
