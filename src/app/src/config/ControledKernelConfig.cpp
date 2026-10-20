#include "config/ControledKernelConfig.h"
#include <QCoreApplication>

#include "config/AppConfig.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QFile>
#include <QJsonArray>

#include "common/JsonUtil.h"
#include "common/Yaml.h"

namespace clashqt {
namespace {

// TUN 设备名：macOS 需要 utunNNNN，其余平台用本程序的名字
QString defaultTunDevice()
{
#ifdef Q_OS_MACOS
    return QStringLiteral("utun1500");
#else
    // 接口名由 CMake 按软件名派生（<app>-tun，并截断到内核允许的 15 字符），
    // 不再沿用参照实现的 Mihomo
    return QStringLiteral(CLASHQT_TUN_DEVICE);
#endif
}

QJsonObject defaultTun()
{
    QJsonObject tun;
    tun.insert(QStringLiteral("enable"), false);
    tun.insert(QStringLiteral("stack"), QStringLiteral("mixed"));
    tun.insert(QStringLiteral("device"), defaultTunDevice());
    tun.insert(QStringLiteral("auto-route"), true);
    tun.insert(QStringLiteral("auto-redirect"), false);
    tun.insert(QStringLiteral("auto-detect-interface"), true);
    tun.insert(QStringLiteral("dns-hijack"), QJsonArray::fromStringList({QStringLiteral("any:53")}));
    tun.insert(QStringLiteral("route-exclude-address"), QJsonArray());
    tun.insert(QStringLiteral("mtu"), 1500);
    return tun;
}

QJsonObject defaultDns()
{
    QJsonObject dns;
    dns.insert(QStringLiteral("enable"), true);
    dns.insert(QStringLiteral("listen"), QString());
    dns.insert(QStringLiteral("ipv6"), false);
    dns.insert(QStringLiteral("enhanced-mode"), QStringLiteral("fake-ip"));
    dns.insert(QStringLiteral("fake-ip-range"), QStringLiteral("198.18.0.1/16"));
    // 注意：这里**不能**放通配 "*" —— 它会命中所有域名，等于让 fake-ip 完全失效
    // （没有域名 → 域名规则失效；系统 DNS 的真实 AAAA 会让应用走 IPv6，
    //   而 TUN 层不支持 IPv6，表现为双栈站点直接打不开）
    dns.insert(QStringLiteral("fake-ip-filter"),
               QJsonArray::fromStringList({QStringLiteral("+.lan"),
                                           QStringLiteral("+.local"),
                                           QStringLiteral("time.*.com"),
                                           QStringLiteral("ntp.*.com"),
                                           QStringLiteral("+.market.xiaomi.com")}));
    dns.insert(QStringLiteral("use-hosts"), false);
    dns.insert(QStringLiteral("use-system-hosts"), false);
    dns.insert(QStringLiteral("respect-rules"), false);
    dns.insert(QStringLiteral("default-nameserver"),
               QJsonArray::fromStringList({QStringLiteral("tls://223.5.5.5")}));

    // 注意：DoT（tls://）在当前 C++ 内核中按明文 TCP 回退处理，
    // 默认值因此改用 DoH，避免用户在不了解限制的情况下依赖 DoT。
    dns.insert(QStringLiteral("nameserver"),
               QJsonArray::fromStringList({QStringLiteral("https://doh.pub/dns-query"),
                                           QStringLiteral("https://dns.alidns.com/dns-query")}));
    dns.insert(QStringLiteral("proxy-server-nameserver"),
               QJsonArray::fromStringList({QStringLiteral("https://doh.pub/dns-query"),
                                           QStringLiteral("https://dns.alidns.com/dns-query")}));
    dns.insert(QStringLiteral("direct-nameserver"), QJsonArray());
    dns.insert(QStringLiteral("fallback"), QJsonArray());

    QJsonObject fallbackFilter;
    fallbackFilter.insert(QStringLiteral("geoip"), true);
    fallbackFilter.insert(QStringLiteral("geoip-code"), QStringLiteral("CN"));
    fallbackFilter.insert(QStringLiteral("ipcidr"),
                          QJsonArray::fromStringList({QStringLiteral("240.0.0.0/4"),
                                                      QStringLiteral("0.0.0.0/32")}));
    fallbackFilter.insert(QStringLiteral("domain"),
                          QJsonArray::fromStringList({QStringLiteral("+.google.com"),
                                                      QStringLiteral("+.facebook.com"),
                                                      QStringLiteral("+.youtube.com")}));
    dns.insert(QStringLiteral("fallback-filter"), fallbackFilter);
    return dns;
}

// 已废弃的受控配置键（含外部下载地址，且本内核无对应实现）
const QStringList &retiredKeys()
{
    static const QStringList keys{
        QStringLiteral("external-ui-url"),   QStringLiteral("geo-auto-update"),
        QStringLiteral("geo-update-interval"), QStringLiteral("geox-url"),
        QStringLiteral("geodata-mode"),      QStringLiteral("lgbm-auto-update"),
        QStringLiteral("lgbm-update-interval"), QStringLiteral("lgbm-url")};
    return keys;
}

QJsonObject defaultSniffer()
{
    QJsonObject sniffer;
    sniffer.insert(QStringLiteral("enable"), true);
    sniffer.insert(QStringLiteral("parse-pure-ip"), true);
    sniffer.insert(QStringLiteral("force-dns-mapping"), true);
    sniffer.insert(QStringLiteral("override-destination"), false);

    QJsonObject http;
    http.insert(QStringLiteral("ports"), QJsonArray::fromVariantList({80, 443}));
    http.insert(QStringLiteral("override-destination"), false);
    QJsonObject tls;
    tls.insert(QStringLiteral("ports"), QJsonArray::fromVariantList({443}));

    QJsonObject sniff;
    sniff.insert(QStringLiteral("HTTP"), http);
    sniff.insert(QStringLiteral("TLS"), tls);
    sniffer.insert(QStringLiteral("sniff"), sniff);

    sniffer.insert(QStringLiteral("skip-domain"),
                   QJsonArray::fromStringList({QStringLiteral("+.push.apple.com")}));
    sniffer.insert(QStringLiteral("skip-dst-address"),
                   QJsonArray::fromStringList(
                       {QStringLiteral("91.105.192.0/23"), QStringLiteral("91.108.4.0/22"),
                        QStringLiteral("91.108.8.0/21"), QStringLiteral("91.108.16.0/21"),
                        QStringLiteral("91.108.56.0/22"), QStringLiteral("95.161.64.0/20"),
                        QStringLiteral("149.154.160.0/20"), QStringLiteral("185.76.151.0/24"),
                        QStringLiteral("2001:67c:4e8::/48"), QStringLiteral("2001:b28:f23c::/47"),
                        QStringLiteral("2001:b28:f23f::/48"), QStringLiteral("2a0a:f280:203::/48")}));
    return sniffer;
}

} // namespace

ControledKernelConfig::ControledKernelConfig(AppConfig *appConfig, QObject *parent)
    : QObject(parent), m_appConfig(appConfig)
{
}

QJsonObject ControledKernelConfig::defaults()
{
    QJsonObject config;

    config.insert(QStringLiteral("external-controller"), QString());
    // 说明：本内核未实现外置面板（external-controller / WebUI）与 geo 数据自动
    // 更新，因此不写入 external-ui-url、geox-url 等下载地址，避免配置文件里
    // 出现无用的外部链接。
    config.insert(QStringLiteral("external-ui"), QString());
    config.insert(QStringLiteral("ipv6"), true);
    config.insert(QStringLiteral("mode"), QStringLiteral("rule"));
    config.insert(QStringLiteral("mixed-port"), 7890);
    config.insert(QStringLiteral("socks-port"), 7891);
    config.insert(QStringLiteral("port"), 7892);
    config.insert(QStringLiteral("redir-port"), 0);
    config.insert(QStringLiteral("tproxy-port"), 0);
    config.insert(QStringLiteral("allow-lan"), false);
    config.insert(QStringLiteral("unified-delay"), true);
    config.insert(QStringLiteral("tcp-concurrent"), false);
    config.insert(QStringLiteral("log-level"), QStringLiteral("info"));
    config.insert(QStringLiteral("find-process-mode"), QStringLiteral("strict"));
    config.insert(QStringLiteral("bind-address"), QStringLiteral("*"));
    config.insert(QStringLiteral("lan-allowed-ips"),
                  QJsonArray::fromStringList({QStringLiteral("0.0.0.0/0"),
                                              QStringLiteral("::/0")}));
    config.insert(QStringLiteral("lan-disallowed-ips"), QJsonArray());
    config.insert(QStringLiteral("authentication"), QJsonArray());
    config.insert(QStringLiteral("skip-auth-prefixes"),
                  QJsonArray::fromStringList({QStringLiteral("127.0.0.1/32"),
                                              QStringLiteral("::1/128")}));

    config.insert(QStringLiteral("tun"), defaultTun());
    config.insert(QStringLiteral("dns"), defaultDns());
    config.insert(QStringLiteral("sniffer"), defaultSniffer());

    QJsonObject profile;
    profile.insert(QStringLiteral("store-selected"), true);
    profile.insert(QStringLiteral("store-fake-ip"), true);
    config.insert(QStringLiteral("profile"), profile);


    return config;
}

bool ControledKernelConfig::load()
{
    // 受控段存放在 config.yaml 的 kernel 键下
    QJsonObject parsed;
    if (m_appConfig) {
        parsed = m_appConfig->object().value(QStringLiteral("kernel")).toObject();
    }
    if (parsed.isEmpty()) parsed = importLegacyFile();

    // 已废弃的键：本内核不实现外置面板与 geo 数据自动更新，这些键会带入
    // GitHub 下载地址。此前版本已把它们写进配置文件，这里主动清除。
    for (const QString &retired : retiredKeys()) parsed.remove(retired);

    m_object = core::jsonutil::deepMerge(defaults(), parsed);
    m_loaded = true;

    // 首次启动（或补齐了新增默认值）时把完整 kernel 段写回 config.yaml
    if (parsed != m_object) save();
    return true;
}

// 兼容旧版本：受控配置曾单独存于 kernel.yaml（更早为 mihomo.yaml）。
// 首次加载时把内容并入 config.yaml 并删除旧文件，避免残留无用文件。
QJsonObject ControledKernelConfig::importLegacyFile()
{
    const QStringList legacyNames{QStringLiteral("/kernel.yaml"),
                                  QStringLiteral("/mihomo.yaml")};
    for (const QString &name : legacyNames) {
        const QString path = paths::dataDir() + name;
        bool          ok   = false;
        const QString content = safefile::readText(path, &ok);
        if (!ok || content.trimmed().isEmpty()) continue;

        QJsonObject parsed;
        QString     error;
        const auto  value = core::Yaml::parse(content, &error);
        if (value && value->isObject()) {
            parsed = value->toObject();
        } else if (!error.isEmpty()) {
            AppLogger::instance().error(QCoreApplication::translate("clashqt::app", "Failed to parse the legacy managed config (%1): %2")
                                            .arg(path, error));
        }

        QFile::remove(path);
        AppLogger::instance().info(
            QCoreApplication::translate("clashqt::app", "Merged the legacy managed config into config.yaml and deleted it: %1").arg(path));
        return parsed;
    }
    return {};
}

bool ControledKernelConfig::save()
{
    if (!m_appConfig) return false;
    // 整体替换 kernel 段并立即落盘 config.yaml —— 必须用替换而非深合并，
    // 否则上面清除掉的废弃键会残留。
    m_appConfig->setKernelSection(m_object, true);
    return true;
}

void ControledKernelConfig::patch(const QJsonObject &patch, bool persist)
{
    m_object = core::jsonutil::deepMerge(m_object, patch);
    if (persist) save();
    emit configChanged();
}

// ---- 常用访问 -------------------------------------------------------------

QString ControledKernelConfig::mode() const
{
    return core::jsonutil::stringValue(m_object, QStringLiteral("mode"), QStringLiteral("rule"));
}

void ControledKernelConfig::setMode(const QString &mode)
{
    m_object.insert(QStringLiteral("mode"), mode);
}

QString ControledKernelConfig::logLevel() const
{
    return core::jsonutil::stringValue(m_object, QStringLiteral("log-level"),
                                         QStringLiteral("info"));
}

int ControledKernelConfig::mixedPort() const
{
    return core::jsonutil::intValue(m_object, QStringLiteral("mixed-port"));
}
int ControledKernelConfig::socksPort() const
{
    return core::jsonutil::intValue(m_object, QStringLiteral("socks-port"));
}
int ControledKernelConfig::httpPort() const
{
    return core::jsonutil::intValue(m_object, QStringLiteral("port"));
}
int ControledKernelConfig::redirPort() const
{
    return core::jsonutil::intValue(m_object, QStringLiteral("redir-port"));
}
int ControledKernelConfig::tproxyPort() const
{
    return core::jsonutil::intValue(m_object, QStringLiteral("tproxy-port"));
}

void ControledKernelConfig::setMixedPort(int port)
{
    m_object.insert(QStringLiteral("mixed-port"), port);
}
void ControledKernelConfig::setSocksPort(int port)
{
    m_object.insert(QStringLiteral("socks-port"), port);
}
void ControledKernelConfig::setHttpPort(int port) { m_object.insert(QStringLiteral("port"), port); }

bool ControledKernelConfig::ipv6() const
{
    return core::jsonutil::boolValue(m_object, QStringLiteral("ipv6"), true);
}
bool ControledKernelConfig::allowLan() const
{
    return core::jsonutil::boolValue(m_object, QStringLiteral("allow-lan"), false);
}
QString ControledKernelConfig::bindAddress() const
{
    return core::jsonutil::stringValue(m_object, QStringLiteral("bind-address"),
                                         QStringLiteral("*"));
}

bool ControledKernelConfig::tunEnabled() const
{
    return core::jsonutil::boolValue(m_object.value(QStringLiteral("tun")).toObject(),
                                       QStringLiteral("enable"), false);
}

void ControledKernelConfig::setTunEnabled(bool enabled)
{
    QJsonObject tun = m_object.value(QStringLiteral("tun")).toObject();
    tun.insert(QStringLiteral("enable"), enabled);
    m_object.insert(QStringLiteral("tun"), tun);
}

QString ControledKernelConfig::tunStack() const
{
    return core::jsonutil::stringValue(m_object.value(QStringLiteral("tun")).toObject(),
                                         QStringLiteral("stack"), QStringLiteral("mixed"));
}

void ControledKernelConfig::setTunStack(const QString &stack)
{
    QJsonObject tun = m_object.value(QStringLiteral("tun")).toObject();
    tun.insert(QStringLiteral("stack"), stack);
    m_object.insert(QStringLiteral("tun"), tun);
}

QString ControledKernelConfig::tunDevice() const
{
    return core::jsonutil::stringValue(m_object.value(QStringLiteral("tun")).toObject(),
                                         QStringLiteral("device"), defaultTunDevice());
}

bool ControledKernelConfig::tunAutoRoute() const
{
    // 默认 true：与参照实现的 tun.auto-route 默认一致
    return core::jsonutil::boolValue(m_object.value(QStringLiteral("tun")).toObject(),
                                      QStringLiteral("auto-route"), true);
}

void ControledKernelConfig::setTunAutoRoute(bool enabled)
{
    QJsonObject tun = m_object.value(QStringLiteral("tun")).toObject();
    tun.insert(QStringLiteral("auto-route"), enabled);
    m_object.insert(QStringLiteral("tun"), tun);
}

int ControledKernelConfig::tunMtu() const
{
    const QJsonObject tun = m_object.value(QStringLiteral("tun")).toObject();
    const int mtu = tun.value(QStringLiteral("mtu")).toInt(1500);
    return (mtu >= 576 && mtu <= 9000) ? mtu : 1500;
}

bool ControledKernelConfig::dnsEnabled() const
{
    return core::jsonutil::boolValue(m_object.value(QStringLiteral("dns")).toObject(),
                                       QStringLiteral("enable"), true);
}

void ControledKernelConfig::setDnsEnabled(bool enabled)
{
    QJsonObject dns = m_object.value(QStringLiteral("dns")).toObject();
    dns.insert(QStringLiteral("enable"), enabled);
    m_object.insert(QStringLiteral("dns"), dns);
}

QJsonObject ControledKernelConfig::dns() const
{
    return m_object.value(QStringLiteral("dns")).toObject();
}

void ControledKernelConfig::setDns(const QJsonObject &dns)
{
    m_object.insert(QStringLiteral("dns"), dns);
}

bool ControledKernelConfig::snifferEnabled() const
{
    return core::jsonutil::boolValue(m_object.value(QStringLiteral("sniffer")).toObject(),
                                       QStringLiteral("enable"), true);
}

void ControledKernelConfig::setSnifferEnabled(bool enabled)
{
    QJsonObject sniffer = m_object.value(QStringLiteral("sniffer")).toObject();
    sniffer.insert(QStringLiteral("enable"), enabled);
    m_object.insert(QStringLiteral("sniffer"), sniffer);
}

QString ControledKernelConfig::externalController() const
{
    return core::jsonutil::stringValue(m_object, QStringLiteral("external-controller"));
}

void ControledKernelConfig::setExternalController(const QString &address)
{
    m_object.insert(QStringLiteral("external-controller"), address);
}

QStringList ControledKernelConfig::authentication() const
{
    return core::jsonutil::stringArrayValue(m_object, QStringLiteral("authentication"));
}

} // namespace clashqt
