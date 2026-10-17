#pragma once

// ---------------------------------------------------------------------------
//  设置页
//  对应 clash-party 的 src/renderer/src/pages/settings.tsx 的核心子集：
//  内核信息、日志、DNS/嗅探覆写开关、端口、自启动、系统代理、主题。
//  内核固定为项目自带的 C++ 实现，不提供切换。
// ---------------------------------------------------------------------------

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;

namespace clashqt {

class AppContext;

class SettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPage(AppContext *context, QWidget *parent = nullptr);

    // 把配置值回填到控件
    void loadFromConfig();

private slots:
    void onLogLevelChanged(const QString &level);
    void onControlDnsToggled(bool enabled);
    void onControlSniffToggled(bool enabled);
    void onMaxLogDaysChanged(int days);
    void onPortChanged(int value);
    void onAutoStartToggled(bool enabled);
    void onSysProxyToggled(bool enabled);
    void onTunToggled(bool enabled);
    void onTunAutoRouteToggled(bool enabled);
    void onThemeChanged(int index);
    void onOpenDataDir();

private:
    AppContext   *m_context = nullptr;
    QComboBox    *m_logLevelCombo = nullptr;
    QSpinBox     *m_maxLogDaysSpin = nullptr;
    QCheckBox    *m_controlDns = nullptr;
    QCheckBox    *m_controlSniff = nullptr;
    QSpinBox     *m_mixedPort = nullptr;
    QSpinBox     *m_socksPort = nullptr;
    QSpinBox     *m_httpPort = nullptr;
    QCheckBox    *m_autoStart = nullptr;
    QCheckBox    *m_sysProxy = nullptr;
    QCheckBox    *m_tunEnabled = nullptr;
    QCheckBox    *m_tunAutoRoute = nullptr;
    QComboBox    *m_themeCombo = nullptr;
    QLabel       *m_dataDirLabel = nullptr;

    bool m_loading = false; // 回填配置时抑制控件信号
};

} // namespace clashqt
