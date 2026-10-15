#pragma once

// ---------------------------------------------------------------------------
//  系统代理
//
//  对应 clash-party 的 src/main/sys/sysproxy.ts（其 Linux 部分通过
//  sysproxy-rs 原生模块实现）。
//
//  Linux 平台实现策略：
//    * 桌面环境从 XDG_CURRENT_DESKTOP 判定
//    * GNOME / Cinnamon / Unity / Deepin 等 gsettings 系：写
//      org.gnome.system.proxy 的 mode / host / port / ignore-hosts
//    * KDE Plasma：写 ~/.config/kioslaverc 的 [Proxy Settings] 段
//    * 两者都不可用时返回明确的失败原因，而不是静默「启用成功」
//
//  为什么不做「静默成功」：系统代理是用户感知网络是否走代理的直接依据。
//  如果设置实际未生效却报告成功，用户会以为流量已受保护，属于安全隐患。
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>
#include <QStringList>

namespace clashqt {

class SysProxy : public QObject {
    Q_OBJECT

public:
    enum class Backend {
        None,
        GSettings,
        KdeConfig,
    };

    explicit SysProxy(QObject *parent = nullptr);

    // 当前可用的后端
    Backend backend() const { return m_backend; }
    QString backendName() const;

    // 是否由本应用设置为启用状态（读取桌面环境当前配置）
    bool isEnabled() const;

    // 启用系统代理：mode 为 auto 时使用 PAC，否则使用 host:port 手动模式
    // bypass 为忽略代理的地址列表
    bool enable(const QString &host, int port, const QStringList &bypass = {},
                QString *error = nullptr);
    // 关闭系统代理并恢复为直连
    bool disable(QString *error = nullptr);

    // 检测各后端可用性（构造时自动执行一次）
    void detect();

    // 上一次操作产生的错误信息
    QString lastError() const { return m_lastError; }

private:
    bool enableGSettings(const QString &host, int port, const QStringList &bypass,
                         QString *error);
    bool disableGSettings(QString *error);
    bool gsettingsEnabled() const;

    bool enableKde(const QString &host, int port, const QStringList &bypass, QString *error);
    bool disableKde(QString *error);
    bool kdeEnabled() const;

    // 执行命令并返回是否成功（stderr 写入 error）
    static bool run(const QString &program, const QStringList &arguments, QString *error);
    static QString runCapture(const QString &program, const QStringList &arguments);

    Backend m_backend = Backend::None;
    QString m_lastError;
};

} // namespace clashqt
