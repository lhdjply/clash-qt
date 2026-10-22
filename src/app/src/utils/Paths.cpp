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

// 日志文件名中的日期段，与参照实现的命名保持一致
QString dateStamp()
{
    return QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
}

// 旧版本把数据目录算成了 <config>/clash-qt/clash-qt：组织名与应用名同时
// 取软件名，而 QStandardPaths::AppConfigLocation 在 Linux 上是
// <config>/<org>/<app>，于是多套了一层。目录修正后这里把旧数据上移一层，
// 否则用户升级后会觉得「订阅、规则、日志全丢了」。
void migrateNestedDataDir(const QString &base)
{
    const QString legacy = base + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);
    if (!QFileInfo(legacy).isDir()) return;

    // 目标就是旧目录的父目录：先整体挪到临时目录再逐个分发。
    // 直接边遍历边往父目录 rename 会改动正在遍历的目录内容。
    const QString staging = base + QStringLiteral("/.migrate-from-nested");
    QDir(staging).removeRecursively(); // 清理上次迁移中断的残留
    if (!QDir().rename(legacy, staging)) return;

    const QFileInfoList entries = QDir(staging).entryInfoList(
        QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    for (const QFileInfo &entry : entries) {
        const QString target = base + QLatin1Char('/') + entry.fileName();
        if (QFileInfo::exists(target)) continue; // 两边都有时保留新目录里的
        QDir().rename(entry.absoluteFilePath(), target);
    }

    // 只在搬空时删除；有冲突留下时数据仍在临时目录里，不会丢
    QDir().rmdir(staging);
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

    // 不用 AppConfigLocation：它会把组织名与应用名拼成 <config>/<org>/<app>，
    // 两者都叫 clash-qt 时就会得到 ~/.config/clash-qt/clash-qt 这种多余的一层。
    // 这里只取通用配置根目录，再拼一次应用目录名，与 AppConfigLocation 无关。
    QString base = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (base.isEmpty()) {
        // 兜底路径按平台给：Windows 的通用配置根是 %LOCALAPPDATA%，
        // Linux 是 $XDG_CONFIG_HOME（默认 ~/.config）
#ifdef Q_OS_WIN
        base = QDir::homePath() + QStringLiteral("/AppData/Local");
#else
        base = QDir::homePath() + QStringLiteral("/.config");
#endif
    }
    base += QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME);

    // 迁移必须早于任何子目录的创建：否则 profiles/、rules/ 这类空目录会先
    // 在 base 下占位，迁移时被判为「目标已存在」而跳过，用户数据就搬不过来。
    migrateNestedDataDir(base);

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
    return logDir() + QLatin1Char('/') + QStringLiteral(CLASHQT_APP_NAME)
           + QLatin1Char('-') + dateStamp() + QStringLiteral(".log");
}

QString coreLogPath()
{
    return logDir() + QStringLiteral("/core-") + dateStamp() + QStringLiteral(".log");
}

} // namespace paths
} // namespace clashqt
