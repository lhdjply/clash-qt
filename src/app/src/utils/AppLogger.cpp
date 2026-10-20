#include "utils/AppLogger.h"
#include <QCoreApplication>

#include "utils/Paths.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <atomic>

#include "base/Common.h"

namespace clashqt {
namespace {

QString timestamp()
{
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
}

} // namespace

AppLogger &AppLogger::instance()
{
    static AppLogger logger;
    return logger;
}

void AppLogger::install()
{
    if (m_installed) return;
    m_installed = true;

    core::Logger::instance().setLevel(m_level);

    // 内核日志：先写文件，再推给界面。
    // 用 sink 而不是信号是为了避免高频日志（每条连接一条 info）在队列中堆积。
    core::Logger::instance().setSink([this](core::LogCategory category, const QString &level,
                                              const QString &message) {
        const QString categoryName = core::logCategoryName(category);
        writeToFile(level, categoryName, message, /*coreLog=*/true);
        emitLog(level, categoryName, message);
    });

    purgeOldLogs();
}

void AppLogger::setLevel(core::LogLevel level)
{
    m_level = level;
    core::Logger::instance().setLevel(level);
}

void AppLogger::setMaxFileSizeMB(int megabytes)
{
    m_maxFileSizeMB = megabytes > 0 ? megabytes : 10;
}

void AppLogger::setMaxLogDays(int days)
{
    m_maxLogDays = days > 0 ? days : 7;
}

void AppLogger::rotateIfNeeded(const QString &path)
{
    const qint64 limit = qint64(m_maxFileSizeMB) * 1024 * 1024;
    if (limit <= 0) return;

    QFileInfo info(path);
    if (!info.exists() || info.size() < limit) return;

    // 归档为 <原名>.<时间戳>.bak，让当前文件从空开始，
    // 避免日志无限增长把磁盘占满。
    const QString archive = QStringLiteral("%1.%2.bak")
                                .arg(path, QDateTime::currentDateTime().toString(
                                               QStringLiteral("yyyyMMddHHmmss")));
    QFile::rename(path, archive);
}

void AppLogger::writeToFile(const QString &level, const QString &category, const QString &message,
                            bool coreLog)
{
    if (coreLog && m_coreLogDisabled) return;

    const QString path = coreLog ? paths::coreLogPath() : paths::appLogPath();
    paths::ensureParentDir(path);

    // 每 512KB 检查一次体积即可，避免每条日志都 stat 文件。
    // 计数必须是原子的：日志会从 TUN 数据面线程、每个连接的拨号线程同时写入，
    // 普通 static int 的 ++ 在这里是数据竞争（UB）。
    static std::atomic<int> writeCounter{0};
    if (++writeCounter % 512 == 0) rotateIfNeeded(path);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) return;

    file.write(QStringLiteral("%1 [%2] [%3] %4\n")
                   .arg(timestamp(), level.toUpper(), category, message)
                   .toUtf8());
}

void AppLogger::emitLog(const QString &level, const QString &category, const QString &message)
{
    emit logEmitted(level, category, timestamp(), message);
}

void AppLogger::info(const QString &message)
{
    writeToFile(QStringLiteral("info"), QStringLiteral("App"), message, /*coreLog=*/false);
    emitLog(QStringLiteral("info"), QStringLiteral("App"), message);
}

void AppLogger::warn(const QString &message)
{
    writeToFile(QStringLiteral("warning"), QStringLiteral("App"), message, false);
    emitLog(QStringLiteral("warning"), QStringLiteral("App"), message);
}

void AppLogger::error(const QString &message)
{
    writeToFile(QStringLiteral("error"), QStringLiteral("App"), message, false);
    emitLog(QStringLiteral("error"), QStringLiteral("App"), message);
}

void AppLogger::debug(const QString &message)
{
    if (m_level != core::LogLevel::Debug) return;
    writeToFile(QStringLiteral("debug"), QStringLiteral("App"), message, false);
    emitLog(QStringLiteral("debug"), QStringLiteral("App"), message);
}

void AppLogger::purgeOldLogs()
{
    QDir dir(paths::logDir());
    if (!dir.exists()) return;

    const QDateTime cutoff = QDateTime::currentDateTime().addDays(-m_maxLogDays);
    const QFileInfoList files = dir.entryInfoList({QStringLiteral("*.log"), QStringLiteral("*.bak")},
                                                 QDir::Files);

    int removed = 0;
    for (const QFileInfo &info : files) {
        if (info.lastModified() < cutoff) {
            if (QFile::remove(info.absoluteFilePath())) ++removed;
        }
    }

    if (removed > 0) {
        writeToFile(QStringLiteral("info"), QStringLiteral("App"),
                    QCoreApplication::translate("clashqt::app", "Removed %1 expired log file(s) (retention %2 days)")
                        .arg(removed)
                        .arg(m_maxLogDays),
                    false);
    }
}

} // namespace clashqt
