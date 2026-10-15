#include "utils/Paths.h"

#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

namespace clashqt {
namespace paths {
namespace {

// 日志文件名中的日期段，与 clash-party 的命名保持一致
QString dateStamp()
{
    return QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
}

} // namespace

QString exeDir()
{
    return QCoreApplication::applicationDirPath();
}

bool isPortable()
{
    return QFileInfo::exists(exeDir() + QStringLiteral("/PORTABLE"));
}

QString dataDir()
{
    // 便携模式优先级最高：便于放在 U 盘或独立目录中运行而不污染用户配置
    if (isPortable()) {
        const QString portable = exeDir() + QStringLiteral("/data");
        ensureDir(portable);
        return portable;
    }

    QString base = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (base.isEmpty()) {
        base = QDir::homePath() + QStringLiteral("/.config/clash-qt");
    }
    ensureDir(base);
    return base;
}

bool ensureDir(const QString &path)
{
    if (path.isEmpty()) return false;
    QDir dir;
    return dir.mkpath(path);
}

bool ensureParentDir(const QString &filePath)
{
    const QFileInfo info(filePath);
    return ensureDir(info.absolutePath());
}

QString appConfigPath() { return dataDir() + QStringLiteral("/config.yaml"); }
QString profileConfigPath() { return dataDir() + QStringLiteral("/profile.yaml"); }
QString overrideConfigPath() { return dataDir() + QStringLiteral("/override.yaml"); }

QString profilesDir()
{
    const QString dir = dataDir() + QStringLiteral("/profiles");
    ensureDir(dir);
    return dir;
}

QString profilePath(const QString &id)
{
    return profilesDir() + QLatin1Char('/') + id + QStringLiteral(".yaml");
}

QString overrideDir()
{
    const QString dir = dataDir() + QStringLiteral("/override");
    ensureDir(dir);
    return dir;
}

QString overridePath(const QString &id, const QString &ext)
{
    return overrideDir() + QLatin1Char('/') + id + QLatin1Char('.') + ext;
}

QString rulesDir()
{
    const QString dir = dataDir() + QStringLiteral("/rules");
    ensureDir(dir);
    return dir;
}

QString rulePath(const QString &id)
{
    return rulesDir() + QLatin1Char('/') + id + QStringLiteral(".yaml");
}

QString workDir()
{
    const QString dir = dataDir() + QStringLiteral("/work");
    ensureDir(dir);
    return dir;
}

QString workConfigPath()
{
    return workDir() + QStringLiteral("/config.yaml");
}

QString logDir()
{
    const QString dir = dataDir() + QStringLiteral("/logs");
    ensureDir(dir);
    return dir;
}

QString appLogPath()
{
    return logDir() + QStringLiteral("/clash-qt-") + dateStamp() + QStringLiteral(".log");
}

QString coreLogPath()
{
    return logDir() + QStringLiteral("/core-") + dateStamp() + QStringLiteral(".log");
}

} // namespace paths
} // namespace clashqt
