#include <QtGlobal> // Q_OS_WIN 等平台宏（必须早于下面的平台分支）

// 开机自启动：Linux 写 XDG autostart 的 .desktop，Windows 写
// HKCU\...\CurrentVersion\Run（无需管理员权限，一条值即可写/删，卸载无残留）。

#include "sys/AutoRun.h"

#include "utils/AppLogger.h"

#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>

#include <string>
#else
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QFile>
#include <QStandardPaths>
#endif

namespace clashqt {
namespace {

#ifdef Q_OS_WIN

constexpr auto kRunKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

// Run 键下的值名：与 CMake 下发的应用名一致，改名时只有一处。
// CLASHQT_APP_NAME 展开为窄字面量，前面加 L"" 让它在编译期拼成宽字面量
// （注册表 API 全是 W 版本，不收 const char*）。
const wchar_t *valueName()
{
    return L"" CLASHQT_APP_NAME;
}

QString runKeyDisplayPath()
{
    return QStringLiteral("HKEY_CURRENT_USER\\%1\\%2")
        .arg(QString::fromWCharArray(kRunKey), QString::fromWCharArray(valueName()));
}

// 写入 Run 键的命令行；路径统一加引号，避免含空格的安装路径被拆开
bool writeValue(const QString &command, QString *error)
{
    HKEY key = nullptr;
    if (::RegCreateKeyExW(HKEY_CURRENT_USER, kRunKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key,
                          nullptr)
        != ERROR_SUCCESS) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::app",
                                                 "Could not open the autostart registry key");
        }
        return false;
    }

    const std::wstring text  = command.toStdWString();
    const DWORD        bytes = DWORD((text.size() + 1) * sizeof(wchar_t));
    const LSTATUS      status =
        ::RegSetValueExW(key, valueName(), 0, REG_SZ,
                         reinterpret_cast<const BYTE *>(text.c_str()), bytes);
    ::RegCloseKey(key);

    if (status != ERROR_SUCCESS) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::app",
                                                 "Could not write the autostart entry (error %1)")
                         .arg(int(status));
        }
        return false;
    }
    return true;
}

#else // !Q_OS_WIN

// 自启动入口的文件名与安装的桌面文件一致（由 CMake 生成）
constexpr auto kDesktopFileName = CLASHQT_DESKTOP_FILE;

#endif // Q_OS_WIN

} // namespace

AutoRun::AutoRun(QObject *parent) : QObject(parent) {}

QString AutoRun::desktopFilePath()
{
#ifdef Q_OS_WIN
    // 名字沿用 desktopFilePath 是为了不改公开接口；Windows 上没有 .desktop 文件，
    // 返回注册表位置，便于日志与界面提示
    return runKeyDisplayPath();
#else
    const QString configHome =
        QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return configHome + QStringLiteral("/autostart/") + QString::fromLatin1(kDesktopFileName);
#endif
}

bool AutoRun::writeDesktopFile(const QString &content, QString *error)
{
#ifdef Q_OS_WIN
    // Windows 上没有 .desktop 文件，content 就是命令行本身
    return writeValue(content, error);
#else
    const QString path = desktopFilePath();
    if (!paths::ensureParentDir(path)) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Could not create the autostart directory");
        return false;
    }
    return safefile::writeAtomic(path, content, error);
#endif
}

bool AutoRun::isEnabled() const
{
#ifdef Q_OS_WIN
    HKEY key = nullptr;
    if (::RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }

    wchar_t       buffer[2048] = {0};
    DWORD         type = 0;
    DWORD         size = sizeof(buffer);
    const LSTATUS status = ::RegQueryValueExW(key, valueName(), nullptr, &type,
                                              reinterpret_cast<LPBYTE>(buffer), &size);
    ::RegCloseKey(key);

    if (status != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ)) {
        return false;
    }

    // 与 Linux 版同款判断：自启动项必须确实指向当前可执行文件。
    // 用户移动了程序位置时旧项是无效的，报「已启用」会误导。
    return QString::fromWCharArray(buffer).contains(QCoreApplication::applicationFilePath());
#else
    QFile file(desktopFilePath());
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) return false;

    const QString content = QString::fromUtf8(file.readAll());
    // 判断自启动项是否确实指向当前可执行文件：
    // 用户可能移动了程序位置，此时旧的自启动项是无效的，
    // 报告「已启用」会让用户以为下次开机会自动运行，实际不会。
    return content.contains(QCoreApplication::applicationFilePath());
#endif
}

bool AutoRun::enable(bool silent, QString *error)
{
#ifdef Q_OS_WIN
    const QString executable = QCoreApplication::applicationFilePath();
    // 注册表值本身就是命令行：路径必须加引号，避免含空格的安装路径被拆开
    const QString command    = silent ? QStringLiteral("\"%1\" --silent").arg(executable)
                                      : QStringLiteral("\"%1\"").arg(executable);

    if (!writeValue(command, error)) {
        AppLogger::instance().error(
            QCoreApplication::translate("clashqt::app", "Failed to enable autostart: %1")
                .arg(error ? *error : QString()));
        return false;
    }

    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "Autostart enabled: %1")
            .arg(runKeyDisplayPath()));
    return true;
#else
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
            "Comment=A desktop client for Clash-style subscriptions\n"
            "Exec=%1\n"
            "Terminal=false\n"
            "StartupNotify=false\n"
            "X-GNOME-Autostart-enabled=true\n")
            .arg(execLine);

    if (!writeDesktopFile(content, error)) {
        AppLogger::instance().error(QCoreApplication::translate("clashqt::app", "Failed to enable autostart: %1").arg(*error));
        return false;
    }
    AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "Autostart enabled: %1").arg(desktopFilePath()));
    return true;
#endif
}

bool AutoRun::disable(QString *error)
{
#ifdef Q_OS_WIN
    HKEY key = nullptr;
    if (::RegOpenKeyExW(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
        // 键都不存在就等于「本来没启用」
        return true;
    }

    const LSTATUS status = ::RegDeleteValueW(key, valueName());
    ::RegCloseKey(key);

    if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND) {
        if (error) {
            *error = QCoreApplication::translate("clashqt::app",
                                                 "Could not remove the autostart entry (error %1)")
                         .arg(int(status));
        }
        AppLogger::instance().error(error ? *error : QString());
        return false;
    }

    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "Autostart disabled"));
    return true;
#else
    const QString path = desktopFilePath();
    if (!QFile::exists(path)) return true;

    if (!QFile::remove(path)) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Could not remove the autostart entry: %1").arg(path);
        AppLogger::instance().error(*error);
        return false;
    }
    AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "Autostart disabled"));
    return true;
#endif
}

} // namespace clashqt
