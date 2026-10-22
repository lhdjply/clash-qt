#include "ui/pages/SettingsPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "sys/AutoRun.h"
#include "sys/SysProxy.h"
#include "ui/UiFactory.h"
#include "ui/UpdateDialog.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "updater/UpdateChecker.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QUrl>
#include <QVBoxLayout>

#include "constant/Constants.h"

namespace clashqt {
namespace {

constexpr int kMaxPort = 65535;

} // namespace

SettingsPage::SettingsPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Settings"),
        tr("Logs, DNS overrides, ports and system integration. Changes are saved and applied immediately."), QStringLiteral("settings")));

    // ---- 界面 -------------------------------------------------------------
    auto *appearanceCard = ui::makeCard(tr("Appearance"), this, QStringLiteral("settings"));
    auto *appearanceLayout = qobject_cast<QVBoxLayout *>(appearanceCard->layout());

    m_themeCombo = new QComboBox(appearanceCard);
    m_themeCombo->addItem(tr("Follow system"), QStringLiteral("system"));
    m_themeCombo->addItem(tr("Light"), QStringLiteral("light"));
    m_themeCombo->addItem(tr("Dark"), QStringLiteral("dark"));
    // 滚轮不参与改值：主题是有明显视觉后果的设置，鼠标划过时被滚轮顺手切走
    // 很难察觉（尤其「跟随系统 / 深色」之间的差异在某些桌面下并不明显）。
    // 忽略后事件冒泡给页面滚动区，滚轮照常滚动页面。
    ui::ignoreWheel(m_themeCombo);
    appearanceLayout->addWidget(
        ui::makeFormRow(tr("Theme"), m_themeCombo, QString(), appearanceCard));

    m_languageCombo = new QComboBox(appearanceCard);
    // 语言名用各自的语言写（简体中文 / English），不随界面语言变化：
    // 英文界面下把「简体中文」翻成 Simplified Chinese 反而让中文用户难找。
    m_languageCombo->addItem(tr("Follow system"), QStringLiteral("system"));
    m_languageCombo->addItem(QStringLiteral("简体中文"), QStringLiteral("zh-CN"));
    m_languageCombo->addItem(QStringLiteral("English"), QStringLiteral("en-US"));
    ui::ignoreWheel(m_languageCombo);
    appearanceLayout->addWidget(ui::makeFormRow(
        tr("Language"), m_languageCombo,
        tr("Interface text is loaded at startup; restart the app after switching."), appearanceCard));

    root->addWidget(appearanceCard);

    // ---- 日志 -------------------------------------------------------------
    auto *logCard = ui::makeCard(tr("Logs"), this, QStringLiteral("logs"));
    auto *logLayout = qobject_cast<QVBoxLayout *>(logCard->layout());

    m_logLevelCombo = new QComboBox(logCard);
    for (const QString &level : core::logLevelNames()) m_logLevelCombo->addItem(level);
    // 滚轮不参与改值，避免鼠标划过时误改（滚轮留给页面滚动）
    ui::ignoreWheel(m_logLevelCombo);
    logLayout->addWidget(ui::makeFormRow(tr("Log level"), m_logLevelCombo,
                                         QString(), logCard));

    m_maxLogDaysSpin = new QSpinBox(logCard);
    m_maxLogDaysSpin->setRange(1, 365);
    m_maxLogDaysSpin->setSuffix(tr(" days"));
    // 滚轮不参与改值，避免鼠标划过时误改（滚轮留给页面滚动）
    ui::ignoreWheel(m_maxLogDaysSpin);
    logLayout->addWidget(ui::makeFormRow(tr("Log retention (days)"), m_maxLogDaysSpin,
                                         tr("Log files older than the retention period are removed at startup."),
                                         logCard));

    auto *openDir = new QPushButton(tr("Open data folder"), logCard);
    ui::decorate(openDir, QStringLiteral("folder"));
    logLayout->addWidget(openDir);

    m_dataDirLabel = ui::makeHintLabel(QString(), logCard);
    logLayout->addWidget(m_dataDirLabel);
    root->addWidget(logCard);

    // ---- 配置覆写 ---------------------------------------------------------
    auto *controlCard = ui::makeCard(tr("Config overrides"), this, QStringLiteral("code"));
    auto *controlLayout = qobject_cast<QVBoxLayout *>(controlCard->layout());

    m_controlDns = new QCheckBox(tr("Manage DNS settings in this app"), controlCard);
    controlLayout->addWidget(m_controlDns);
    controlLayout->addWidget(ui::makeHintLabel(
        tr("When on, the DNS section configured here is used; when off, the profile's own DNS settings apply."),
        controlCard));

    m_controlSniff = new QCheckBox(tr("Manage domain sniffing in this app"), controlCard);
    controlLayout->addWidget(m_controlSniff);

    root->addWidget(controlCard);

    // ---- 端口 -------------------------------------------------------------
    auto *portCard = ui::makeCard(tr("Inbound ports"), this, QStringLiteral("port"));
    auto *portLayout = qobject_cast<QVBoxLayout *>(portCard->layout());

    m_mixedPort = new QSpinBox(portCard);
    m_mixedPort->setRange(1, kMaxPort);
    m_socksPort = new QSpinBox(portCard);
    m_socksPort->setRange(1, kMaxPort);
    m_httpPort = new QSpinBox(portCard);
    m_httpPort->setRange(1, kMaxPort);
    // 端口误触的代价是内核要重新监听，这里一并禁掉滚轮改值
    for (QSpinBox *spin : {m_mixedPort, m_socksPort, m_httpPort}) {
        ui::ignoreWheel(spin);
    }

    portLayout->addWidget(ui::makeFormRow(tr("Mixed port (HTTP + SOCKS5)"),
                                          m_mixedPort, QString(), portCard));
    portLayout->addWidget(ui::makeFormRow(tr("SOCKS5 port"), m_socksPort,
                                         tr("Set it equal to the mixed port to reuse the mixed port."),
                                         portCard));
    portLayout->addWidget(ui::makeFormRow(tr("HTTP port"), m_httpPort, QString(),
                                         portCard));
    root->addWidget(portCard);

    // ---- 系统集成 ---------------------------------------------------------
    auto *systemCard = ui::makeCard(tr("System integration"), this, QStringLiteral("system"));
    auto *systemLayout = qobject_cast<QVBoxLayout *>(systemCard->layout());

    m_autoStart = new QCheckBox(tr("Start at login"), systemCard);
    systemLayout->addWidget(m_autoStart);
    systemLayout->addWidget(ui::makeHintLabel(tr("Implemented via an XDG autostart entry (~/.config/autostart/clash-qt.desktop)."),
                                             systemCard));

    m_sysProxy = new QCheckBox(tr("System proxy"), systemCard);
    systemLayout->addWidget(m_sysProxy);
    systemLayout->addWidget(ui::makeHintLabel(
        tr("Points the system HTTP/HTTPS/SOCKS proxy at this app's mixed port. Current backend: %1").arg(m_context->sysProxy()->backendName()),
        systemCard));

    // ---- TUN 模式 --------------------------------------------------------
    m_tunEnabled = new QCheckBox(tr("TUN mode"), systemCard);
    systemLayout->addWidget(m_tunEnabled);
    systemLayout->addWidget(ui::makeHintLabel(
        tr("Creates a virtual network interface to capture all traffic, including apps that ignore the system proxy (games, CLI tools, some Electron apps). The first enable asks for your password once; it only takes effect together with \"Route domain traffic\"."),
        systemCard));

    m_tunAutoRoute = new QCheckBox(tr("Route domain traffic (fake-IP)"), systemCard);
    systemLayout->addWidget(m_tunAutoRoute);
    systemLayout->addWidget(ui::makeHintLabel(
        tr("Routes the fake-IP range to the TUN interface (removed together with the interface). The default route is untouched, so connectivity is unaffected; with this off, TUN receives no traffic."),
        systemCard));

    root->addWidget(systemCard);

    // ---- 关于 / 检查更新 ---------------------------------------------------
    auto *aboutCard = ui::makeCard(tr("About"), this, QStringLiteral("info"));
    auto *aboutLayout = qobject_cast<QVBoxLayout *>(aboutCard->layout());

    auto *versionLabel =
        new QLabel(tr("Version v%1").arg(QLatin1String(CLASHQT_VERSION)), aboutCard);
    aboutLayout->addWidget(versionLabel);

    m_checkUpdate = new QPushButton(tr("Check for updates"), aboutCard);
    ui::decorate(m_checkUpdate, QStringLiteral("refresh"));
    auto *updateRow = new QHBoxLayout();
    updateRow->setContentsMargins(0, 0, 0, 0);
    updateRow->setSpacing(8);
    updateRow->addWidget(m_checkUpdate);
    updateRow->addStretch(1);
    aboutLayout->addLayout(updateRow);

    aboutLayout->addWidget(ui::makeHintLabel(
        tr("Fetches the latest version from GitHub Releases. When an update exists, its changelog is shown and the release page can be opened in your browser; downloading and installing is up to you."),
        aboutCard));
    root->addWidget(aboutCard);

    root->addStretch(1);

    // ---- 信号 -------------------------------------------------------------
    QObject::connect(m_logLevelCombo, &QComboBox::currentTextChanged, this,
                     &SettingsPage::onLogLevelChanged);
    QObject::connect(m_maxLogDaysSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &SettingsPage::onMaxLogDaysChanged);
    QObject::connect(m_controlDns, &QCheckBox::toggled, this, &SettingsPage::onControlDnsToggled);
    QObject::connect(m_controlSniff, &QCheckBox::toggled, this,
                     &SettingsPage::onControlSniffToggled);
    QObject::connect(m_mixedPort, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &SettingsPage::onPortChanged);
    QObject::connect(m_socksPort, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &SettingsPage::onPortChanged);
    QObject::connect(m_httpPort, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &SettingsPage::onPortChanged);
    QObject::connect(m_autoStart, &QCheckBox::toggled, this, &SettingsPage::onAutoStartToggled);
    QObject::connect(m_sysProxy, &QCheckBox::toggled, this, &SettingsPage::onSysProxyToggled);
    QObject::connect(m_tunEnabled, &QCheckBox::toggled, this, &SettingsPage::onTunToggled);
    QObject::connect(m_tunAutoRoute, &QCheckBox::toggled, this,
                     &SettingsPage::onTunAutoRouteToggled);
    // 异步启用完成/失败后同步开关，避免「界面已勾上、实际没起来」
    QObject::connect(m_context, &clashqt::AppContext::tunStateChanged, this,
                     [this](bool enabled) {
                         m_loading = true;
                         m_tunEnabled->setChecked(enabled);
                         m_tunEnabled->setEnabled(true); // 启用流程已结束，解除临时禁用
                         m_loading = false;
                     });
    QObject::connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &SettingsPage::onThemeChanged);
    QObject::connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &SettingsPage::onLanguageChanged);
    QObject::connect(openDir, &QPushButton::clicked, this, &SettingsPage::onOpenDataDir);

    // ---- 检查更新 ---------------------------------------------------------
    m_updater = new UpdateChecker(this);
    QObject::connect(m_checkUpdate, &QPushButton::clicked, this, &SettingsPage::onCheckUpdate);

    QObject::connect(m_updater, &UpdateChecker::updateAvailable, this,
                     [this](const ReleaseInfo &release) {
                         setUpdateCheckBusy(false);
                         UpdateDialog dialog(release, this);
                         dialog.exec();
                     });
    QObject::connect(m_updater, &UpdateChecker::upToDate, this, [this](const QString &current) {
        setUpdateCheckBusy(false);
        emit m_context->toast(QStringLiteral("info"),
                              tr("Already up to date (v%1)").arg(current));
    });
    QObject::connect(m_updater, &UpdateChecker::failed, this, [this](const QString &reason) {
        setUpdateCheckBusy(false);
        // 直连失败时最常见的原因是 GitHub 不可达：把「怎么办」一并说清，
        // 否则用户只会看到一句 Connection refused
        const QString hint = m_context->isKernelRunning()
                                 ? QString()
                                 : tr(" (the kernel is not running; start the proxy and try again)");
        emit m_context->toast(QStringLiteral("error"),
                              tr("Update check failed: %1%2").arg(reason, hint));
    });

    loadFromConfig();
}

void SettingsPage::loadFromConfig()
{
    m_loading = true;

    AppConfig             *app      = m_context->appConfig();
    ControledKernelConfig *controled = m_context->controledConfig();

    m_logLevelCombo->setCurrentText(controled->logLevel());
    m_maxLogDaysSpin->setValue(app->maxLogDays());

    m_controlDns->setChecked(app->controlDns());
    m_controlSniff->setChecked(app->controlSniff());

    m_mixedPort->setValue(controled->mixedPort());
    m_socksPort->setValue(controled->socksPort());
    m_httpPort->setValue(controled->httpPort());

    m_autoStart->setChecked(m_context->autoRun()->isEnabled());
    m_sysProxy->setChecked(m_context->isSysProxyEnabled());
    m_tunEnabled->setChecked(m_context->tunEnabled());
    m_tunAutoRoute->setChecked(m_context->tunAutoRoute());

    const QString theme = app->appTheme();
    const int themeIndex = m_themeCombo->findData(theme);
    m_themeCombo->setCurrentIndex(themeIndex >= 0 ? themeIndex : 0);

    int languageIndex = m_languageCombo->findData(app->language());
    // 旧配置里可能是本版本还没支持的语言（例如 zh-TW），回落到中文而不是
    // 悄悄显示成「跟随系统」——用户至少能看出自己选过什么。
    if (languageIndex < 0) languageIndex = m_languageCombo->findData(QStringLiteral("zh-CN"));
    m_languageCombo->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);

    m_dataDirLabel->setText(tr("Data folder: %1").arg(paths::dataDir()));

    m_loading = false;
}

void SettingsPage::onLogLevelChanged(const QString &level)
{
    if (m_loading) return;

    m_context->controledConfig()->patch(QJsonObject{{QStringLiteral("log-level"), level}});
    // 只改应用侧的日志器，不重载内核：运行期没有任何地方读运行配置里的
    // log-level（内核日志统一经 AppLogger 落盘），重载反而要重建监听、把
    // 所有在途连接掐断。
    AppLogger::instance().setLevel(
        core::parseLogLevel(level).value_or(core::LogLevel::Info));
}

void SettingsPage::onMaxLogDaysChanged(int days)
{
    if (m_loading) return;
    m_context->appConfig()->patch(QJsonObject{{QStringLiteral("maxLogDays"), days}});
    AppLogger::instance().setMaxLogDays(days);
    AppLogger::instance().purgeOldLogs();
}

void SettingsPage::onControlDnsToggled(bool enabled)
{
    if (m_loading) return;

    // controlDns 决定生成配置时是否注入受控 DNS 段，
    // 必须同时更新应用配置与内核运行状态，否则界面与实际行为不一致。
    m_context->appConfig()->setControlDns(enabled);
    m_context->appConfig()->save();
    m_context->hub()->setDnsEnabled(enabled);
    m_context->reloadKernel();
}

void SettingsPage::onControlSniffToggled(bool enabled)
{
    if (m_loading) return;
    m_context->appConfig()->setControlSniff(enabled);
    m_context->appConfig()->save();
    m_context->hub()->setSnifferEnabled(enabled);
    m_context->reloadKernel();
}

void SettingsPage::onPortChanged(int)
{
    if (m_loading) return;

    m_context->controledConfig()->patch(
        QJsonObject{{QStringLiteral("mixed-port"), m_mixedPort->value()},
                    {QStringLiteral("socks-port"), m_socksPort->value()},
                    {QStringLiteral("port"), m_httpPort->value()}});

    QString error;
    if (!m_context->reloadKernel(&error)) {
        m_context->showToast(QStringLiteral("error"),
                             tr("Reload failed after port change: %1").arg(error));
    }
}

void SettingsPage::onAutoStartToggled(bool enabled)
{
    if (m_loading) return;

    QString error;
    bool    ok = enabled ? m_context->autoRun()->enable(/*silent=*/true, &error)
                         : m_context->autoRun()->disable(&error);
    if (!ok) {
        m_context->showToast(QStringLiteral("error"),
                             tr("Failed to change startup setting: %1").arg(error));
        m_loading = true;
        m_autoStart->setChecked(m_context->autoRun()->isEnabled());
        m_loading = false;
    }
}

void SettingsPage::onTunToggled(bool enabled)
{
    if (m_loading) return;

    QString error;
    if (!m_context->setTunEnabled(enabled, &error)) {
        // 失败（未授权 / 端口冲突 / 无内核）：回滚开关并说明原因
        m_loading = true;
        m_tunEnabled->setChecked(m_context->tunEnabled());
        m_loading = false;
        if (!error.isEmpty()) {
            emit m_context->toast(QStringLiteral("error"), error);
        }
        return;
    }

    m_loading = true;
    if (m_context->tunStarting()) {
        // 启用是异步的（拉起辅助程序 → 等 fd → 建数据面）：先保持勾选并临时
        // 禁用，等 tunStateChanged 回来再解禁，避免开关"点下就弹回"的错觉
        m_tunEnabled->setChecked(true);
        m_tunEnabled->setEnabled(false);
    } else {
        m_tunEnabled->setChecked(m_context->tunEnabled());
    }
    m_loading = false;
}

void SettingsPage::onTunAutoRouteToggled(bool enabled)
{
    if (m_loading) return;

    QString error;
    if (!m_context->setTunAutoRoute(enabled, &error)) {
        m_loading = true;
        m_tunAutoRoute->setChecked(m_context->tunAutoRoute());
        m_loading = false;
        if (!error.isEmpty()) emit m_context->toast(QStringLiteral("error"), error);
        return;
    }
    m_loading = true;
    m_tunAutoRoute->setChecked(m_context->tunAutoRoute());
    m_loading = false;
}

void SettingsPage::onSysProxyToggled(bool enabled)
{
    if (m_loading) return;

    QString error;
    if (!m_context->setSysProxyEnabled(enabled, &error)) {
        m_loading = true;
        m_sysProxy->setChecked(m_context->isSysProxyEnabled());
    m_tunEnabled->setChecked(m_context->tunEnabled());
    m_tunAutoRoute->setChecked(m_context->tunAutoRoute());
        m_loading = false;
    }
}

void SettingsPage::onThemeChanged(int index)
{
    if (m_loading) return;
    const QString theme = m_themeCombo->itemData(index).toString();
    m_context->appConfig()->patch(QJsonObject{{QStringLiteral("appTheme"), theme}});
    // 按主题名解析（system → 跟随桌面配色方案）
    ui::applyThemeFor(theme);
}

void SettingsPage::onLanguageChanged(int index)
{
    if (m_loading) return;
    const QString language = m_languageCombo->itemData(index).toString();
    m_context->appConfig()->patch(QJsonObject{{QStringLiteral("language"), language}});

    // 界面文案在控件构造时就已经取走，运行期没有可靠的换语言手段；
    // 这里只落配置并明确告知需要重启，不做假的「立即生效」。
    m_context->showToast(QStringLiteral("info"),
                         tr("Language saved; restart the app to apply."));
}

void SettingsPage::onOpenDataDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(paths::dataDir()));
}

void SettingsPage::onCheckUpdate()
{
    if (!m_updater || !m_checkUpdate->isEnabled()) return;

    // 检查走本机混合端口：GitHub 直连在多数网络下不可达，而内核常驻运行，
    // 混合端口既能代理 HTTP 也能 CONNECT，正好复用用户的节点。
    const int port = m_context->isKernelRunning() ? m_context->controledConfig()->mixedPort() : 0;
    setUpdateCheckBusy(true);
    if (!m_updater->check(port)) {
        setUpdateCheckBusy(false); // 已有请求在飞（重复点击）
    }
}

void SettingsPage::setUpdateCheckBusy(bool busy)
{
    if (!m_checkUpdate) return;
    m_checkUpdate->setEnabled(!busy);
    m_checkUpdate->setText(busy ? tr("Checking…") : tr("Check for updates"));
}

} // namespace clashqt
