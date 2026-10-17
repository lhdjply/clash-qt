#include "ui/pages/SettingsPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "sys/AutoRun.h"
#include "sys/SysProxy.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"

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
        QStringLiteral("设置"),
        QStringLiteral("日志、DNS 覆写、端口与系统集成。修改后会自动保存并即时生效。")));

    // ---- 日志 -------------------------------------------------------------
    auto *logCard = ui::makeCard(QStringLiteral("日志"), this);
    auto *logLayout = qobject_cast<QVBoxLayout *>(logCard->layout());

    m_logLevelCombo = new QComboBox(logCard);
    for (const QString &level : core::logLevelNames()) m_logLevelCombo->addItem(level);
    logLayout->addWidget(ui::makeFormRow(QStringLiteral("日志等级"), m_logLevelCombo,
                                         QString(), logCard));

    m_maxLogDaysSpin = new QSpinBox(logCard);
    m_maxLogDaysSpin->setRange(1, 365);
    m_maxLogDaysSpin->setSuffix(QStringLiteral(" 天"));
    // 滚轮不参与改值，避免鼠标划过时误改（滚轮留给页面滚动）
    ui::ignoreWheel(m_maxLogDaysSpin);
    logLayout->addWidget(ui::makeFormRow(QStringLiteral("日志保留天数"), m_maxLogDaysSpin,
                                         QStringLiteral("超过保留天数的日志文件会在启动时清理。"),
                                         logCard));

    auto *openDir = new QPushButton(QStringLiteral("打开数据目录"), logCard);
    ui::decorate(openDir, QStringLiteral("folder"));
    logLayout->addWidget(openDir);

    m_dataDirLabel = ui::makeHintLabel(QString(), logCard);
    logLayout->addWidget(m_dataDirLabel);
    root->addWidget(logCard);

    // ---- 配置覆写 ---------------------------------------------------------
    auto *controlCard = ui::makeCard(QStringLiteral("配置覆写"), this);
    auto *controlLayout = qobject_cast<QVBoxLayout *>(controlCard->layout());

    m_controlDns = new QCheckBox(QStringLiteral("由本应用管理 DNS 配置"), controlCard);
    controlLayout->addWidget(m_controlDns);
    controlLayout->addWidget(ui::makeHintLabel(
        QStringLiteral("开启后使用本应用设置的 DNS 段；关闭则沿用订阅自带的 DNS 配置。"),
        controlCard));

    m_controlSniff = new QCheckBox(QStringLiteral("由本应用管理域名嗅探"), controlCard);
    controlLayout->addWidget(m_controlSniff);

    root->addWidget(controlCard);

    // ---- 端口 -------------------------------------------------------------
    auto *portCard = ui::makeCard(QStringLiteral("入站端口"), this);
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

    portLayout->addWidget(ui::makeFormRow(QStringLiteral("混合端口（HTTP + SOCKS5）"),
                                          m_mixedPort, QString(), portCard));
    portLayout->addWidget(ui::makeFormRow(QStringLiteral("SOCKS5 端口"), m_socksPort,
                                         QStringLiteral("设为与混合端口相同则复用混合端口。"),
                                         portCard));
    portLayout->addWidget(ui::makeFormRow(QStringLiteral("HTTP 端口"), m_httpPort, QString(),
                                         portCard));
    root->addWidget(portCard);

    // ---- 系统集成 ---------------------------------------------------------
    auto *systemCard = ui::makeCard(QStringLiteral("系统集成"), this);
    auto *systemLayout = qobject_cast<QVBoxLayout *>(systemCard->layout());

    m_autoStart = new QCheckBox(QStringLiteral("开机自启动"), systemCard);
    systemLayout->addWidget(m_autoStart);
    systemLayout->addWidget(ui::makeHintLabel(QStringLiteral("通过 XDG 自启动项实现"
                                                             "（~/.config/autostart/clash-qt.desktop）。"),
                                             systemCard));

    m_sysProxy = new QCheckBox(QStringLiteral("系统代理"), systemCard);
    systemLayout->addWidget(m_sysProxy);
    systemLayout->addWidget(ui::makeHintLabel(
        QStringLiteral("将系统 HTTP/HTTPS/SOCKS 代理指向本应用的混合端口。"
                       "当前后端：%1").arg(m_context->sysProxy()->backendName()),
        systemCard));

    // ---- TUN 模式 --------------------------------------------------------
    m_tunEnabled = new QCheckBox(QStringLiteral("TUN 模式"), systemCard);
    systemLayout->addWidget(m_tunEnabled);
    systemLayout->addWidget(ui::makeHintLabel(
        QStringLiteral("创建虚拟网卡接管流量，连不认系统代理的程序（游戏 / 命令行 / 部分 Electron）"
                       "也能走代理。首次开启会弹系统密码框授权一次；"
                       "必须与「接管域名流量」配合才真正生效。"),
        systemCard));

    m_tunAutoRoute = new QCheckBox(QStringLiteral("接管域名流量（fake-ip 路由）"), systemCard);
    systemLayout->addWidget(m_tunAutoRoute);
    systemLayout->addWidget(ui::makeHintLabel(
        QStringLiteral("把假 IP 网段的路由指向 TUN（关闭 TUN 时随接口一起消失）。"
                       "不改动系统默认路由，因此不会影响网络连通性；"
                       "关掉这一项，TUN 将收不到任何流量。"),
        systemCard));

    m_themeCombo = new QComboBox(systemCard);
    m_themeCombo->addItem(QStringLiteral("跟随系统"), QStringLiteral("system"));
    m_themeCombo->addItem(QStringLiteral("浅色"), QStringLiteral("light"));
    m_themeCombo->addItem(QStringLiteral("深色"), QStringLiteral("dark"));
    systemLayout->addWidget(ui::makeFormRow(QStringLiteral("主题"), m_themeCombo, QString(),
                                           systemCard));
    root->addWidget(systemCard);

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
    QObject::connect(openDir, &QPushButton::clicked, this, &SettingsPage::onOpenDataDir);

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

    m_dataDirLabel->setText(QStringLiteral("数据目录：%1").arg(paths::dataDir()));

    m_loading = false;
}

void SettingsPage::onLogLevelChanged(const QString &level)
{
    if (m_loading) return;

    m_context->controledConfig()->patch(QJsonObject{{QStringLiteral("log-level"), level}});
    AppLogger::instance().setLevel(
        core::parseLogLevel(level).value_or(core::LogLevel::Info));
    m_context->reloadKernel();
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
                             QStringLiteral("端口修改后重载失败：%1").arg(error));
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
                             QStringLiteral("自启动设置失败：%1").arg(error));
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

void SettingsPage::onOpenDataDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(paths::dataDir()));
}

} // namespace clashqt
