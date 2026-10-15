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

private slots:
    void onNavChanged(int index);
    void onKernelRunningChanged(bool running);
    void onOutboundModeChanged(int index);
    void onSysProxyToggled(bool enabled);
    void onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed);
    void onToast(const QString &type, const QString &message);

private:
    // 构建左侧栏（状态卡片与导航）
    QWidget *buildSider();
    // 注册全部页面，返回页面标题列表（顺序与导航项一致）
    QStringList registerPages();
    // 构建系统托盘
    void buildTray();
    // 刷新托盘图标与菜单状态
    void updateTray();
    // 依据配置应用主题
    void applyThemeFromConfig();

    AppContext     *m_context = nullptr;
    QListWidget    *m_navList = nullptr;
    QStackedWidget *m_pages = nullptr;
    // 原始页面部件。页面被 QScrollArea 包裹后才进 m_pages，
    // 因此按类型访问页面必须用本列表而不是 m_pages->widget()。
    QList<QWidget *> m_pageContents;

    QLabel     *m_statsLabel = nullptr;
    QComboBox  *m_modeCombo = nullptr;
    QCheckBox  *m_sysProxySwitch = nullptr;

    QSystemTrayIcon *m_tray = nullptr;
    QAction  *m_trayToggleSysProxy = nullptr;
};

} // namespace clashqt
