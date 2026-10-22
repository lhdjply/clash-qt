#pragma once

// ---------------------------------------------------------------------------
//  主窗口
//
//  对应参照实现的 src/renderer/src/App.tsx + components/sider/。
//  布局：左侧为「状态卡片 + 导航」的侧栏，右侧为页面栈。
//
//  侧栏按瓦片组织（见 ui/TileButton.h）：出站模式分段、系统代理 / TUN 两块
//  开关瓦片、当前订阅的流量卡、导航瓦片网格，底部是流量入口与实时速率。
//  订阅页不占导航位（上方订阅卡即入口），设置页是网格里的最后一块瓦片。
// ---------------------------------------------------------------------------

#include <QLabel>
#include <QMainWindow>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QVector>

class QButtonGroup;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class QToolButton;

namespace clashqt {

class AppContext;
class ProfileItem;

namespace ui {
class TileButton;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(AppContext *context, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    // 自绘窗口背景：对角渐变 + 左上角光晕。侧栏与卡片是半透明面板，
    // 叠在这层背景上形成「磨砂玻璃透底」的观感（见 UiFactory.h 说明）。
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onKernelRunningChanged(bool running);
    void onOutboundModeChanged(int id);
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
    // 品牌行：图标 + 名称版本
    QWidget *buildBrand();
    // 出站模式分段控件
    QWidget *buildModeSegment();
    // 系统代理 / TUN 两块开关瓦片
    QWidget *buildToggleTiles();
    // 当前订阅的流量卡
    QWidget *buildProfileCard();
    // 导航瓦片网格 + 底部流量卡
    QWidget *buildNavGrid();
    // 底部「流量」卡：入口 + 上下行实时速率
    QWidget *buildTrafficCard();
    // 订阅卡 / 流量卡在对应页面时的选中态（与强调色瓦片一致）
    void updateProfileCardState();
    void updateTrafficCardState();

    // 注册全部页面（顺序即页面栈下标）
    void registerPages();
    // 切到指定页面，并同步导航瓦片的选中态
    void showPage(int page);
    // 刷新导航瓦片的选中态
    void updateTileStates();
    // 刷新导航瓦片的计数徽标（代理组数 / 规则条数 / 覆写条数 / 连接数）
    void refreshBadges();
    // 只刷新连接数徽标：连接变化每秒都会通知，不能连带去读规则覆写文件
    void refreshConnectionBadge();
    // 刷新订阅卡的名称、流量与到期日
    void refreshProfileCard();
    // 重新拉取当前订阅（订阅卡右上角刷新按钮）
    void refreshCurrentSubscription();

    // 构建系统托盘
    void buildTray();
    // 刷新托盘图标与菜单状态
    void updateTray();
    // 按 AppContext 的真实状态同步「快捷设置」与托盘里的 TUN 开关
    void syncTunUi();
    // 启动时自动检查一次更新（设置里可关）：只在真有新版本时提示
    void checkUpdatesOnStartup();
    // 托盘 tooltip 里的接管状态（系统代理 / TUN / 两者）
    QString trayStateText() const;
    // 依据配置应用主题
    void applyThemeFromConfig();

    AppContext     *m_context = nullptr;
    QStackedWidget *m_pages = nullptr;
    // 原始页面部件。页面被 QScrollArea 包裹后才进 m_pages，
    // 因此按类型访问页面必须用本列表而不是 m_pages->widget()。
    QList<QWidget *> m_pageContents;

    // 导航瓦片（下标与 m_navPages 对应）
    QVector<ui::TileButton *> m_navTiles;
    QVector<int>              m_navPages;
    int                       m_currentPage = 0;
    // 需要显示计数徽标的导航瓦片下标（代理组 / 规则 / 覆写 / 连接）
    int m_proxyTileIndex     = -1;
    int m_ruleTileIndex      = -1;
    int m_overrideTileIndex  = -1;
    int m_connectionTileIndex = -1;

    QButtonGroup *m_modeGroup    = nullptr;
    ui::TileButton *m_sysProxyTile = nullptr;
    ui::TileButton *m_tunTile      = nullptr;

    QWidget     *m_profileCard  = nullptr;
    QLabel      *m_profileName  = nullptr;
    QLabel      *m_profileUsage = nullptr;
    QLabel      *m_profileExpire = nullptr;
    QProgressBar *m_profileBar  = nullptr;
    // 订阅卡右上角的刷新按钮（重新拉取当前远程订阅）
    QToolButton *m_profileRefresh = nullptr;

    QPushButton *m_trafficCard  = nullptr;
    QLabel      *m_trafficTitle = nullptr;
    QLabel      *m_trafficIcon  = nullptr;
    QLabel      *m_uploadLabel = nullptr;
    QLabel      *m_downloadLabel = nullptr;

    QSystemTrayIcon *m_tray = nullptr;
    QAction  *m_trayToggleSysProxy = nullptr;
    QAction  *m_trayToggleTun = nullptr;
    // 托盘图标最近一次设置的状态；用于判断是否需要补发延迟重设
    int  m_trayIconState   = -1;   // AppIconState 的整数值
    bool m_trayIconApplied = false;
    // 首次显示后是否已安排过启动更新检查
    bool m_updateCheckScheduled = false;
};

} // namespace clashqt
