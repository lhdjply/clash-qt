#include "sys/AutoRun.h"

#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>

namespace clashqt {
namespace {

constexpr auto kDesktopFileName = "clash-qt.desktop";

} // namespace

AutoRun::AutoRun(QObject *parent) : QObject(parent) {}

QString AutoRun::desktopFilePath()
{
    const QString configHome =
        QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return configHome + QStringLiteral("/autostart/") + QString::fromLatin1(kDesktopFileName);
}

bool AutoRun::writeDesktopFile(const QString &content, QString *error)
{
    const QString path = desktopFilePath();
    if (!paths::ensureParentDir(path)) {
        if (error) *error = QStringLiteral("无法创建自启动目录");
        return false;
    }
    return safefile::writeAtomic(path, content, error);
}

bool AutoRun::isEnabled() const
{
    QFile file(desktopFilePath());
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) return false;

    const QString content = QString::fromUtf8(file.readAll());
    // 判断自启动项是否确实指向当前可执行文件：
    // 用户可能移动了程序位置，此时旧的自启动项是无效的，
    // 报告「已启用」会让用户以为下次开机会自动运行，实际不会。
    return content.contains(QCoreApplication::applicationFilePath());
}

bool AutoRun::enable(bool silent, QString *error)
{
    const QString executable = QCoreApplication::applicationFilePath();

    // .desktop 规范要求 Exec 中的路径含空格时使用引号包裹
    const QString quotedExec = executable.contains(QLatin1Char(' '))
                                   ? QStringLiteral("\"%1\"").arg(executable)
                                   : executable;
    const QString execLine = silent ? quotedExec + QStringLiteral(" --silent") : quotedExec;

    const QString content =
        QStringLiteral(
            "[Desktop Entry]\n"
            "Type=Application\n"
            "Version=1.0\n"
            "Name=Clash Qt\n"
            "Comment=Clash Party rewritten in Qt6 / C++\n"
            "Exec=%1\n"
            "Terminal=false\n"
            "StartupNotify=false\n"
            "X-GNOME-Autostart-enabled=true\n")
            .arg(execLine);

    if (!writeDesktopFile(content, error)) {
        AppLogger::instance().error(QStringLiteral("启用自启动失败：%1").arg(*error));
        return false;
    }
    AppLogger::instance().info(QStringLiteral("已启用开机自启动：%1").arg(desktopFilePath()));
    return true;
}

bool AutoRun::disable(QString *error)
{
    const QString path = desktopFilePath();
    if (!QFile::exists(path)) return true;

    if (!QFile::remove(path)) {
        if (error) *error = QStringLiteral("无法删除自启动项：%1").arg(path);
        AppLogger::instance().error(*error);
        return false;
    }
    AppLogger::instance().info(QStringLiteral("已关闭开机自启动"));
    return true;
}

} // namespace clashqt
