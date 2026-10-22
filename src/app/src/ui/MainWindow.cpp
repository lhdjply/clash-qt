#include "ui/MainWindow.h"
#include <QCoreApplication>

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "config/ProfileConfig.h"
#include "sys/SysProxy.h"
#include "ui/Icons.h"
#include "ui/TileButton.h"
#include "ui/UiFactory.h"
#include "ui/pages/ConnectionsPage.h"
#include "ui/pages/LogsPage.h"
#include "ui/pages/OverridePage.h"
#include "ui/pages/ProfilesPage.h"
#include "ui/pages/ProxiesPage.h"
#include "ui/pages/RulesPage.h"
#include "ui/pages/SettingsPage.h"
#include "ui/pages/TrafficPage.h"
#include "utils/AppLogger.h"
#include "utils/Calc.h"

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCloseEvent>
#include <QDateTime>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QStyleHints>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
#include <QRadialGradient>

#include "hub/Hub.h"

namespace clashqt {

namespace {

// 页面栈下标的固定约定（registerPages 的添加顺序）
enum PageIndex : int {
    PageProxies = 0,
    PageProfiles,
    PageConnections,
    PageRules,
    PageOverrides,
    PageLogs,
    PageTraffic,
    PageSettings,
};

// 出站模式分段控件的按钮 id
enum ModeId : int { ModeRule = 0, ModeGlobal, ModeDirect };

// 子控件不吃鼠标：卡片整体可点，点到的应该是外层的按钮
void passThroughMouse(QWidget *widget)
{
    widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

} // namespace

MainWindow::MainWindow(AppContext *context, QWidget *parent)
    : QMainWindow(parent), m_context(context)
{
    setWindowTitle(QStringLiteral("Clash Qt"));
    resize(1110, 620);
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

    registerPages();

    // ---- 信号 -------------------------------------------------------------
    QObject::connect(m_context, &AppContext::kernelRunningChanged, this,
                     &MainWindow::onKernelRunningChanged);
    QObject::connect(m_context, &AppContext::toast, this, &MainWindow::onToast);
    QObject::connect(m_context->hub(), &core::Hub::trafficUpdated, this,
                     &MainWindow::onTrafficUpdated);
    // 连接数随统计轮询变化，频率高，只刷新自身瓦片的徽标
    QObject::connect(m_context->hub(), &core::Hub::connectionsChanged, this,
                     &MainWindow::refreshConnectionBadge);
    // 规则 / 代理组随配置重载变化，徽标要跟着刷新（不是实时数据，不轮询）
    QObject::connect(m_context->hub(), &core::Hub::configChanged, this, [this]() {
        refreshBadges();
        refreshProfileCard();
    });
    QObject::connect(m_context->profileConfig(), &ProfileConfig::configChanged, this,
                     &MainWindow::refreshProfileCard);

    // 系统代理也可能在设置页里被改动，那里不走本窗口的开关回调；
    // 订阅配置变更信号后统一同步侧栏开关与托盘图标（含颜色），
    // 避免出现「设置页已开、托盘还是蓝的」
    QObject::connect(m_context->appConfig(), &AppConfig::configChanged, this, [this]() {
        const bool enabled = m_context->isSysProxyEnabled();
        if (m_sysProxyTile->isChecked() != enabled) {
            m_sysProxyTile->blockSignals(true);
            m_sysProxyTile->setChecked(enabled);
            m_sysProxyTile->blockSignals(false);
        }
        updateTray();
    });

    // TUN 的启用是异步的（拉起特权辅助程序 → 等 fd → 建数据面），
    // 完成/失败都只通过这个信号通知；三处开关（快捷设置、托盘、设置页）
    // 全部以它为准，避免出现「界面已开、实际没起来」
    QObject::connect(m_context, &AppContext::tunStateChanged, this,
                     [this](bool) { syncTunUi(); });

    buildTray();
    syncTunUi();

    // 启动已由入口完成（与窗口解耦，静默启动也要运行），这里只把界面
    // 同步到当前状态：状态栏端口、托盘文案，以及各卡片的首次加载。
    // 默认停在订阅页：打开软件先看当前订阅与用量，比看代理组更常用
    showPage(PageProfiles);
    refreshBadges();
    refreshProfileCard();
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

    siderLayout->addWidget(buildBrand());
    siderLayout->addWidget(buildModeSegment());
    siderLayout->addWidget(buildToggleTiles());
    siderLayout->addWidget(buildProfileCard());
    // 只有导航区域吸收多余高度：其余各块固定紧凑高度，窗口变高时
    // 空白均匀落在瓦片行之间，而不是在卡片内部撑出大片空洞
    siderLayout->addWidget(buildNavGrid(), 1);
    siderLayout->addWidget(buildTrafficCard());

    return sider;
}

QWidget *MainWindow::buildBrand()
{
    auto *brand = new QWidget(this);
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

    return brand;
}

QWidget *MainWindow::buildModeSegment()
{
    auto *bar = new QWidget(this);
    bar->setObjectName(QStringLiteral("segmentBar"));
    auto *barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(3, 3, 3, 3);
    barLayout->setSpacing(2);

    m_modeGroup = new QButtonGroup(this);
    m_modeGroup->setExclusive(true);

    const struct {
        int         id;
        const char *label;
    } entries[] = {
        {ModeRule, QT_TRANSLATE_NOOP("clashqt::MainWindow", "Rule")},
        {ModeGlobal, QT_TRANSLATE_NOOP("clashqt::MainWindow", "Global")},
        {ModeDirect, QT_TRANSLATE_NOOP("clashqt::MainWindow", "Direct")},
    };

    for (const auto &entry : entries) {
        auto *button = new QPushButton(tr(entry.label), bar);
        button->setObjectName(QStringLiteral("segment"));
        button->setCheckable(true);
        button->setCursor(Qt::PointingHandCursor);
        m_modeGroup->addButton(button, entry.id);
        barLayout->addWidget(button, 1);
    }

    const QString mode = m_context->controledConfig()->mode();
    int checked = ModeRule;
    if (mode == QLatin1String("global")) {
        checked = ModeGlobal;
    } else if (mode == QLatin1String("direct")) {
        checked = ModeDirect;
    }
    if (auto *button = m_modeGroup->button(checked)) button->setChecked(true);

    QObject::connect(m_modeGroup, &QButtonGroup::idClicked, this,
                     &MainWindow::onOutboundModeChanged);

    return bar;
}

QWidget *MainWindow::buildToggleTiles()
{
    auto *host = new QWidget(this);
    auto *layout = new QHBoxLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_sysProxyTile = new ui::TileButton(QStringLiteral("system"), tr("System proxy"), host);
    m_sysProxyTile->setTint(ui::Tint::Blue);
    // 比导航瓦片略高：图标与开关占一行，名称再占一行，78 刚好留出上下留白
    m_sysProxyTile->setFixedHeight(78);
    m_sysProxyTile->setCheckable(true);
    m_sysProxyTile->setSwitchVisible(true);
    m_sysProxyTile->setChecked(m_context->isSysProxyEnabled());
    m_sysProxyTile->setEnabled(m_context->sysProxy()->backend() != SysProxy::Backend::None);
    if (m_context->sysProxy()->backend() == SysProxy::Backend::None) {
        m_sysProxyTile->setToolTip(
            tr("No system proxy backend found (gsettings / kwriteconfig); system proxy is unavailable"));
    }
    layout->addWidget(m_sysProxyTile, 1);

    // TUN 模式：接管全部流量（不只是走系统代理的应用），首次启用会弹一次授权框。
    // 它与系统代理是两套独立机制，可以同时开启。
    m_tunTile = new ui::TileButton(QStringLiteral("shield"), tr("TUN mode"), host);
    m_tunTile->setTint(ui::Tint::Green);
    m_tunTile->setFixedHeight(78);
    m_tunTile->setCheckable(true);
    m_tunTile->setSwitchVisible(true);
    m_tunTile->setChecked(m_context->tunEnabled());
    m_tunTile->setToolTip(
        tr("Captures all local traffic, including apps that ignore the system proxy. The first enable requires a one-time password prompt."));
    layout->addWidget(m_tunTile, 1);

    QObject::connect(m_sysProxyTile, &ui::TileButton::toggled, this,
                     &MainWindow::onSysProxyToggled);
    QObject::connect(m_tunTile, &ui::TileButton::toggled, this,
                     &MainWindow::onTunToggleRequested);

    return host;
}

QWidget *MainWindow::buildProfileCard()
{
    // 整张卡可点：进订阅页。用按钮承载是为了拿到点击与悬停，内部标签一律
    // 让鼠标事件穿过去。
    auto *card = new QPushButton(this);
    card->setObjectName(QStringLiteral("profileCard"));
    card->setCursor(Qt::PointingHandCursor);
    // 订阅卡是侧栏里最高的一块，但要压住高度：内容只有名称与用量两行，
    // 拉得过高就成了一大片空蓝色
    card->setFixedHeight(100);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(11, 9, 11, 9);
    cardLayout->setSpacing(6);
    // 内容在卡内垂直居中（上下各留一段等量空白）
    cardLayout->addStretch(1);

    // 名称行右侧放刷新：手动拉一次当前订阅（仅远程订阅可用）
    auto *nameRow    = new QWidget(card);
    auto *nameLayout = new QHBoxLayout(nameRow);
    nameLayout->setContentsMargins(0, 0, 0, 0);
    nameLayout->setSpacing(6);

    m_profileName = new QLabel(nameRow);
    m_profileName->setObjectName(QStringLiteral("profileName"));
    m_profileName->setTextFormat(Qt::PlainText);
    m_profileName->setFixedHeight(17);
    m_profileName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLayout->addWidget(m_profileName, 1);

    m_profileRefresh = new QToolButton(nameRow);
    m_profileRefresh->setObjectName(QStringLiteral("profileRefresh"));
    m_profileRefresh->setCursor(Qt::PointingHandCursor);
    // 22×22 的方钮配 16px 图标：14px 时环形箭头只有 1px 描边，显得发虚
    m_profileRefresh->setFixedSize(22, 22);
    m_profileRefresh->setIconSize(QSize(16, 16));
    m_profileRefresh->setAutoRaise(true);
    m_profileRefresh->setToolTip(tr("Update this subscription"));
    nameLayout->addWidget(m_profileRefresh, 0, Qt::AlignVCenter);
    cardLayout->addWidget(nameRow);

    QObject::connect(m_profileRefresh, &QToolButton::clicked, this,
                     &MainWindow::refreshCurrentSubscription);

    auto *metaRow = new QWidget(card);
    metaRow->setFixedHeight(17);
    auto *metaLayout = new QHBoxLayout(metaRow);
    metaLayout->setContentsMargins(0, 0, 0, 0);
    metaLayout->setSpacing(6);

    m_profileUsage = new QLabel(metaRow);
    m_profileUsage->setObjectName(QStringLiteral("profileMeta"));
    metaLayout->addWidget(m_profileUsage, 1);

    m_profileExpire = new QLabel(metaRow);
    m_profileExpire->setObjectName(QStringLiteral("profileMeta"));
    m_profileExpire->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    metaLayout->addWidget(m_profileExpire);
    cardLayout->addWidget(metaRow);

    m_profileBar = new QProgressBar(card);
    m_profileBar->setObjectName(QStringLiteral("profileBar"));
    m_profileBar->setTextVisible(false);
    m_profileBar->setRange(0, 1000);
    m_profileBar->setValue(0);
    m_profileBar->setFixedHeight(4);
    cardLayout->addWidget(m_profileBar);
    cardLayout->addStretch(1);

    // 子控件一律让鼠标穿过去，只有刷新按钮（连同它所在的那一行容器）例外：
    // WA_TransparentForMouseEvents 连子控件一起穿透，容器设了按钮就点不到了
    for (QWidget *child : card->findChildren<QWidget *>()) {
        if (child == m_profileRefresh || child == m_profileRefresh->parentWidget()) continue;
        passThroughMouse(child);
    }

    QObject::connect(card, &QPushButton::clicked, this,
                     [this]() { showPage(PageProfiles); });

    m_profileCard = card;
    return m_profileCard;
}

QWidget *MainWindow::buildNavGrid()
{
    auto *host = new QWidget(this);
    auto *outer = new QVBoxLayout(host);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    auto *gridHost = new QWidget(host);
    auto *grid = new QGridLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(8);
    // 导航瓦片保持紧凑；网格吃下剩余高度，多出来的空间均分到行间，
    // 窗口拉高时是行距变大，而不是某个卡片被撑成一片空白
    outer->addWidget(gridHost, 1);

    const struct {
        int         page;
        const char *icon;
        const char *label;
        bool        badge;
        ui::Tint    tint;
    } entries[] = {
        {PageProxies, "proxy", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Proxies"), true,
         ui::Tint::Blue},
        {PageRules, "rules", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Rules"), true,
         ui::Tint::Purple},
        {PageOverrides, "code", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Overrides"), true,
         ui::Tint::Teal},
        {PageLogs, "logs", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Logs"), false,
         ui::Tint::Amber},
        {PageConnections, "connections", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Connections"),
         true, ui::Tint::Green},
        // 设置不需要识别色：它是配置入口，不和「有数据在动」的几页抢注意力
        {PageSettings, "settings", QT_TRANSLATE_NOOP("clashqt::MainWindow", "Settings"), false,
         ui::Tint::None},
    };

    for (int i = 0; i < int(std::size(entries)); ++i) {
        const auto &entry = entries[i];
        auto *tile = new ui::TileButton(QString::fromLatin1(entry.icon), tr(entry.label), host);
        tile->setTint(entry.tint);
        tile->setMinimumHeight(56);
        tile->setMaximumHeight(72);
        if (entry.badge) {
            switch (entry.page) {
            case PageProxies:     m_proxyTileIndex = i; break;
            case PageRules:       m_ruleTileIndex = i; break;
            case PageOverrides:   m_overrideTileIndex = i; break;
            case PageConnections: m_connectionTileIndex = i; break;
            default:              break;
            }
        }

        m_navTiles << tile;
        m_navPages << entry.page;
        grid->addWidget(tile, i / 2, i % 2);

        QObject::connect(tile, &ui::TileButton::clicked, this,
                         [this, page = entry.page]() { showPage(page); });
    }

    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    // 行也拉伸：多余高度均分到三行，瓦片在各自行内垂直居中
    for (int row = 0; row < (int(std::size(entries)) + 1) / 2; ++row) {
        grid->setRowStretch(row, 1);
    }
    return host;
}

QWidget *MainWindow::buildTrafficCard()
{
    // 网速属于流量，入口与实时速率放在同一张卡上：整卡可点，进流量页
    auto *card = new QPushButton(this);
    card->setObjectName(QStringLiteral("trafficCard"));
    card->setCursor(Qt::PointingHandCursor);
    card->setFixedHeight(62);

    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(11, 9, 11, 9);
    layout->setSpacing(8);

    auto *left = new QWidget(card);
    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(3);

    m_trafficIcon = new QLabel(left);
    m_trafficIcon->setFixedSize(16, 16);
    m_trafficIcon->setPixmap(
        ui::icons::pixmap(QStringLiteral("traffic"), ui::tintColor(ui::Tint::Teal), 16));
    leftLayout->addWidget(m_trafficIcon);
    leftLayout->addStretch(1);

    m_trafficTitle = new QLabel(tr("Traffic"), left);
    m_trafficTitle->setObjectName(QStringLiteral("trafficTitle"));
    leftLayout->addWidget(m_trafficTitle);
    layout->addWidget(left, 1);

    auto *right = new QWidget(card);
    auto *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(3);
    rightLayout->addStretch(1);

    m_uploadLabel = new QLabel(QStringLiteral("↑ 0 B/s"), right);
    m_uploadLabel->setObjectName(QStringLiteral("speedValue"));
    m_uploadLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rightLayout->addWidget(m_uploadLabel);

    m_downloadLabel = new QLabel(QStringLiteral("↓ 0 B/s"), right);
    m_downloadLabel->setObjectName(QStringLiteral("speedValue"));
    m_downloadLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rightLayout->addWidget(m_downloadLabel);
    rightLayout->addStretch(1);

    layout->addWidget(right, 0, Qt::AlignVCenter);

    for (QWidget *child : card->findChildren<QWidget *>()) passThroughMouse(child);

    QObject::connect(card, &QPushButton::clicked, this,
                     [this]() { showPage(PageTraffic); });

    m_trafficCard = card;
    return card;
}

void MainWindow::updateProfileCardState()
{
    if (!m_profileCard) return;

    // 订阅卡与流量卡同理：只有停在订阅页时才是强调色底，其余时候是普通卡片，
    // 否则侧栏会同时出现两块高饱和蓝，看不出「当前在哪一页」
    const bool active = (m_currentPage == PageProfiles);

    for (QWidget *widget : {m_profileCard,
                            static_cast<QWidget *>(m_profileName),
                            static_cast<QWidget *>(m_profileUsage),
                            static_cast<QWidget *>(m_profileExpire),
                            static_cast<QWidget *>(m_profileBar),
                            static_cast<QWidget *>(m_profileRefresh)}) {
        if (!widget) continue;
        widget->setProperty("active", active);
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }

    // 图标是位图，颜色得跟着底色换：强调色底上用反色，普通卡上用正文色
    m_profileRefresh->setIcon(ui::icons::get(QStringLiteral("refresh"),
                                            active ? ui::theme().onAccent : ui::theme().textMuted,
                                            16));
}

void MainWindow::updateTrafficCardState()
{
    if (!m_trafficCard) return;

    // 卡片本身不是导航瓦片，选中态靠动态属性交给样式表，
    // 属性变更后必须重新 polish 才会重绘
    const bool active = (m_currentPage == PageTraffic);
    // 选中时整卡是强调色底，图标得用反色；平时跟随「流量」的识别色
    const QColor foreground = active ? ui::theme().onAccent : ui::tintColor(ui::Tint::Teal);

    for (QWidget *widget : {static_cast<QWidget *>(m_trafficCard),
                            static_cast<QWidget *>(m_trafficTitle),
                            static_cast<QWidget *>(m_uploadLabel),
                            static_cast<QWidget *>(m_downloadLabel)}) {
        if (!widget) continue;
        widget->setProperty("active", active);
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }

    m_trafficIcon->setPixmap(
        ui::icons::pixmap(QStringLiteral("traffic"), foreground, 16));
}

void MainWindow::registerPages()
{
    // 页面内容可能高于可视区（例如设置页有多张卡片）。放进滚动区域后，
    // 布局不会为了塞进窗口把「可压缩」的行（自动换行标签）压到 0 高度，
    // 否则会出现文字互相重叠。
    //
    // 外层再包一个留白容器：卡片带投影，投影会画到卡片矩形之外，
    // 紧贴滚动区边缘时会被裁掉。
    auto add = [this](QWidget *page) {
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

        m_pageContents << page;
    };

    add(new ProxiesPage(m_context, this));
    add(new ProfilesPage(m_context, this));
    add(new ConnectionsPage(m_context, this));
    add(new RulesPage(m_context, this));
    add(new OverridePage(m_context, this));
    add(new LogsPage(m_context, this));
    add(new TrafficPage(m_context, this));
    add(new SettingsPage(m_context, this));
}

void MainWindow::showPage(int page)
{
    if (page < 0 || page >= m_pages->count()) return;

    m_pages->setCurrentIndex(page);
    m_currentPage = page;
    updateTileStates();

    // 切到某页时立即刷新一次，避免展示过期数据
    switch (page) {
    case PageProxies:
        if (auto *p = qobject_cast<ProxiesPage *>(m_pageContents.value(page))) p->refresh();
        break;
    case PageProfiles:
        if (auto *p = qobject_cast<ProfilesPage *>(m_pageContents.value(page))) p->refresh();
        break;
    case PageConnections:
        if (auto *p = qobject_cast<ConnectionsPage *>(m_pageContents.value(page))) p->refresh();
        break;
    case PageRules:
        if (auto *p = qobject_cast<RulesPage *>(m_pageContents.value(page))) p->refresh();
        break;
    default:
        break;
    }
}

void MainWindow::updateTileStates()
{
    for (int i = 0; i < m_navTiles.size(); ++i) {
        m_navTiles.at(i)->setAccentFill(m_navPages.value(i) == m_currentPage);
    }
    updateProfileCardState();
    updateTrafficCardState();
}

void MainWindow::refreshBadges()
{
    const bool running = m_context->isKernelRunning();

    int groups = 0;
    int rules  = 0;
    if (running && m_context->hub()) {
        // 含 all 字段的条目才是组，节点条目没有；GLOBAL 是内核自带的固定组，
        // 不计入订阅自身的组数
        const QJsonObject proxies =
            m_context->hub()->proxiesStatus().value(QStringLiteral("proxies")).toObject();
        for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
            if (it.key() == QLatin1String("GLOBAL")) continue;
            const QJsonObject entry = it.value().toObject();
            if (entry.contains(QStringLiteral("all"))
                && !entry.contains(QStringLiteral("unsupported"))) {
                ++groups;
            }
        }
        rules = m_context->hub()->rulesStatus().value(QStringLiteral("rules")).toArray().size();
    }

    if (m_proxyTileIndex >= 0) m_navTiles.at(m_proxyTileIndex)->setBadge(running ? groups : -1);
    if (m_ruleTileIndex >= 0) m_navTiles.at(m_ruleTileIndex)->setBadge(running ? rules : -1);

    // 覆写条目存在本地规则文件里，与内核是否运行无关；保存后会重载内核，
    // 由 kernelRunningChanged 回到这里刷新
    const int overrides = m_context->ruleOverride().prepend.size();
    if (m_overrideTileIndex >= 0) {
        m_navTiles.at(m_overrideTileIndex)->setBadge(overrides > 0 ? overrides : -1);
    }

    refreshConnectionBadge();
}

void MainWindow::refreshConnectionBadge()
{
    if (m_connectionTileIndex < 0) return;

    // 连接数跟着统计轮询每秒变一次，单独成一条轻量路径：不能每秒都去
    // 重算代理组、规则条数，更不能每秒读一次规则覆写文件
    int count = 0;
    if (m_context->isKernelRunning() && m_context->hub()) {
        count = m_context->hub()->statistics().value(QStringLiteral("connectionCount")).toInt();
    }
    m_navTiles.at(m_connectionTileIndex)->setBadge(count > 0 ? count : -1);
}

void MainWindow::refreshProfileCard()
{
    if (!m_profileCard) return;

    const ProfileItem item = m_context->profileConfig()->currentItem();

    if (item.name.isEmpty()) {
        m_profileName->setText(tr("No profile selected"));
        m_profileUsage->setText(QStringLiteral("—"));
        m_profileExpire->setText(QStringLiteral("—"));
        m_profileBar->hide();
        m_profileRefresh->setEnabled(false);
        return;
    }

    m_profileName->setText(item.name);

    // 只有远程订阅能重新拉取；本地订阅换了文件要走订阅页的「替换内容」
    const bool refreshable = (item.type == QLatin1String("remote") && !item.url.isEmpty());
    m_profileRefresh->setEnabled(refreshable);
    m_profileRefresh->setToolTip(refreshable
                                     ? tr("Update this subscription")
                                     : tr("Local profiles have no URL to update from"));

    qint64 used   = 0;
    qint64 total  = 0;
    qint64 expire = 0;
    if (item.extra) {
        used   = item.extra->upload + item.extra->download;
        total  = item.extra->total;
        expire = item.extra->expire;
    }

    if (total > 0) {
        m_profileUsage->setText(tr("%1 / %2").arg(calc::traffic(used), calc::traffic(total)));
        const double ratio = double(used) / double(total);
        m_profileBar->setValue(int(qBound(0.0, ratio, 1.0) * 1000));
        m_profileBar->show();
    } else {
        // 本地配置与从未更新过的订阅没有 Subscription-UserInfo 头，没有总量
        // 可画进度条；改显示最近更新时间，比一个占位破折号有用
        m_profileUsage->setText(
            item.updated > 0
                ? tr("Updated %1").arg(QDateTime::fromSecsSinceEpoch(item.updated)
                                           .toString(QStringLiteral("yyyy-MM-dd HH:mm")))
                : QStringLiteral("—"));
        m_profileBar->hide();
    }

    // 没有到期日就留空：右侧摆一个破折号既没有信息量，也会和左侧的
    // 「更新于 …」挤在一起
    if (expire > 0) {
        m_profileExpire->setText(
            QDateTime::fromSecsSinceEpoch(expire).toString(QStringLiteral("yyyy-MM-dd")));
    } else {
        m_profileExpire->clear();
    }

    m_profileCard->setToolTip(tr("Open the profiles page"));
}

void MainWindow::refreshCurrentSubscription()
{
    const ProfileItem item = m_context->profileConfig()->currentItem();
    if (item.id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a profile first"));
        return;
    }
    if (item.type != QLatin1String("remote") || item.url.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"),
                             tr("Local profiles have no URL to update from"));
        return;
    }

    // 下载是同步的：先禁用按钮，避免用户连点叠出多次请求
    m_profileRefresh->setEnabled(false);
    QString error;
    if (!m_context->updateSubscription(item.id, &error)) {
        m_context->showToast(QStringLiteral("error"), tr("Update failed: %1").arg(error));
    } else {
        m_context->showToast(QStringLiteral("info"), tr("Subscription updated"));
    }
    // 按钮可用性与卡片上的用量 / 更新时间都按最新状态重画
    refreshProfileCard();
}

void MainWindow::buildTray()
{
    if (m_context->appConfig()->disableTray()) return;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        AppLogger::instance().warn(QCoreApplication::translate("clashqt::app", "System tray unavailable; skipping tray setup"));
        return;
    }

    m_tray = new QSystemTrayIcon(this);

    // 专属图标（resources/icons/app.svg），与窗口图标一致
    m_tray->setIcon(ui::icons::appIcon(64));
    m_tray->setToolTip(QStringLiteral("Clash Qt"));

    auto *menu = new QMenu(this);

    // 内核不再有独立的启停入口：其生命周期跟随系统代理开关
    // （见 AppContext::setSysProxyEnabled）。
    m_trayToggleSysProxy = menu->addAction(tr("Disable system proxy"));
    m_trayToggleSysProxy->setEnabled(m_context->sysProxy()->backend() != SysProxy::Backend::None);
    QObject::connect(m_trayToggleSysProxy, &QAction::triggered, this, [this]() {
        m_sysProxyTile->setChecked(!m_sysProxyTile->isChecked());
    });

    // TUN 与系统代理并列：不读系统代理的程序（游戏、容器、命令行工具）
    // 只有 TUN 能接管，所以托盘里必须能直接开关
    m_trayToggleTun = menu->addAction(tr("Enable TUN mode"));
    QObject::connect(m_trayToggleTun, &QAction::triggered, this, [this]() {
        // 走与快捷设置同一条入口：失败会 toast，状态以 AppContext 为准
        onTunToggleRequested(!m_context->tunEnabled());
    });

    menu->addSeparator();
    QAction *showAction = menu->addAction(tr("Show main window"));
    QObject::connect(showAction, &QAction::triggered, this, [this]() {
        showNormal();
        raise();
        activateWindow();
    });

    menu->addSeparator();
    QAction *quitAction = menu->addAction(tr("Quit"));
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

    const bool sysProxy = m_context->isSysProxyEnabled();
    const bool tun      = m_context->tunEnabled();

    // 托盘图标随「用哪种方式接管流量」变色：
    //   都没开 → 蓝紫；仅系统代理 → 红；仅 TUN → 绿；两者同时 → 紫
    ui::icons::AppIconState state = ui::icons::AppIconState::Normal;
    if (sysProxy && tun) {
        state = ui::icons::AppIconState::Both;
    } else if (tun) {
        state = ui::icons::AppIconState::Tun;
    } else if (sysProxy) {
        state = ui::icons::AppIconState::Active;
    }
    m_tray->setIcon(ui::icons::appIcon(64, state));

    const int stateValue = int(state);
    if (!m_trayIconApplied || stateValue != m_trayIconState) {
        m_trayIconState   = stateValue;
        m_trayIconApplied = true;
        for (int delayMs : {400, 1600}) {
            QTimer::singleShot(delayMs, this, [this, state, stateValue]() {
                if (!m_tray || stateValue != m_trayIconState) return;
                m_tray->setIcon(ui::icons::appIcon(64, state));
            });
        }
    }

    m_tray->setToolTip(QStringLiteral("Clash Qt · %1").arg(trayStateText()));

    m_trayToggleSysProxy->setText(sysProxy ? tr("Disable system proxy")
                                           : tr("Enable system proxy"));
}

QString MainWindow::trayStateText() const
{
    if (m_context->tunStarting()) return tr("Enabling TUN…");

    const bool sysProxy = m_context->isSysProxyEnabled();
    const bool tun      = m_context->tunEnabled();
    if (sysProxy && tun) return tr("System proxy + TUN enabled");
    if (tun) return tr("TUN enabled");
    if (sysProxy) return tr("System proxy enabled");
    return tr("System proxy disabled");
}

// 快捷设置与托盘菜单共用：显示状态一律以 AppContext 为准。
// 启用是异步的（要拉起特权辅助程序），所以「启用中」单独占一个状态，
// 否则开关会在点下之后立刻弹回未勾选（看起来像点击失效）。
void MainWindow::syncTunUi()
{
    const bool running  = m_context->tunEnabled();
    const bool starting = m_context->tunStarting();

    if (m_tunTile) {
        m_tunTile->blockSignals(true);
        m_tunTile->setChecked(running || starting);
        m_tunTile->blockSignals(false);
        m_tunTile->setEnabled(!starting);
        m_tunTile->setHint(starting ? tr("enabling…") : QString());
    }

    if (m_trayToggleTun) {
        m_trayToggleTun->setEnabled(!starting);
        m_trayToggleTun->setText(starting ? tr("Enabling TUN…")
                                          : (running ? tr("Disable TUN mode")
                                                     : tr("Enable TUN mode")));
    }

    updateTray();
}

void MainWindow::onKernelRunningChanged(bool running)
{
    statusBar()->showMessage(running
                                 ? tr("Mixed port %1")
                                       .arg(m_context->controledConfig()->mixedPort())
                                 : tr("Not running"));

    updateTray();
    refreshBadges();

    // 启动后立即刷新代理与规则列表
    if (running) {
        if (auto *page = qobject_cast<ProxiesPage *>(m_pageContents.value(PageProxies))) {
            page->refresh();
        }
        if (auto *page = qobject_cast<RulesPage *>(m_pageContents.value(PageRules))) {
            page->refresh();
        }
    }
}

void MainWindow::onOutboundModeChanged(int id)
{
    QString mode = QStringLiteral("rule");
    if (id == ModeGlobal) {
        mode = QStringLiteral("global");
    } else if (id == ModeDirect) {
        mode = QStringLiteral("direct");
    }

    QString error;
    if (!m_context->setOutboundMode(mode, &error)) {
        m_context->showToast(QStringLiteral("error"),
                             tr("Failed to switch outbound mode: %1").arg(error));
        return;
    }

    if (auto *button = m_modeGroup->checkedButton()) {
        m_context->showToast(QStringLiteral("info"),
                             tr("Outbound mode: %1").arg(button->text()));
    }

    // 模式变化会影响代理组的可见性（GLOBAL 只在全局模式列出）
    refreshBadges();
}

void MainWindow::onSysProxyToggled(bool enabled)
{
    QString error;
    if (!m_context->setSysProxyEnabled(enabled, &error)) {
        // 恢复开关状态到实际状态，避免界面与系统不一致
        m_sysProxyTile->blockSignals(true);
        m_sysProxyTile->setChecked(m_context->isSysProxyEnabled());
        m_sysProxyTile->blockSignals(false);
        return;
    }
    updateTray();
}

// 快捷设置与托盘菜单共用同一个入口，避免两处逻辑出现分歧
void MainWindow::onTunToggleRequested(bool enabled)
{
    // 正在启用（等特权辅助程序回 fd）时忽略重复请求；AppContext 内部也会挡，
    // 这里只是让界面不去抖那一下
    if (enabled && m_context->tunStarting()) {
        syncTunUi();
        return;
    }

    QString error;
    if (!m_context->setTunEnabled(enabled, &error)) {
        if (!error.isEmpty()) emit m_context->toast(QStringLiteral("error"), error);
        syncTunUi(); // 回滚到 AppContext 的真实状态
        return;
    }
    // 启用是异步的：这里先显示「启用中…」，成功/失败由 tunStateChanged 收尾
    syncTunUi();
}

void MainWindow::onTrafficUpdated(qint64 uploadSpeed, qint64 downloadSpeed)
{
    if (m_uploadLabel) {
        m_uploadLabel->setText(QStringLiteral("↑ %1").arg(calc::speed(uploadSpeed)));
    }
    if (m_downloadLabel) {
        m_downloadLabel->setText(QStringLiteral("↓ %1").arg(calc::speed(downloadSpeed)));
    }

    // 托盘 tooltip 同步显示当前速度（对应原实现的 setToolTip）。
    // 速度每秒刷新、会覆盖 updateTray 里设的文案，所以这里必须把接管状态
    // 一起带上，否则开了 TUN 之后 tooltip 里就再也看不到状态了。
    if (m_tray) {
        m_tray->setToolTip(QStringLiteral("Clash Qt · %1\n↑ %2/s\n↓ %3/s")
                               .arg(trayStateText(), calc::traffic(uploadSpeed),
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
