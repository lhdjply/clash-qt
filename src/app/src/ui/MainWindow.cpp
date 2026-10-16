#include "ui/MainWindow.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "config/ProfileConfig.h"
#include "sys/SysProxy.h"
#include "ui/Icons.h"
#include "ui/UiFactory.h"
#include "ui/pages/ConnectionsPage.h"
#include "ui/pages/LogsPage.h"
#include "ui/pages/ProfilesPage.h"
#include "ui/pages/ProxiesPage.h"
#include "ui/pages/RulesPage.h"
#include "ui/pages/SettingsPage.h"
#include "ui/pages/TrafficPage.h"
#include "utils/AppLogger.h"
#include "utils/Calc.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QWidget>

#include <QLinearGradient>
#include <QListWidgetItem>
#include <QPaintEvent>
#include <QPainter>
#include <QRadialGradient>

#include "hub/Hub.h"

namespace clashqt {

MainWindow::MainWindow(AppContext *context, QWidget *parent)
    : QMainWindow(parent), m_context(context)
{
    setWindowTitle(QStringLiteral("Clash Qt"));
    resize(1180, 760);
    setMinimumSize(960, 620);
    setWindowIcon(ui::icons::appIcon(64));

    // 主题必须在创建任何界面部件之前应用：卡片、徽标、按钮的图标都是
    // 按当前主题色渲染的位图，晚于部件创建就只会得到默认浅色配色
    // （深色主题下按钮图标会变成近黑，几乎看不见）。
    applyThemeFromConfig();

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // 桌面深浅色在运行期变化时（设置里选「跟随系统」）实时跟随
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this,
                     &MainWindow::onSystemColorSchemeChanged);
#endif

    auto *central = new QWidget(this);
    // 命名以便样式表把它设为透明，露出主窗口自绘的渐变背景
    central->setObjectName(QStringLiteral("centralRoot"));
    auto *layout  = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(buildSider());

    m_pages = new QStackedWidget(central);
    layout->addWidget(m_pages, 1);

    setCentralWidget(central);

    // ---- 注册页面（顺序与导航项一致） ------------------------------------
    const QStringList titles = registerPages();

    m_navTitles = titles;
    m_navList->blockSignals(true);
    for (const QString &title : titles) {
        auto *item = new QListWidgetItem(title, m_navList);
        item->setSizeHint(QSize(0, 40));
    }
    m_navList->blockSignals(false);
    m_navList->setCurrentRow(0);
    updateNavIcons();

    // ---- 信号 -------------------------------------------------------------
    QObject::connect(m_navList, &QListWidget::currentRowChanged, this,
                     &MainWindow::onNavChanged);
    QObject::connect(m_context, &AppContext::kernelRunningChanged, this,
                     &MainWindow::onKernelRunningChanged);
    QObject::connect(m_context, &AppContext::toast, this, &MainWindow::onToast);
    QObject::connect(m_context->hub(), &core::Hub::trafficUpdated, this,
                     &MainWindow::onTrafficUpdated);

    buildTray();

    // 启动已由入口完成（与窗口解耦，静默启动也要运行），这里只把界面
    // 同步到当前状态：状态栏端口、托盘文案，以及代理/规则列表的首次加载。
    onKernelRunningChanged(m_context->isKernelRunning());
}

QWidget *MainWindow::buildSider()
{
    auto *sider = new QWidget(this);
    sider->setObjectName(QStringLiteral("sider"));
    sider->setFixedWidth(m_context->appConfig()->siderWidth());

    auto *siderLayout = new QVBoxLayout(sider);
    siderLayout->setContentsMargins(10, 12, 10, 12);
    siderLayout->setSpacing(10);

    // ---- 应用标识（图标 + 名称 + 版本） -----------------------------------
    auto *brand = new QWidget(sider);
    brand->setObjectName(QStringLiteral("brand"));
    auto *brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(4, 2, 4, 4);
    brandLayout->setSpacing(10);

    auto *logo = new QLabel(brand);
    logo->setFixedSize(30, 30);
    logo->setPixmap(ui::icons::appPixmap(30));
    brandLayout->addWidget(logo);

    auto *brandTexts = new QWidget(brand);
    auto *brandTextLayout = new QVBoxLayout(brandTexts);
    brandTextLayout->setContentsMargins(0, 0, 0, 0);
    brandTextLayout->setSpacing(0);

    auto *brandName = new QLabel(QStringLiteral("Clash Qt"), brandTexts);
    brandName->setObjectName(QStringLiteral("brandName"));
    brandTextLayout->addWidget(brandName);

    auto *brandVersion =
        new QLabel(QStringLiteral("v%1").arg(QLatin1String(CLASHQT_VERSION)), brandTexts);
    brandVersion->setObjectName(QStringLiteral("brandVersion"));
    brandTextLayout->addWidget(brandVersion);

    brandLayout->addWidget(brandTexts, 1);
    siderLayout->addWidget(brand);

    // ---- 快捷设置卡片（出站模式 + 系统代理） -------------------------------
    auto *kernelCard = ui::makeCard(QStringLiteral("快捷设置"), sider);
    auto *kernelLayout = qobject_cast<QVBoxLayout *>(kernelCard->layout());

    m_modeCombo = new QComboBox(kernelCard);
    m_modeCombo->addItem(QStringLiteral("规则模式"), QStringLiteral("rule"));
    m_modeCombo->addItem(QStringLiteral("全局模式"), QStringLiteral("global"));
    m_modeCombo->addItem(QStringLiteral("直连模式"), QStringLiteral("direct"));
    const QString mode = m_context->controledConfig()->mode();
    const int modeIndex = m_modeCombo->findData(mode);
    m_modeCombo->setCurrentIndex(modeIndex >= 0 ? modeIndex : 0);
    kernelLayout->addWidget(ui::makeFormRow(QStringLiteral("出站模式"), m_modeCombo, QString(),
                                            kernelCard));

    m_sysProxySwitch = new QCheckBox(QStringLiteral("系统代理"), kernelCard);
    m_sysProxySwitch->setChecked(m_context->isSysProxyEnabled());
    m_sysProxySwitch->setEnabled(m_context->sysProxy()->backend() != SysProxy::Backend::None);
    kernelLayout->addWidget(m_sysProxySwitch);

    if (m_context->sysProxy()->backend() == SysProxy::Backend::None) {
        kernelLayout->addWidget(ui::makeHintLabel(
            QStringLiteral("未检测到系统代理后端（gsettings / kwriteconfig），系统代理不可用"),
            kernelCard));
    }

    siderLayout->addWidget(kernelCard);

    // ---- 统计卡片（上行 / 下行各一行，带方向图标） --------------------------
    auto *statsCard = ui::makeCard(QStringLiteral("统计"), sider);
    auto *statsLayout = qobject_cast<QVBoxLayout *>(statsCard->layout());

    // 生成一行「方向图标 + 名称 + 数值」，返回数值标签供实时刷新
    auto makeStatRow = [&](const QString &iconName, const QColor &color,
                           const QString &caption) {
        auto *row = new QWidget(statsCard);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(8);

        auto *iconLabel = new QLabel(row);
        iconLabel->setFixedSize(16, 16);
        iconLabel->setPixmap(ui::icons::pixmap(iconName, color, 16));
        rowLayout->addWidget(iconLabel);

        rowLayout->addWidget(new QLabel(caption, row));

        auto *value = new QLabel(QStringLiteral("0 B/s"), row);
        value->setObjectName(QStringLiteral("statValue"));
        value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        rowLayout->addWidget(value, 1);
        return value;
    };

    m_uploadLabel = makeStatRow(QStringLiteral("arrowup"),
                                QColor(0xE0, 0x9B, 0x36), QStringLiteral("上行"));
    m_downloadLabel = makeStatRow(QStringLiteral("arrowdown"),
                                  QColor(0x2F, 0xA8, 0x7C), QStringLiteral("下行"));
    statsLayout->addWidget(m_uploadLabel->parentWidget());
    statsLayout->addWidget(m_downloadLabel->parentWidget());
    siderLayout->addWidget(statsCard);

    // ---- 导航 -------------------------------------------------------------
    m_navList = new QListWidget(sider);
    m_navList->setObjectName(QStringLiteral("navList"));
    siderLayout->addWidget(m_navList, 1);

    // ---- 信号 -------------------------------------------------------------
    QObject::connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &MainWindow::onOutboundModeChanged);
    QObject::connect(m_sysProxySwitch, &QCheckBox::toggled, this,
                     &MainWindow::onSysProxyToggled);

    return sider;
}

QStringList MainWindow::registerPages()
{
    QStringList titles;

    // 按引用捕获 titles 与 m_pages：lambda 需要写入二者
    auto add = [&](const QString &title, QWidget *page) {
        titles << title;
        m_pageContents << page;

        // 页面内容可能高于可视区（例如设置页有多张卡片）。放进滚动区域后，
        // 布局不会为了塞进窗口把「可压缩」的行（自动换行标签）压到 0 高度，
        // 否则会出现文字互相重叠。
        //
        // 外层再包一个留白容器：卡片带投影，投影会画到卡片矩形之外，
        // 紧贴滚动区边缘时会被裁掉。
        auto *host = new QWidget(this);
        host->setObjectName(QStringLiteral("pageHost"));
        auto *hostLayout = new QVBoxLayout(host);
        hostLayout->setContentsMargins(6, 6, 6, 6);
        hostLayout->addWidget(page);

        auto *scroll = new QScrollArea(this);
        scroll->setWidget(host);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->viewport()->setAutoFillBackground(false);
        m_pages->addWidget(scroll);
    };

    add(QStringLiteral("代理"),     new ProxiesPage(m_context, this));
    add(QStringLiteral("订阅"),     new ProfilesPage(m_context, this));
    add(QStringLiteral("连接"),     new ConnectionsPage(m_context, this));
    add(QStringLiteral("规则"),     new RulesPage(m_context, this));
    add(QStringLiteral("日志"),     new LogsPage(m_context, this));
    add(QStringLiteral("流量"),     new TrafficPage(m_context, this));
    add(QStringLiteral("设置"),     new SettingsPage(m_context, this));

    return titles;
}

void MainWindow::buildTray()
{
    if (m_context->appConfig()->disableTray()) return;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        AppLogger::instance().warn(QStringLiteral("系统托盘不可用，跳过托盘初始化"));
        return;
    }

    m_tray = new QSystemTrayIcon(this);

    // 专属图标（resources/icons/app.svg），与窗口图标一致
    m_tray->setIcon(ui::icons::appIcon(64));
    m_tray->setToolTip(QStringLiteral("Clash Qt"));

    auto *menu = new QMenu(this);

    // 内核不再有独立的启停入口：其生命周期跟随系统代理开关
    // （见 AppContext::setSysProxyEnabled）。
    m_trayToggleSysProxy = menu->addAction(QStringLiteral("关闭系统代理"));
    m_trayToggleSysProxy->setEnabled(m_context->sysProxy()->backend() != SysProxy::Backend::None);
    QObject::connect(m_trayToggleSysProxy, &QAction::triggered, this, [this]() {
        m_sysProxySwitch->setChecked(!m_sysProxySwitch->isChecked());
    });

    menu->addSeparator();
    QAction *showAction = menu->addAction(QStringLiteral("显示主窗口"));
    QObject::connect(showAction, &QAction::triggered, this, [this]() {
        showNormal();
        raise();
        activateWindow();
    });

    menu->addSeparator();
    QAction *quitAction = menu->addAction(QStringLiteral("退出"));
    QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_tray->setContextMenu(menu);

    // 单击托盘显示主窗口（对应原实现的 triggerMainWindowBehavior=show）
    QObject::connect(m_tray, &QSystemTrayIcon::activated, this,
                     [this](QSystemTrayIcon::ActivationReason reason) {
                         if (reason == QSystemTrayIcon::Trigger) {
                             if (isVisible()) {
                                 hide();
                             } else {
                                 showNormal();
                                 raise();
                                 activateWindow();
                             }
                         }
                     });

    m_tray->show();
    updateTray();
}

void MainWindow::updateTray()
{
    if (!m_tray) return;

    m_trayToggleSysProxy->setText(m_context->isSysProxyEnabled()
                                      ? QStringLiteral("关闭系统代理")
                                      : QStringLiteral("启用系统代理"));
}

void MainWindow::onNavChanged(int index)
{
    if (index < 0 || index >= m_pages->count()) return;
    m_pages->setCurrentIndex(index);
    updateNavIcons();

    // 切到某页时立即刷新一次，避免展示过期数据
    switch (index) {
    case 0: qobject_cast<ProxiesPage *>(m_pageContents.value(index))->refresh(); break;
    case 1: qobject_cast<ProfilesPage *>(m_pageContents.value(index))->refresh(); break;
    case 2: qobject_cast<ConnectionsPage *>(m_pageContents.value(index))->refresh(); break;
    case 3: qobject_cast<RulesPage *>(m_pageContents.value(index))->refresh(); break;
    case 5: qobject_cast<TrafficPage *>(m_pageContents.value(index)); break;
    default: break;
    }
}

void MainWindow::onKernelRunningChanged(bool running)
{
    statusBar()->showMessage(running
                                 ? QStringLiteral("混合端口 %1")
                                       .arg(m_context->controledConfig()->mixedPort())
                                 : QStringLiteral("未启动"));

    updateTray();

    // 启动后立即刷新代理与规则列表
    if (running) {
        if (auto *page = qobject_cast<ProxiesPage *>(m_pageContents.value(0))) page->refresh();
        if (auto *page = qobject_cast<RulesPage *>(m_pageContents.value(3))) page->refresh();
    }
}

void MainWindow::onOutboundModeChanged(int index)
{
    const QString mode = m_modeCombo->itemData(index).toString();
    QString error;
    if (!m_context->setOutboundMode(mode, &error)) {
        m_context->showToast(QStringLiteral("error"),
                             QStringLiteral("切换出站模式失败：%1").arg(error));
        return;
    }
    m_context->showToast(QStringLiteral("info"),
                         QStringLiteral("出站模式：%1").arg(m_modeCombo->itemText(index)));
}

void MainWindow::onSysProxyToggled(bool enabled)
{
    QString error;
    if (!m_context->setSysProxyEnabled(enabled, &error)) {
        // 恢复开关状态到实际状态，避免界面与系统不一致
        m_sysProxySwitch->blockSignals(true);
        m_sysProxySwitch->setChecked(m_context->isSysProxyEnabled());
        m_sysProxySwitch->blockSignals(false);
        return;
    }
    updateTray();
}

void MainWindow::onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed)
{
    if (m_uploadLabel) m_uploadLabel->setText(calc::speed(uploadSpeed));
    if (m_downloadLabel) m_downloadLabel->setText(calc::speed(downloadSpeed));

    // 托盘 tooltip 同步显示当前速度（对应原实现的 setToolTip）
    if (m_tray) {
        m_tray->setToolTip(QStringLiteral("Clash Qt\n↑ %1/s\n↓ %2/s")
                               .arg(calc::traffic(uploadSpeed),
                                    calc::traffic(downloadSpeed)));
    }
}

void MainWindow::onToast(const QString &type, const QString &message)
{
    statusBar()->showMessage(message, 5000);

    // 错误信息需要持久可见，仅用状态栏容易漏看
    if (type == QLatin1String("error")) {
        AppLogger::instance().error(message);
    }
}

void MainWindow::applyThemeFromConfig()
{
    // 主题名 → 深浅：system 跟随桌面配色方案，light / dark 为用户显式选择
    ui::applyThemeFor(m_context->appConfig()->appTheme());
}

void MainWindow::onSystemColorSchemeChanged()
{
    // 只有「跟随系统」才跟着变；显式选了浅色 / 深色时不受桌面影响
    if (m_context->appConfig()->appTheme() != QLatin1String("system")) return;

    ui::applyThemeFor(QStringLiteral("system"));
    update(); // 窗口背景是自绘的，换主题后需要重画
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    const QRectF area(rect());
    const ui::Theme &theme = ui::theme();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 对角渐变：为半透明卡片提供可透出的底色层次
    QLinearGradient background(area.topLeft(), area.bottomRight());
    background.setColorAt(0.0, theme.bgTop);
    background.setColorAt(1.0, theme.bgBottom);
    painter.fillRect(area, background);

    // 左上角强调色光晕：卡片是半透明的，光晕会从卡片下透出来，
    // 形成「磨砂玻璃后面有光」的观感
    QRadialGradient glow(QPointF(area.width() * 0.15, area.height() * 0.06),
                         qMax(area.width(), area.height()) * 0.55);
    QColor start = theme.glow;
    glow.setColorAt(0.0, start);
    start.setAlpha(0);
    glow.setColorAt(1.0, start);
    painter.fillRect(area, glow);
}

void MainWindow::updateNavIcons()
{
    if (!m_navList) return;

    const int current = m_navList->currentRow();
    for (int i = 0; i < m_navList->count(); ++i) {
        QListWidgetItem *item = m_navList->item(i);
        if (!item) continue;

        const QString title = m_navTitles.value(i, item->text());
        const QColor  color = (i == current) ? ui::theme().onAccent : ui::theme().text;
        item->setIcon(ui::icons::forNavTitle(title, color, 18));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 托盘可用时关闭窗口只是隐藏（最小化到托盘），与原实现行为一致
    if (m_tray && m_tray->isVisible()) {
        event->ignore();
        hide();
        return;
    }

    // 无托盘时退出前停止内核，避免留下孤立的监听端口
    m_context->stopKernel();
    event->accept();
}

} // namespace clashqt
