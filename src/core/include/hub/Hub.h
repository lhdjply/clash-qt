#pragma once

// ---------------------------------------------------------------------------
//  Hub —— 内核门面
//
//  对应参照实现 hub/ 包。对外提供内核的完整生命周期与查询能力：
//    * applyConfig()  应用配置（构建适配器、规则、DNS、启动入站）
//    * start()/stop() 启动与停止
//    * 查询接口       版本、配置、代理、规则、连接、流量、内存
//
//  Hub 同时被两类调用方使用：
//    1. Qt 界面（进程内直调，无需序列化）
//    2. ExternalController 暴露的 REST/WebSocket 接口（兼容第三方面板）
//  因此 Hub 的方法只返回结构化数据，不做任何展示层格式化。
// ---------------------------------------------------------------------------

#include "adapter/AdapterRegistry.h"
#include "component/dns/Resolver.h"
#include "config/RawConfig.h"
#include "listener/Listener.h"
#include "tunnel/Tunnel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

#include <memory>
#include <vector>

class QTimer;

namespace core {

class ExternalController;

// 监听端口的启用状态（与配置中的 *_port 字段对应）
struct InboundPorts {
    int mixed  = 0;
    int socks  = 0;
    int http   = 0;
    int redir  = 0;
    int tproxy = 0;

    bool isEmpty() const { return mixed == 0 && socks == 0 && http == 0; }
};

class Hub : public QObject {
    Q_OBJECT

public:
    explicit Hub(QObject *parent = nullptr);
    ~Hub() override;

    // ---- 生命周期 ---------------------------------------------------------

    // 应用配置：重建全部内核组件并（重新）启动入站。
    // 返回 false 时 error 会带上失败原因，且原有监听保持关闭状态。
    bool applyConfig(const RawConfig &config, QString *error = nullptr);

    // 启动（依赖已 applyConfig 的配置）。已启动时返回 true。
    bool start(QString *error = nullptr);
    void stop();
    bool isRunning() const { return m_running; }

    // ---- 查询 -------------------------------------------------------------

    QString     version() const;
    bool        meta() const { return true; }
    RawConfig   config() const;
    OutboundMode mode() const { return m_mode; }
    QStringList unsupportedNodes() const;

    // 允许在运行期切换出站模式（等价 PATCH /configs 的 mode 字段）
    void setMode(OutboundMode mode);

    // 运行期开关 DNS 段 / 嗅探段（对应受控配置的 controlDns / controlSniff）
    void setDnsEnabled(bool enabled);
    bool dnsEnabled() const { return m_dnsEnabled; }
    void setSnifferEnabled(bool enabled);
    bool snifferEnabled() const { return m_snifferEnabled; }

    // /proxies
    QJsonObject proxiesStatus() const;
    // /proxies/{name} 切换选中节点
    bool selectProxy(const QString &group, const QString &proxy, QString *error = nullptr);
    bool unfixProxy(const QString &group, QString *error = nullptr);

    // 导出各代理组的手动选择（组名 -> 成员名），只含用户显式选过的组。
    // 供应用层持久化，使选择能跨「配置重载 / 重启」保留。
    QJsonObject selectedProxies() const;
    // 恢复各代理组的手动选择（启动/配置重载后调用）；返回实际恢复的组数。
    int restoreSelectedProxies(const QJsonObject &selections);
    // 对单个节点或组做延迟测试
    QJsonObject testDelay(const QString &name, const QString &url, int timeoutMs);
    QJsonObject testGroupDelay(const QString &group, const QString &url, int timeoutMs);

    // /rules
    QJsonObject rulesStatus() const;
    bool        setRuleDisabled(const QString &ruleText, bool disabled);

    // /connections
    QJsonObject connectionsStatus() const;
    bool        closeConnection(const QString &id);
    int         closeAllConnections();
    int         closeConnectionsByChain(const QString &chain);

    // /version 之外的运行时信息
    qint64 memoryInUse() const;
    qint64 uploadBytes() const;
    qint64 downloadBytes() const;

    // 统计信息（供界面状态栏使用）
    QJsonObject statistics() const;

    std::shared_ptr<Tunnel> tunnel() const { return m_tunnel; }

signals:
    // 内核日志（等级、正文字符串），由应用层写文件与推送到界面
    void logMessage(const QString &level, const QString &message);

    // 每秒统计：上行/下行瞬时速度（字节/秒）
    void trafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed);

    // 内存占用（字节）与系统限制，0 表示不适用
    void memoryUpdated(qint64 inUse, qint64 osLimit);

    // 连接列表变化（不携带数据，由订阅者按需拉取快照，避免高频拷贝）
    void connectionsChanged();

    // 配置被运行期修改（模式切换、规则启停等）
    void configChanged();

private slots:
    void onStatisticsTick();

private:
    void stopListeners();
    bool startListeners(QString *error);

    std::shared_ptr<Tunnel>                 m_tunnel;
    std::shared_ptr<AdapterRegistry>        m_registry;
    std::shared_ptr<Resolver>               m_resolver;
    std::vector<RulePtr>                    m_rules;

    // 所有入站监听统一以基类指针持有：mixed 端口会同时提供 HTTP 与 SOCKS5，
    // 需要与独立的 http / socks 监听放在同一组里统一启停。
    std::vector<std::shared_ptr<Listener>> m_listeners;

    RawConfig    m_config;
    OutboundMode m_mode = OutboundMode::Rule;

    bool m_running         = false;
    bool m_dnsEnabled      = true;
    bool m_snifferEnabled  = true;

    // 统计采样
    QTimer *m_statsTimer = nullptr;
    qint64  m_lastUpload   = 0;
    qint64  m_lastDownload = 0;
};

} // namespace core
