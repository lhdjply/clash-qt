#pragma once

// ---------------------------------------------------------------------------
//  主窗口
//
//  对应 clash-party 的 src/renderer/src/App.tsx + components/sider/。
//  布局：左侧为「状态卡片 + 导航」的侧栏，右侧为页面栈。
//
//  与原实现的差异：Electron 版的侧栏卡片可拖拽排序（dnd-kit），
//  本阶段先提供固定顺序；卡片排序与悬浮窗属于后续阶段。
// ---------------------------------------------------------------------------

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStringList>
#include <QSystemTrayIcon>

class QListWidget;
class QStackedWidget;

namespace clashqt {

class AppContext;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(AppContext *context, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    // 自绘窗口背景：对角渐变 + 左上角光晕。侧栏与卡片是半透明面板，
    // 叠在这层背景上形成「磨砂玻璃透底」的观感（见 UiFactory.h 说明）。
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onNavChanged(int index);
    void onKernelRunningChanged(bool running);
    void onOutboundModeChanged(int index);
    void onSysProxyToggled(bool enabled);
    // 快捷设置 / 托盘菜单共用的 TUN 开关入口
    void onTunToggleRequested(bool enabled);
    void onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed);
    void onToast(const QString &type, const QString &message);
    // 桌面深浅色发生变化（仅「跟随系统」时跟随）
    void onSystemColorSchemeChanged();

private:
    // 构建左侧栏（状态卡片与导航）
    QWidget *buildSider();
    // 注册全部页面，返回页面标题列表（顺序与导航项一致）
    QStringList registerPages();
    // 构建系统托盘
    void buildTray();
    // 刷新托盘图标与菜单状态
    void updateTray();
    // 按 AppContext 的真实状态同步「快捷设置」与托盘里的 TUN 开关
    void syncTunUi();
    // 托盘 tooltip 里的接管状态（系统代理 / TUN / 两者）
    QString trayStateText() const;
    // 依据配置应用主题
    void applyThemeFromConfig();
    // 刷新导航图标：选中项是实心强调色底，图标取白；其余取正文色
    void updateNavIcons();

    AppContext     *m_context = nullptr;
    QListWidget    *m_navList = nullptr;
    QStackedWidget *m_pages = nullptr;
    // 原始页面部件。页面被 QScrollArea 包裹后才进 m_pages，
    // 因此按类型访问页面必须用本列表而不是 m_pages->widget()。
    QList<QWidget *> m_pageContents;

    // 导航标题（顺序与页面一致），刷新图标时按标题取图标
    QStringList m_navTitles;

    QLabel     *m_uploadLabel = nullptr;
    QLabel     *m_downloadLabel = nullptr;
    QComboBox  *m_modeCombo = nullptr;
    QCheckBox  *m_sysProxySwitch = nullptr;
    QCheckBox  *m_tunSwitch = nullptr;

    QSystemTrayIcon *m_tray = nullptr;
    QAction  *m_trayToggleSysProxy = nullptr;
    QAction  *m_trayToggleTun = nullptr;
    // 托盘图标最近一次设置的状态；用于判断是否需要补发延迟重设
    int  m_trayIconState   = -1;   // AppIconState 的整数值
    bool m_trayIconApplied = false;
};

} // namespace clashqt
