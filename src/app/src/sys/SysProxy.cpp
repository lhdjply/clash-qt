#include "sys/SysProxy.h"

#include "utils/AppLogger.h"

#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

namespace clashqt {
namespace {

// 系统代理的默认忽略列表：本机与私有网段不应走代理
QStringList defaultBypass()
{
    return {QStringLiteral("localhost"), QStringLiteral("127.0.0.1"), QStringLiteral("::1"),
            QStringLiteral("10.0.0.0/8"), QStringLiteral("172.16.0.0/12"),
            QStringLiteral("192.168.0.0/16"), QStringLiteral("*.local")};
}

QString kdeConfigPath()
{
    const QString configHome = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return configHome + QStringLiteral("/kioslaverc");
}

} // namespace

SysProxy::SysProxy(QObject *parent) : QObject(parent)
{
    detect();
}

void SysProxy::detect()
{
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
        QStringLiteral("系统代理后端：%1（XDG_CURRENT_DESKTOP=%2）")
            .arg(backendName(), qEnvironmentVariable("XDG_CURRENT_DESKTOP")));
}

QString SysProxy::backendName() const
{
    switch (m_backend) {
    case Backend::GSettings: return QStringLiteral("gsettings");
    case Backend::KdeConfig: return QStringLiteral("kioslaverc");
    case Backend::None:      return QStringLiteral("不可用");
    }
    return QStringLiteral("不可用");
}

bool SysProxy::run(const QString &program, const QStringList &arguments, QString *error)
{
    QProcess process;
    process.start(program, arguments);
    if (!process.waitForStarted(5000)) {
        if (error) *error = QStringLiteral("无法启动 %1").arg(program);
        return false;
    }
    if (!process.waitForFinished(10000)) {
        process.kill();
        if (error) *error = QStringLiteral("%1 执行超时").arg(program);
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error) {
            *error = QStringLiteral("%1 执行失败：%2")
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
            QStringLiteral("写入系统代理忽略列表失败：%1").arg(error2));
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
        if (error) *error = QStringLiteral("未找到 kwriteconfig5/6");
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
        if (error) *error = QStringLiteral("未找到 kwriteconfig5/6");
        return false;
    }
    // ProxyType=0 表示不使用代理
    return run(writer, {QStringLiteral("--file"), kdeConfigPath(), QStringLiteral("--group"),
                        QStringLiteral("Proxy Settings"), QStringLiteral("--key"),
                        QStringLiteral("ProxyType"), QStringLiteral("0")},
               error);
}

// ---------------------------------------------------------------------------
//  对外接口
// ---------------------------------------------------------------------------

bool SysProxy::isEnabled() const
{
    switch (m_backend) {
    case Backend::GSettings: return gsettingsEnabled();
    case Backend::KdeConfig: return kdeEnabled();
    case Backend::None:      return false;
    }
    return false;
}

bool SysProxy::enable(const QString &host, int port, const QStringList &bypass, QString *error)
{
    if (m_backend == Backend::None) {
        m_lastError = QStringLiteral("未检测到可用的系统代理后端"
                                     "（需要 gsettings 或 kwriteconfig5/6）");
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
        AppLogger::instance().error(QStringLiteral("启用系统代理失败：%1").arg(localError));
        return false;
    }

    m_lastError.clear();
    AppLogger::instance().info(
        QStringLiteral("系统代理已启用（%1）：%2:%3").arg(backendName(), host).arg(port));
    return true;
}

bool SysProxy::disable(QString *error)
{
    if (m_backend == Backend::None) {
        m_lastError = QStringLiteral("未检测到可用的系统代理后端");
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
        AppLogger::instance().error(QStringLiteral("关闭系统代理失败：%1").arg(localError));
        return false;
    }

    m_lastError.clear();
    AppLogger::instance().info(QStringLiteral("系统代理已关闭"));
    return true;
}

} // namespace clashqt
