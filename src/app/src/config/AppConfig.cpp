#include "config/AppConfig.h"

#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QJsonArray>

#include "common/JsonUtil.h"
#include "common/Yaml.h"

namespace clashqt {
namespace {

// 读取布尔值并带默认值，容忍 YAML 中的字符串字面量
bool readBool(const QJsonObject &object, const QString &key, bool fallback)
{
    return core::jsonutil::boolValue(object, key, fallback);
}

int readInt(const QJsonObject &object, const QString &key, int fallback)
{
    return core::jsonutil::intValue(object, key, fallback);
}

QString readString(const QJsonObject &object, const QString &key, const QString &fallback = {})
{
    return core::jsonutil::stringValue(object, key, fallback);
}

} // namespace

AppConfig::AppConfig(QObject *parent) : QObject(parent) {}

QJsonObject AppConfig::defaults()
{
    // 与 clash-party 的 defaultConfig（src/main/utils/template.ts）逐项对齐。
    // 保持取值一致有两层意义：一是用户从旧版迁移时体验不变；
    // 二是内核生成配置时的默认端口、默认 DNS 行为完全一致。
    QJsonObject config;

    // 运行模式（内核固定为项目自带实现，已无内核选择项）
    config.insert(QStringLiteral("modeSelected"), false);
    config.insert(QStringLiteral("operationMode"), QStringLiteral("standard"));
    config.insert(QStringLiteral("enableSmartOverride"), true);

    // 启动与更新
    config.insert(QStringLiteral("silentStart"), false);
    config.insert(QStringLiteral("autoCheckUpdate"), true);
    config.insert(QStringLiteral("autoUpdateProfileOnStart"), true);
    config.insert(QStringLiteral("silentUpdate"), true);
    config.insert(QStringLiteral("autoCloseConnection"), true);
    config.insert(QStringLiteral("autoCloseConnectionOnlyGroup"), false);
    config.insert(QStringLiteral("subscriptionTimeout"), 30000);
    config.insert(QStringLiteral("testProfileOnStart"), true);
    config.insert(QStringLiteral("useHotReloadProfile"), false);
    config.insert(QStringLiteral("coreStartupMode"), QStringLiteral("log"));

    // 界面
    config.insert(QStringLiteral("appTheme"), QStringLiteral("system"));
    config.insert(QStringLiteral("customTheme"), QString());
    config.insert(QStringLiteral("useWindowFrame"), false);
    config.insert(QStringLiteral("language"), QStringLiteral("zh-CN"));
    config.insert(QStringLiteral("siderWidth"), 250);
    config.insert(QStringLiteral("lastSelectedSiderCard"), QStringLiteral("proxy"));
    config.insert(QStringLiteral("rememberSelectedSiderCard"), true);
    config.insert(QStringLiteral("lockSiderCards"), false);
    config.insert(QStringLiteral("disableAnimations"), false);
    config.insert(QStringLiteral("proxyCols"), QStringLiteral("auto"));
    config.insert(QStringLiteral("proxyDisplayMode"), QStringLiteral("simple"));
    config.insert(QStringLiteral("proxyDisplayOrder"), QStringLiteral("default"));
    config.insert(QStringLiteral("hideUnavailableProxies"), false);

    // 侧栏卡片顺序（对应 DEFAULT_SIDER_ORDER）
    config.insert(QStringLiteral("siderOrder"),
                  QJsonArray::fromStringList(
                      {QStringLiteral("sysproxy"), QStringLiteral("tun"),
                       QStringLiteral("profile"), QStringLiteral("proxy"),
                       QStringLiteral("rule"), QStringLiteral("resource"),
                       QStringLiteral("override"), QStringLiteral("connection"),
                       QStringLiteral("kernel"), QStringLiteral("dns"), QStringLiteral("sniff"),
                       QStringLiteral("log"), QStringLiteral("substore"),
                       QStringLiteral("network"), QStringLiteral("usage")}));

    // 托盘
    config.insert(QStringLiteral("disableTray"), false);
    config.insert(QStringLiteral("proxyInTray"), true);
    config.insert(QStringLiteral("showCurrentProxyInTray"), false);
    config.insert(QStringLiteral("trayProxyGroupStyle"), QStringLiteral("default"));
    config.insert(QStringLiteral("disableTrayIconColor"), false);
    config.insert(QStringLiteral("trayTrafficTextColor"), QStringLiteral("auto"));
    config.insert(QStringLiteral("showTraffic"), false);
    config.insert(QStringLiteral("customTrayIcon"), QString());
    config.insert(QStringLiteral("customTrayIcons"), QJsonObject());
    config.insert(QStringLiteral("swapTrayClick"), false);
    config.insert(QStringLiteral("triggerMainWindowBehavior"), QStringLiteral("show"));

    // 悬浮窗
    config.insert(QStringLiteral("showFloatingWindow"), false);
    config.insert(QStringLiteral("floatingWindowCompatMode"), true);
    config.insert(QStringLiteral("spinFloatingIcon"), false);

    // 日志
    config.insert(QStringLiteral("maxLogDays"), 7);
    config.insert(QStringLiteral("maxLogFileSize"), 10);
    config.insert(QStringLiteral("disableAppLog"), false);
    config.insert(QStringLiteral("disableCoreLog"), false);

    // 内核受控开关
    config.insert(QStringLiteral("controlDns"), false);
    config.insert(QStringLiteral("controlSniff"), true);
    config.insert(QStringLiteral("disableDnsOverrideGuard"), false);
    config.insert(QStringLiteral("useNameserverPolicy"), false);
    config.insert(QStringLiteral("nameserverPolicy"), QJsonObject());
    config.insert(QStringLiteral("diffWorkDir"), false);
    config.insert(QStringLiteral("autoSetDNS"), false);
    config.insert(QStringLiteral("originDNS"), QString());
    config.insert(QStringLiteral("enableTrafficLogger"), true);
    config.insert(QStringLiteral("kernelCpuPriority"), QStringLiteral("PRIORITY_NORMAL"));

    // 端口（默认值对应 DEFAULT_MIHOMO_PORTS）
    config.insert(QStringLiteral("showMixedPort"), 7890);
    config.insert(QStringLiteral("enableMixedPort"), true);
    config.insert(QStringLiteral("showSocksPort"), 7891);
    config.insert(QStringLiteral("enableSocksPort"), true);
    config.insert(QStringLiteral("showHttpPort"), 7892);
    config.insert(QStringLiteral("enableHttpPort"), true);
    config.insert(QStringLiteral("showRedirPort"), 0);
    config.insert(QStringLiteral("enableRedirPort"), false);
    config.insert(QStringLiteral("showTproxyPort"), 0);
    config.insert(QStringLiteral("enableTproxyPort"), false);

    // 系统代理
    QJsonObject sysProxy;
    sysProxy.insert(QStringLiteral("enable"), false);
    sysProxy.insert(QStringLiteral("mode"), QStringLiteral("manual"));
    sysProxy.insert(QStringLiteral("host"), QString());
    sysProxy.insert(QStringLiteral("bypass"), QJsonArray());
    config.insert(QStringLiteral("sysProxy"), sysProxy);

    // 各代理组的手动选择（由界面写入，用于配置重载/重启后恢复）
    config.insert(QStringLiteral("selectedProxies"), QJsonObject());

    // 代理页节点排序（界面状态：default / name / type / delay）
    config.insert(QStringLiteral("proxySort"), QStringLiteral("default"));

    // 延迟测试
    config.insert(QStringLiteral("delayTestUrl"),
                  QStringLiteral("https://www.gstatic.com/generate_204"));
    config.insert(QStringLiteral("delayTestTimeout"), 5000);
    config.insert(QStringLiteral("delayTestConcurrency"), 8);

    // 自动退出
    config.insert(QStringLiteral("autoQuitWithoutCore"), false);
    config.insert(QStringLiteral("autoQuitWithoutCoreDelay"), 60);
    config.insert(QStringLiteral("autoQuitWithoutCoreMode"), QStringLiteral("core"));

    // 连接列表排序
    config.insert(QStringLiteral("connectionDirection"), QStringLiteral("asc"));
    config.insert(QStringLiteral("connectionOrderBy"), QStringLiteral("time"));
    config.insert(QStringLiteral("connectionViewMode"), QStringLiteral("list"));
    config.insert(QStringLiteral("displayIcon"), true);
    config.insert(QStringLiteral("displayAppName"), true);

    // Sub-Store
    config.insert(QStringLiteral("useSubStore"), true);
    config.insert(QStringLiteral("subStoreHost"), QString());
    config.insert(QStringLiteral("useCustomSubStore"), false);
    config.insert(QStringLiteral("useProxyInSubStore"), false);
    config.insert(QStringLiteral("customSubStoreUrl"), QString());

    // 备份
    config.insert(QStringLiteral("webdavUrl"), QString());
    config.insert(QStringLiteral("webdavDir"), QString());
    config.insert(QStringLiteral("webdavUsername"), QString());
    config.insert(QStringLiteral("webdavPassword"), QString());
    config.insert(QStringLiteral("webdavMaxBackups"), 5);
    config.insert(QStringLiteral("webdavBackupCron"), QString());
    config.insert(QStringLiteral("webdavIgnoreCert"), false);

    // 订阅 User-Agent：机场普遍按 UA 识别客户端并决定返回哪种配置格式，
    // 必须携带 clash.meta 标识，否则会被判定为「不支持的代理软件」而降级返回。
    config.insert(QStringLiteral("userAgent"), QString());

    // Gist
    config.insert(QStringLiteral("githubToken"), QString());
    config.insert(QStringLiteral("gistAgeEncrypt"), false);
    config.insert(QStringLiteral("gistAgeRecipient"), QString());
    config.insert(QStringLiteral("gistAgeSecretKey"), QString());

    return config;
}

bool AppConfig::load()
{
    bool ok = false;
    const QString content = safefile::readText(paths::appConfigPath(), &ok);

    QJsonObject parsed;
    if (ok && !content.trimmed().isEmpty()) {
        QString                 error;
        const auto              value = core::Yaml::parse(content, &error);
        if (value && value->isObject()) {
            parsed = value->toObject();
        } else if (!error.isEmpty()) {
            AppLogger::instance().error(
                QStringLiteral("应用配置解析失败，已使用默认配置：%1").arg(error));
        }
    }

    // 与默认值深合并：新增字段获得默认值，用户已有设置保持不变
    m_object = core::jsonutil::deepMerge(defaults(), parsed);

    // 旧版本配置（无 modeSelected）沿用标准模式，只对全新配置保留首次选择门控
    if (!parsed.contains(QStringLiteral("modeSelected"))) {
        m_object.insert(QStringLiteral("modeSelected"), true);
        m_object.insert(QStringLiteral("operationMode"), QStringLiteral("standard"));
    }

    m_loaded = true;

    // 首次运行或字段补齐后回写一次，保证磁盘上的配置文件结构完整
    if (!ok || m_object != parsed) {
        QString error;
        if (!safefile::writeAtomic(paths::appConfigPath(), core::Yaml::stringify(m_object),
                                   &error)) {
            AppLogger::instance().warn(QStringLiteral("应用配置回写失败：%1").arg(error));
        }
    }
    return true;
}

bool AppConfig::save()
{
    QString error;
    if (!safefile::writeAtomic(paths::appConfigPath(), core::Yaml::stringify(m_object), &error)) {
        AppLogger::instance().error(QStringLiteral("应用配置保存失败：%1").arg(error));
        return false;
    }
    return true;
}

void AppConfig::setKernelSection(const QJsonObject &section, bool persist)
{
    m_object.insert(QStringLiteral("kernel"), section);
    if (persist) save();
}

void AppConfig::patch(const QJsonObject &patch, bool persist)
{
    // nameserverPolicy 需要整体替换语义：清空后不加回会残留旧键
    const bool replacePolicy = patch.contains(QStringLiteral("nameserverPolicy"));

    m_object = core::jsonutil::deepMerge(m_object, patch);
    if (replacePolicy) {
        m_object.insert(QStringLiteral("nameserverPolicy"),
                        patch.value(QStringLiteral("nameserverPolicy")));
    }

    if (persist) save();
    emit configChanged();
}

// ---- 类型化访问 -----------------------------------------------------------

bool AppConfig::enableSmartOverride() const { return readBool(m_object, QStringLiteral("enableSmartOverride"), true); }

bool AppConfig::silentStart() const { return readBool(m_object, QStringLiteral("silentStart"), false); }
bool AppConfig::autoCheckUpdate() const { return readBool(m_object, QStringLiteral("autoCheckUpdate"), true); }
bool AppConfig::autoUpdateProfileOnStart() const
{
    return readBool(m_object, QStringLiteral("autoUpdateProfileOnStart"), true);
}

QString AppConfig::appTheme() const { return readString(m_object, QStringLiteral("appTheme"), QStringLiteral("system")); }
QString AppConfig::language() const { return readString(m_object, QStringLiteral("language"), QStringLiteral("zh-CN")); }

bool AppConfig::disableTray() const { return readBool(m_object, QStringLiteral("disableTray"), false); }
bool AppConfig::proxyInTray() const { return readBool(m_object, QStringLiteral("proxyInTray"), true); }
bool AppConfig::showCurrentProxyInTray() const
{
    return readBool(m_object, QStringLiteral("showCurrentProxyInTray"), false);
}
QString AppConfig::trayProxyGroupStyle() const
{
    return readString(m_object, QStringLiteral("trayProxyGroupStyle"), QStringLiteral("default"));
}

int AppConfig::maxLogDays() const { return readInt(m_object, QStringLiteral("maxLogDays"), 7); }
int AppConfig::maxLogFileSizeMB() const { return readInt(m_object, QStringLiteral("maxLogFileSize"), 10); }
bool AppConfig::disableAppLog() const { return readBool(m_object, QStringLiteral("disableAppLog"), false); }
bool AppConfig::disableCoreLog() const { return readBool(m_object, QStringLiteral("disableCoreLog"), false); }
QString AppConfig::logLevel() const
{
    return readString(m_object, QStringLiteral("logLevel"), QStringLiteral("info"));
}

bool AppConfig::controlDns() const { return readBool(m_object, QStringLiteral("controlDns"), false); }
void AppConfig::setControlDns(bool enabled) { m_object.insert(QStringLiteral("controlDns"), enabled); }
bool AppConfig::controlSniff() const { return readBool(m_object, QStringLiteral("controlSniff"), true); }
void AppConfig::setControlSniff(bool enabled) { m_object.insert(QStringLiteral("controlSniff"), enabled); }
bool AppConfig::useNameserverPolicy() const
{
    return readBool(m_object, QStringLiteral("useNameserverPolicy"), false);
}

int AppConfig::showMixedPort() const { return readInt(m_object, QStringLiteral("showMixedPort"), 7890); }
int AppConfig::showSocksPort() const { return readInt(m_object, QStringLiteral("showSocksPort"), 7891); }
int AppConfig::showHttpPort() const { return readInt(m_object, QStringLiteral("showHttpPort"), 7892); }
bool AppConfig::enableMixedPort() const { return readBool(m_object, QStringLiteral("enableMixedPort"), true); }
bool AppConfig::enableSocksPort() const { return readBool(m_object, QStringLiteral("enableSocksPort"), true); }
bool AppConfig::enableHttpPort() const { return readBool(m_object, QStringLiteral("enableHttpPort"), true); }

QJsonObject AppConfig::sysProxy() const
{
    return m_object.value(QStringLiteral("sysProxy")).toObject();
}

bool AppConfig::sysProxyEnabled() const
{
    return readBool(sysProxy(), QStringLiteral("enable"), false);
}

void AppConfig::setSysProxyEnabled(bool enabled)
{
    QJsonObject proxy = sysProxy();
    proxy.insert(QStringLiteral("enable"), enabled);
    m_object.insert(QStringLiteral("sysProxy"), proxy);
}

QString AppConfig::sysProxyMode() const
{
    return readString(sysProxy(), QStringLiteral("mode"), QStringLiteral("manual"));
}

QStringList AppConfig::sysProxyBypass() const
{
    return core::jsonutil::stringArrayValue(sysProxy(), QStringLiteral("bypass"));
}

QString AppConfig::mode() const { return readString(m_object, QStringLiteral("mode"), QStringLiteral("rule")); }
void AppConfig::setMode(const QString &mode) { m_object.insert(QStringLiteral("mode"), mode); }

QString AppConfig::lastSelectedSiderCard() const
{
    return readString(m_object, QStringLiteral("lastSelectedSiderCard"), QStringLiteral("proxy"));
}

void AppConfig::setLastSelectedSiderCard(const QString &key)
{
    m_object.insert(QStringLiteral("lastSelectedSiderCard"), key);
}

bool AppConfig::rememberSelectedSiderCard() const
{
    return readBool(m_object, QStringLiteral("rememberSelectedSiderCard"), true);
}

int AppConfig::siderWidth() const { return readInt(m_object, QStringLiteral("siderWidth"), 250); }
void AppConfig::setSiderWidth(int width) { m_object.insert(QStringLiteral("siderWidth"), width); }

bool AppConfig::diffWorkDir() const { return readBool(m_object, QStringLiteral("diffWorkDir"), false); }
bool AppConfig::autoSetDNS() const { return readBool(m_object, QStringLiteral("autoSetDNS"), false); }
QString AppConfig::originDNS() const { return readString(m_object, QStringLiteral("originDNS")); }

QString AppConfig::userAgent() const
{
    return core::jsonutil::stringValue(m_object, QStringLiteral("userAgent"));
}

QString AppConfig::delayTestUrl() const
{
    return readString(m_object, QStringLiteral("delayTestUrl"),
                      QStringLiteral("https://www.gstatic.com/generate_204"));
}

QJsonObject AppConfig::selectedProxies() const
{
    return m_object.value(QStringLiteral("selectedProxies")).toObject();
}

void AppConfig::setSelectedProxies(const QJsonObject &selections)
{
    // 整体替换：没有用户显式选择的组必须从配置里消失，
    // 而 patch() 的深合并语义删不掉键。
    m_object.insert(QStringLiteral("selectedProxies"), selections);
    save();
}

QString AppConfig::proxySort() const
{
    return readString(m_object, QStringLiteral("proxySort"), QStringLiteral("default"));
}

void AppConfig::setProxySort(const QString &sort)
{
    m_object.insert(QStringLiteral("proxySort"), sort);
}

int AppConfig::delayTestTimeout() const { return readInt(m_object, QStringLiteral("delayTestTimeout"), 5000); }
int AppConfig::delayTestConcurrency() const
{
    return readInt(m_object, QStringLiteral("delayTestConcurrency"), 8);
}

} // namespace clashqt
