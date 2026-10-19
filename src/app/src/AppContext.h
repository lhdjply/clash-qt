#pragma once

// ---------------------------------------------------------------------------
//  AppContext —— 应用侧的内核管理链路
//
//  对应 clash-party 主进程中 src/main/index.ts + src/main/lifecycle.ts 的职责：
//  把「配置持久化 → 配置生成 → 内核应用 → 系统代理/托盘联动」串成一条链路，
//  并作为界面唯一的业务入口。
//
//  与内核的关系：内核以静态库形式在同一进程内运行（clashqt_core），
//  界面通过本类调用 Hub，无需 HTTP 往返。这也是本次重写相对原实现
//  （Electron 主进程通过 unix socket 访问内核进程）在架构上的主要差异。
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>

#include <memory>

#include "hub/Hub.h"

namespace clashqt {

class AppConfig;
class AutoRun;
class ControledKernelConfig;
class Factory;
class OverrideConfig;
class ProfileConfig;
class SysProxy;
class TunController;

class AppContext : public QObject {
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext() override;

    // 加载全部配置；返回是否成功（失败时 error 带原因）
    bool initialize(QString *error = nullptr);

    AppConfig             *appConfig() const { return m_appConfig; }
    ControledKernelConfig *controledConfig() const { return m_controledConfig; }
    ProfileConfig         *profileConfig() const { return m_profileConfig; }
    OverrideConfig        *overrideConfig() const { return m_overrideConfig; }
    core::Hub           *hub() const { return m_hub; }
    SysProxy              *sysProxy() const { return m_sysProxy; }
    AutoRun               *autoRun() const { return m_autoRun; }

    // ---- 内核 -------------------------------------------------------------

    // 生成配置 → 应用到内核 → 启动监听
    bool startKernel(QString *error = nullptr);
    void stopKernel();
    bool isKernelRunning() const;

    // 仅重新生成并应用配置（不停监听），对应「热重载」
    bool reloadKernel(QString *error = nullptr);

    // ---- TUN 模式 ----
    // 开启需要 root 权限：内部会先检查辅助程序是否已 setuid 授权，
    // 未授权时弹 pkexec 密码框完成授权，再建 TUN 并把 fd 交给内核协议栈。
    bool tunEnabled() const { return m_tunRunning; }
    // 正在启用（已拉起辅助程序，等 fd）：界面据此保持开关处于「已打开」
    bool tunStarting() const { return m_tunStarting; }
    bool setTunEnabled(bool enabled, QString *error = nullptr);
    // 「接管系统路由」开关：改默认路由（断网级操作，需 TUN 先开启）
    bool setTunAutoRoute(bool enabled, QString *error = nullptr);
    bool tunAutoRoute() const;

    // ---- 系统代理 ---------------------------------------------------------

    // 启用/关闭系统代理，并同步应用配置中的开关状态
    bool setSysProxyEnabled(bool enabled, QString *error = nullptr);
    bool isSysProxyEnabled() const;

    // ---- 出站模式 ---------------------------------------------------------

    bool setOutboundMode(const QString &mode, QString *error = nullptr);
    QString outboundMode() const;

    // ---- 代理组选择 -------------------------------------------------------

    // 把当前各代理组的手动选择写入应用配置，
    // 使配置重载（换订阅/改端口等）与下次启动都能恢复用户的选择。
    void saveProxySelections();

    // ---- 订阅 -------------------------------------------------------------

    // 从 URL 添加远程订阅（同步下载，用于用户手动操作）
    bool addSubscription(const QString &url, const QString &name, QString *error = nullptr);
    // 从本地文件添加订阅
    bool addLocalProfile(const QString &filePath, const QString &name, QString *error = nullptr);
    // 用新的配置文件替换已有订阅的内容（保留 id、名称与「当前订阅」状态）。
    // 用于编辑本地订阅时重新选择文件。
    bool replaceProfileContent(const QString &id, const QString &filePath,
                               QString *error = nullptr);
    // 重新下载订阅内容
    bool updateSubscription(const QString &id, QString *error = nullptr);
    // 更新全部启用自动更新的远程订阅；返回成功数量
    int updateAllSubscriptions(QStringList *errors = nullptr);
    // 切换当前订阅并热重载内核
    bool switchProfile(const QString &id, QString *error = nullptr);

    // ---- 覆写 -------------------------------------------------------------

    // 新增 yaml 覆写（内容为 YAML 文本）
    bool addYamlOverride(const QString &name, const QString &yaml, bool global,
                         QString *error = nullptr);

    // ---- 自定义规则覆写（按订阅保存）--------------------------------------
    // 写在 <数据目录>/rules/<订阅 id>.yaml，字段与 Factory::applyRuleOverride
    // 的读取约定一致：
    //   prepend  自定义规则（插到订阅规则最前，优先生效）
    //   append   追加到规则列表末尾（注意：若订阅末尾是 MATCH，追加项不会生效）
    //   delete   按文本精确删除订阅里的同名规则
    //   disabled 被用户临时停用的自定义规则：仍保留在 prepend 里（不丢内容），
    //            但应用时跳过，重新启用只需从本列表移除
    struct RuleOverride {
        QStringList prepend;
        QStringList append;
        QStringList remove;
        QStringList disabled;
    };

    // 读取当前订阅的规则覆写（无文件时返回空）
    RuleOverride ruleOverride() const;
    // 保存并热重载；失败时 error 带原因
    bool saveRuleOverride(const RuleOverride &value, QString *error = nullptr);
    // 当前订阅名称（规则覆写按订阅存储，界面需要标明作用于哪份订阅）
    QString currentProfileName() const;

    // 便捷：内核统计信息（供状态栏）
    core::RawConfig runtimeConfig() const;

    // 供界面请求一次用户提示。
    // Qt 信号是 protected 的，类外无法 emit，因此统一走这个方法。
    void showToast(const QString &type, const QString &message) { emit toast(type, message); }

signals:
    // TUN 状态变化（异步启用完成/失败），界面据此同步开关
    void tunStateChanged(bool enabled);
    void kernelRunningChanged(bool running);
    // 需要向用户提示的信息：type 取 "info" / "warning" / "error"
    void toast(const QString &type, const QString &message);

private:
    // 生成配置并应用到内核（不启动监听），是 start/reload 的公共前半段
    bool applyGeneratedConfig(QString *error);

    AppConfig             *m_appConfig = nullptr;
    ControledKernelConfig *m_controledConfig = nullptr;
    ProfileConfig         *m_profileConfig = nullptr;
    OverrideConfig        *m_overrideConfig = nullptr;
    std::unique_ptr<Factory> m_factory;
    core::Hub           *m_hub = nullptr;
    SysProxy              *m_sysProxy = nullptr;
    AutoRun               *m_autoRun = nullptr;

    std::unique_ptr<TunController> m_tun;
    bool                           m_tunRunning  = false;
    bool                           m_tunStarting = false;
    bool                           m_tunAutoRoutePending = false; // 启用的同时是否接管路由
    bool                           m_tunSignalsConnected = false;  // ready/failed 只连一次
    void                           finishTunStart();

    // 收集当前配置里代理服务器的 IP，作为 TUN 的旁路路由（防止出站环路）
    QStringList proxyServerIps() const;
    void        stopTunInternal();
};

} // namespace clashqt
