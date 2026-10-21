#include <QtGlobal> // Q_OS_WIN 等平台宏（必须早于下面的平台分支）

// 系统代理：Linux 走 gsettings / kioslaverc，Windows 写注册表 Internet Settings
// 并发 WinINet 刷新通知（只写注册表不会通知已启动的进程）。
// 平台分支写在函数内部，同一函数的两种实现上下相邻，便于逐函数对照。

#include "sys/SysProxy.h"

#include <QCoreApplication>
#include <QRegularExpression>

#include "utils/AppLogger.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <wininet.h>

#include <string>
#else
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#endif

namespace clashqt {
namespace {

// 系统代理的默认忽略列表：本机与私有网段不应走代理（两平台共用）
QStringList defaultBypass()
{
    return {QStringLiteral("localhost"), QStringLiteral("127.0.0.1"), QStringLiteral("::1"),
            QStringLiteral("10.0.0.0/8"), QStringLiteral("172.16.0.0/12"),
            QStringLiteral("192.168.0.0/16"), QStringLiteral("*.local")};
}

#ifdef Q_OS_WIN

// Windows 专有辅助：注册表读写

constexpr auto kInternetSettings =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";

HKEY openSettings(REGSAM access)
{
    HKEY key = nullptr;
    if (::RegOpenKeyExW(HKEY_CURRENT_USER, kInternetSettings, 0, access, &key) != ERROR_SUCCESS) {
        return nullptr;
    }
    return key;
}

bool readDword(const wchar_t *name, DWORD *value)
{
    HKEY key = openSettings(KEY_READ);
    if (!key) return false;

    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    const bool ok = ::RegQueryValueExW(key, name, nullptr, &type,
                                       reinterpret_cast<LPBYTE>(value), &size)
                        == ERROR_SUCCESS
                    && type == REG_DWORD;
    ::RegCloseKey(key);
    return ok;
}

QString readString(const wchar_t *name)
{
    HKEY key = openSettings(KEY_READ);
    if (!key) return QString();

    wchar_t buffer[2048] = {0};
    DWORD   type = 0;
    DWORD   size = sizeof(buffer);
    const bool ok = ::RegQueryValueExW(key, name, nullptr, &type,
                                       reinterpret_cast<LPBYTE>(buffer), &size)
                    == ERROR_SUCCESS
                    && (type == REG_SZ || type == REG_EXPAND_SZ);
    ::RegCloseKey(key);
    return ok ? QString::fromWCharArray(buffer) : QString();
}

bool writeDword(const wchar_t *name, DWORD value)
{
    HKEY key = openSettings(KEY_SET_VALUE);
    if (!key) return false;

    const bool ok = ::RegSetValueExW(key, name, 0, REG_DWORD,
                                     reinterpret_cast<const BYTE *>(&value), sizeof(value))
                    == ERROR_SUCCESS;
    ::RegCloseKey(key);
    return ok;
}

bool writeString(const wchar_t *name, const QString &value)
{
    HKEY key = openSettings(KEY_SET_VALUE);
    if (!key) return false;

    const std::wstring text  = value.toStdWString();
    const DWORD      bytes   = DWORD((text.size() + 1) * sizeof(wchar_t));
    const bool       ok      = ::RegSetValueExW(key, name, 0, REG_SZ,
                                                reinterpret_cast<const BYTE *>(text.c_str()), bytes)
                         == ERROR_SUCCESS;
    ::RegCloseKey(key);
    return ok;
}

// 通知 WinINet 重新读取设置；失败不影响注册表已经写入的内容
void notifySettingsChanged()
{
    ::InternetSetOptionW(nullptr, INTERNET_OPTION_SETTINGS_CHANGED, nullptr, 0);
    ::InternetSetOptionW(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
}

// ProxyOverride 不支持 CIDR，只认主机名、IPv4 通配符和 <local>。
// 这里把默认列表里能表达的转过去，表达不了的（/12 这类跨段区间）跳过 ——
// 宁可少绕一项，也不写一条无效规则让用户以为已绕开。
QString buildOverride(const QStringList &bypass)
{
    const QStringList source = bypass.isEmpty() ? defaultBypass() : bypass;

    QStringList out;
    out << QStringLiteral("<local>");

    static const QRegularExpression cidr(QStringLiteral(R"(^(\d+)\.(\d+)\.\d+\.\d+/(\d+)$)"));

    for (const QString &entry : source) {
        const QString trimmed = entry.trimmed();
        if (trimmed.isEmpty()) continue;

        // IPv6 字面量在 ProxyOverride 里无效，<local> 已覆盖本机
        if (trimmed.contains(QLatin1Char(':'))) continue;

        const auto match = cidr.match(trimmed);
        if (match.hasMatch()) {
            const QString first  = match.captured(1);
            const QString second = match.captured(2);
            const int     prefix = match.captured(3).toInt();
            if (prefix == 8) {
                out << first + QStringLiteral(".*");
            } else if (prefix == 16) {
                out << first + QLatin1Char('.') + second + QStringLiteral(".*");
            }
            // 其它前缀无法用通配符精确表达，直接跳过
            continue;
        }
        out << trimmed;
    }

    out.removeDuplicates();
    return out.join(QLatin1Char(';'));
}

// 解析 "http=1.2.3.4:80;https=...;socks=..."；也接受单一地址形式
bool parseProxyServer(const QString &server, QString *http, QString *https, QString *socks)
{
    if (server.isEmpty()) return false;

    if (!server.contains(QLatin1Char('='))) {
        // 只写一个地址：对三种协议都生效
        *http  = server.trimmed();
        *https = *http;
        *socks = *http;
        return true;
    }

    const QStringList parts = server.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        const int separator = part.indexOf(QLatin1Char('='));
        if (separator <= 0) continue;
        const QString scheme = part.left(separator).trimmed().toLower();
        const QString value  = part.mid(separator + 1).trimmed();
        if (scheme == QLatin1String("http")) *http = value;
        else if (scheme == QLatin1String("https")) *https = value;
        else if (scheme == QLatin1String("socks")) *socks = value;
    }
    return !http->isEmpty() || !https->isEmpty() || !socks->isEmpty();
}

#else // !Q_OS_WIN

// Linux 专有辅助：kioslaverc 解析

QString kdeConfigPath()
{
    const QString configHome = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return configHome + QStringLiteral("/kioslaverc");
}

// gsettings get 返回的是 GVariant 字面量（字符串带单引号），比较前先剥掉
QString unquote(const QString &value)
{
    QString text = value.trimmed();
    if (text.size() >= 2 && text.startsWith(QLatin1Char('\'')) && text.endsWith(QLatin1Char('\''))) {
        text = text.mid(1, text.size() - 2);
    }
    return text;
}

// KDE 的 kioslaverc 里代理写成 "http://127.0.0.1:7890"（我们写的）或
// "http://127.0.0.1 7890"（KDE 自身格式），两种都要认
bool parseKdeProxy(const QString &value, QString *host, int *port)
{
    const int schemeEnd = value.indexOf(QLatin1String("://"));
    if (schemeEnd < 0) return false;

    QString rest = value.mid(schemeEnd + 3).trimmed();
    const int separator = rest.indexOf(QRegularExpression(QStringLiteral("[:\\s]")));
    if (separator < 0) return false;

    *host = rest.left(separator).trimmed();
    bool ok = false;
    *port = rest.mid(separator + 1).trimmed().toInt(&ok);
    return ok && !host->isEmpty();
}

#endif // Q_OS_WIN

} // namespace

SysProxy::SysProxy(QObject *parent) : QObject(parent)
{
    detect();
}

void SysProxy::detect()
{
#ifdef Q_OS_WIN
    // Windows 上这条链路始终可用（注册表是系统自带的），没有「后端探测」这一步
    m_backend = Backend::WinInet;
    AppLogger::instance().debug(
        QCoreApplication::translate("clashqt::app", "System-proxy backend: %1")
            .arg(backendName()));
#else
    m_backend = Backend::None;

    const QString desktop =
        qEnvironmentVariable("XDG_CURRENT_DESKTOP").toLower()
        + QLatin1Char(':') + qEnvironmentVariable("DESKTOP_SESSION").toLower();

    const bool hasGsettings =
        !QStandardPaths::findExecutable(QStringLiteral("gsettings")).isEmpty();
    const bool hasKwrite = !QStandardPaths::findExecutable(QStringLiteral("kwriteconfig6")).isEmpty()
                           || !QStandardPaths::findExecutable(QStringLiteral("kwriteconfig5")).isEmpty();

    // 优先按桌面环境选择；环境变量不可靠时再按可用工具回退
    if (desktop.contains(QLatin1String("kde")) || desktop.contains(QLatin1String("plasma"))) {
        if (hasKwrite) m_backend = Backend::KdeConfig;
        else if (hasGsettings) m_backend = Backend::GSettings;
    } else {
        if (hasGsettings) m_backend = Backend::GSettings;
        else if (hasKwrite) m_backend = Backend::KdeConfig;
    }

    AppLogger::instance().debug(
        QCoreApplication::translate("clashqt::app", "System-proxy backend: %1 (XDG_CURRENT_DESKTOP=%2)")
            .arg(backendName(), qEnvironmentVariable("XDG_CURRENT_DESKTOP")));
#endif
}

QString SysProxy::backendName() const
{
    // 名称映射与平台无关：WinInet 只会在 Windows 构建里被 detect() 选中，
    // gsettings / kioslaverc 只会在 Linux 构建里被选中，所以一个 switch 够用
    switch (m_backend) {
    case Backend::GSettings: return QStringLiteral("gsettings");
    case Backend::KdeConfig: return QStringLiteral("kioslaverc");
    case Backend::WinInet:   return QStringLiteral("wininet");
    case Backend::None:      break;
    }
    return QCoreApplication::translate("clashqt::app", "unavailable");
}

bool SysProxy::isEnabled() const
{
#ifdef Q_OS_WIN
    DWORD enabled = 0;
    if (!readDword(L"ProxyEnable", &enabled)) return false;
    return enabled != 0;
#else
    switch (m_backend) {
    case Backend::GSettings: return gsettingsEnabled();
    case Backend::KdeConfig: return kdeEnabled();
    case Backend::WinInet:
    case Backend::None:      return false;
    }
    return false;
#endif
}

bool SysProxy::targets(const QString &host, int port) const
{
#ifdef Q_OS_WIN
    DWORD enabled = 0;
    if (!readDword(L"ProxyEnable", &enabled) || enabled == 0) return false;

    QString http;
    QString https;
    QString socks;
    if (!parseProxyServer(readString(L"ProxyServer"), &http, &https, &socks)) return false;

    const QString expected = QStringLiteral("%1:%2").arg(host).arg(port);
    const auto equalToExpected = [&expected](const QString &value) {
        return value.compare(expected, Qt::CaseInsensitive) == 0;
    };
    return equalToExpected(http) && equalToExpected(https) && equalToExpected(socks);
#else
    switch (m_backend) {
    case Backend::GSettings: return gsettingsTargets(host, port);
    case Backend::KdeConfig: return kdeTargets(host, port);
    case Backend::WinInet:
    case Backend::None:      return false;
    }
    return false;
#endif
}

bool SysProxy::enable(const QString &host, int port, const QStringList &bypass, QString *error)
{
#ifdef Q_OS_WIN
    const QString proxy  = QStringLiteral("%1:%2").arg(host).arg(port);
    const QString server = QStringLiteral("http=%1;https=%1;socks=%1").arg(proxy);

    if (!writeString(L"ProxyServer", server)) {
        m_lastError = QCoreApplication::translate(
            "clashqt::app", "Could not write ProxyServer to the registry");
        if (error) *error = m_lastError;
        AppLogger::instance().error(m_lastError);
        return false;
    }
    if (!writeString(L"ProxyOverride", buildOverride(bypass))) {
        // 忽略列表写失败不影响代理本身，降级为告警（与 Linux 版一致）
        AppLogger::instance().warn(QCoreApplication::translate(
            "clashqt::app", "Failed to write the system-proxy ignore list"));
    }
    if (!writeDword(L"ProxyEnable", 1)) {
        m_lastError = QCoreApplication::translate(
            "clashqt::app", "Could not enable the system proxy in the registry");
        if (error) *error = m_lastError;
        AppLogger::instance().error(m_lastError);
        return false;
    }

    notifySettingsChanged();

    m_lastError.clear();
    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "System proxy enabled (%1): %2:%3")
            .arg(backendName(), host)
            .arg(port));
    return true;
#else
    if (m_backend == Backend::None) {
        m_lastError = QCoreApplication::translate("clashqt::app", "No usable system-proxy backend detected (gsettings or kwriteconfig5/6 is required)");
        if (error) *error = m_lastError;
        return false;
    }

    QString localError;
    bool    ok = false;
    switch (m_backend) {
    case Backend::GSettings: ok = enableGSettings(host, port, bypass, &localError); break;
    case Backend::KdeConfig: ok = enableKde(host, port, bypass, &localError); break;
    case Backend::None:      break;
    }

    if (!ok) {
        m_lastError = localError;
        if (error) *error = localError;
        AppLogger::instance().error(QCoreApplication::translate("clashqt::app", "Failed to enable the system proxy: %1").arg(localError));
        return false;
    }

    m_lastError.clear();
    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "System proxy enabled (%1): %2:%3").arg(backendName(), host).arg(port));
    return true;
#endif
}

bool SysProxy::disable(QString *error)
{
#ifdef Q_OS_WIN
    if (!writeDword(L"ProxyEnable", 0)) {
        m_lastError = QCoreApplication::translate(
            "clashqt::app", "Could not turn off the system proxy in the registry");
        if (error) *error = m_lastError;
        AppLogger::instance().error(m_lastError);
        return false;
    }

    notifySettingsChanged();

    m_lastError.clear();
    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "System proxy disabled"));
    return true;
#else
    if (m_backend == Backend::None) {
        m_lastError = QCoreApplication::translate("clashqt::app", "No usable system-proxy backend detected");
        if (error) *error = m_lastError;
        return false;
    }

    QString localError;
    bool    ok = false;
    switch (m_backend) {
    case Backend::GSettings: ok = disableGSettings(&localError); break;
    case Backend::KdeConfig: ok = disableKde(&localError); break;
    case Backend::None:      break;
    }

    if (!ok) {
        m_lastError = localError;
        if (error) *error = localError;
        AppLogger::instance().error(QCoreApplication::translate("clashqt::app", "Failed to turn off the system proxy: %1").arg(localError));
        return false;
    }

    m_lastError.clear();
    AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "System proxy disabled"));
    return true;
#endif
}

#ifndef Q_OS_WIN

// 以下只有 Linux 后端（gsettings / kioslaverc）需要：没有 Windows 对等物，
// 只能整段守卫，而不是函数内的 #ifdef。

bool SysProxy::run(const QString &program, const QStringList &arguments, QString *error)
{
    QProcess process;
    process.start(program, arguments);
    if (!process.waitForStarted(5000)) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Could not start %1").arg(program);
        return false;
    }
    if (!process.waitForFinished(10000)) {
        process.kill();
        if (error) *error = QCoreApplication::translate("clashqt::app", "%1 timed out").arg(program);
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::app", "%1 failed: %2")
                         .arg(program, QString::fromUtf8(process.readAllStandardError()).trimmed());
        }
        return false;
    }
    return true;
}

QString SysProxy::runCapture(const QString &program, const QStringList &arguments)
{
    QProcess process;
    process.start(program, arguments);
    if (!process.waitForStarted(5000) || !process.waitForFinished(10000)) {
        process.kill();
        return QString();
    }
    return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
}

// ---------------------------------------------------------------------------
//  gsettings 后端（GNOME / Cinnamon / Unity / Deepin 等）
// ---------------------------------------------------------------------------

bool SysProxy::gsettingsEnabled() const
{
    const QString mode = runCapture(QStringLiteral("gsettings"),
                                    {QStringLiteral("get"), QStringLiteral("org.gnome.system.proxy"),
                                     QStringLiteral("mode")});
    return mode.contains(QLatin1String("manual")) || mode.contains(QLatin1String("auto"));
}

bool SysProxy::enableGSettings(const QString &host, int port, const QStringList &bypass,
                              QString *error)
{
    const QString schema = QStringLiteral("org.gnome.system.proxy");
    const QString g      = QStringLiteral("gsettings");

    QStringList commands[] = {
        {QStringLiteral("set"), schema, QStringLiteral("mode"), QStringLiteral("manual")},
        {QStringLiteral("set"), schema + QStringLiteral(".http"), QStringLiteral("host"), host},
        {QStringLiteral("set"), schema + QStringLiteral(".http"), QStringLiteral("port"),
         QString::number(port)},
        {QStringLiteral("set"), schema + QStringLiteral(".https"), QStringLiteral("host"), host},
        {QStringLiteral("set"), schema + QStringLiteral(".https"), QStringLiteral("port"),
         QString::number(port)},
        {QStringLiteral("set"), schema + QStringLiteral(".socks"), QStringLiteral("host"), host},
        {QStringLiteral("set"), schema + QStringLiteral(".socks"), QStringLiteral("port"),
         QString::number(port)},
    };

    for (const QStringList &arguments : commands) {
        if (!run(g, arguments, error)) return false;
    }

    // ignore-hosts 需要写成 gsettings 的数组字面量形式
    const QStringList effectiveBypass = bypass.isEmpty() ? defaultBypass() : bypass;
    QStringList       quoted;
    for (const QString &entry : effectiveBypass) {
        quoted << QStringLiteral("'%1'").arg(entry);
    }
    const QString arrayLiteral = QStringLiteral("[%1]").arg(quoted.join(QStringLiteral(", ")));

    QString error2;
    if (!run(g, {QStringLiteral("set"), schema, QStringLiteral("ignore-hosts"), arrayLiteral},
             &error2)) {
        // 忽略列表写入失败不影响代理生效，降级为告警
        AppLogger::instance().warn(
            QCoreApplication::translate("clashqt::app", "Failed to write the system-proxy ignore list: %1").arg(error2));
    }
    return true;
}

bool SysProxy::disableGSettings(QString *error)
{
    return run(QStringLiteral("gsettings"),
               {QStringLiteral("set"), QStringLiteral("org.gnome.system.proxy"),
                QStringLiteral("mode"), QStringLiteral("none")},
               error);
}

// 逐个协议核对 host/port：我们启用时三项都写成同一个 127.0.0.1:port，
// 所以三项全部吻合才认定「这个手动代理是本程序写的」。
bool SysProxy::gsettingsTargets(const QString &host, int port) const
{
    const QString schema = QStringLiteral("org.gnome.system.proxy");
    const QString g      = QStringLiteral("gsettings");

    const QString mode = runCapture(g, {QStringLiteral("get"), schema, QStringLiteral("mode")});
    if (!mode.contains(QLatin1String("manual"))) return false;

    const QStringList groups = {QStringLiteral("http"), QStringLiteral("https"),
                                QStringLiteral("socks")};
    for (const QString &group : groups) {
        const QString node = schema + QLatin1Char('.') + group;
        const QString currentHost =
            unquote(runCapture(g, {QStringLiteral("get"), node, QStringLiteral("host")}));
        const QString currentPort =
            unquote(runCapture(g, {QStringLiteral("get"), node, QStringLiteral("port")}));
        if (currentHost.compare(host, Qt::CaseInsensitive) != 0) return false;
        if (currentPort.toInt() != port) return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
//  KDE 后端（kioslaverc）
// ---------------------------------------------------------------------------

bool SysProxy::kdeEnabled() const
{
    const QFileInfo info(kdeConfigPath());
    if (!info.exists()) return false;

    const QString content = QString::fromUtf8([](const QString &path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return file.readAll();
    }(kdeConfigPath()));

    // ProxyType=1 表示使用手动指定的代理
    return content.contains(QLatin1String("ProxyType=1"));
}

bool SysProxy::enableKde(const QString &host, int port, const QStringList &bypass, QString *error)
{
    const QString writer = !QStandardPaths::findExecutable(QStringLiteral("kwriteconfig6")).isEmpty()
                               ? QStringLiteral("kwriteconfig6")
                               : QStringLiteral("kwriteconfig5");
    if (writer.isEmpty()) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "kwriteconfig5/6 not found");
        return false;
    }

    const QString file  = kdeConfigPath();
    const QString group = QStringLiteral("Proxy Settings");
    const QString proxy = QStringLiteral("http://%1:%2").arg(host).arg(port);

    const QList<QPair<QString, QString>> entries = {
        {QStringLiteral("ProxyType"), QStringLiteral("1")},
        {QStringLiteral("httpProxy"), proxy},
        {QStringLiteral("httpsProxy"), proxy},
        {QStringLiteral("socksProxy"), proxy},
        {QStringLiteral("NoProxyFor"),
         (bypass.isEmpty() ? defaultBypass() : bypass).join(QLatin1Char(','))},
    };

    for (const auto &entry : entries) {
        if (!run(writer, {QStringLiteral("--file"), file, QStringLiteral("--group"), group,
                          QStringLiteral("--key"), entry.first, entry.second},
                 error)) {
            return false;
        }
    }

    // 通知 KDE 重新读取配置；失败不影响设置本身
    if (!QStandardPaths::findExecutable(QStringLiteral("dbus-send")).isEmpty()) {
        QString notifyError;
        run(QStringLiteral("dbus-send"),
            {QStringLiteral("--type=signal"), QStringLiteral("/KIO/Scheduler"),
             QStringLiteral("org.kde.KIO.Scheduler.reparseSlaveConfiguration"),
             QStringLiteral("string:''")},
            &notifyError);
    }
    return true;
}

bool SysProxy::disableKde(QString *error)
{
    const QString writer = !QStandardPaths::findExecutable(QStringLiteral("kwriteconfig6")).isEmpty()
                               ? QStringLiteral("kwriteconfig6")
                               : QStringLiteral("kwriteconfig5");
    if (writer.isEmpty()) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "kwriteconfig5/6 not found");
        return false;
    }
    // ProxyType=0 表示不使用代理
    return run(writer, {QStringLiteral("--file"), kdeConfigPath(), QStringLiteral("--group"),
                        QStringLiteral("Proxy Settings"), QStringLiteral("--key"),
                        QStringLiteral("ProxyType"), QStringLiteral("0")},
               error);
}

bool SysProxy::kdeTargets(const QString &host, int port) const
{
    QFile file(kdeConfigPath());
    if (!file.open(QIODevice::ReadOnly)) return false;
    const QString content = QString::fromUtf8(file.readAll());
    if (!content.contains(QLatin1String("ProxyType=1"))) return false;

    const QStringList keys = {QStringLiteral("httpProxy"), QStringLiteral("httpsProxy"),
                              QStringLiteral("socksProxy")};
    for (const QString &key : keys) {
        const QRegularExpression pattern(
            QStringLiteral("^%1=(.+)$").arg(key), QRegularExpression::MultilineOption);
        const auto match = pattern.match(content);
        if (!match.hasMatch()) return false;

        QString currentHost;
        int     currentPort = 0;
        if (!parseKdeProxy(match.captured(1).trimmed(), &currentHost, &currentPort)) return false;
        if (currentHost.compare(host, Qt::CaseInsensitive) != 0) return false;
        if (currentPort != port) return false;
    }
    return true;
}

#endif // !Q_OS_WIN

} // namespace clashqt
