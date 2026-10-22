#include "utils/Calc.h"
#include <QCoreApplication>

#include <QtMath>

namespace clashqt {
namespace calc {
namespace {

// 与参照实现的 formatNumString 保持一致：
//  ≤5 字符保留 2 位小数；6 字符保留 1 位；更长直接取整
QString formatNumber(double value)
{
    QString text = QString::number(value, 'f', 2);
    if (text.size() <= 5) return text;
    if (text.size() == 6) return QString::number(value, 'f', 1);
    return QString::number(qRound(value));
}

QString scale(qint64 bytes, const QString &suffix)
{
    static const char *kUnits[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

    double value = double(bytes);
    int    unit  = 0;
    while (value >= 1024.0 && unit < 8) {
        value /= 1024.0;
        ++unit;
    }

    const QString unitText = QString::fromLatin1(kUnits[unit]);
    if (unit == 0) {
        // 字节不显示小数，与原实现 "N B" 一致
        return QStringLiteral("%1 %2%3").arg(qint64(value)).arg(unitText, suffix);
    }
    return QStringLiteral("%1 %2%3").arg(formatNumber(value), unitText, suffix);
}

} // namespace

QString traffic(qint64 bytes) { return scale(bytes, QString()); }

SpeedUnit speedUnit(double bytesPerSecond)
{
    static const char *kUnits[] = {"B/s", "KB/s", "MB/s", "GB/s", "TB/s"};

    double value = bytesPerSecond;
    int    unit  = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        ++unit;
    }

    double divisor = 1.0;
    for (int i = 0; i < unit; ++i) divisor *= 1024.0;

    return SpeedUnit{divisor, QString::fromLatin1(kUnits[unit])};
}

QString speed(qint64 bytesPerSecond)
{
    return scale(bytesPerSecond, QStringLiteral("/s"));
}

QString timeLeft(double usedBytes, double totalBytes, double bytesPerSecond)
{
    if (bytesPerSecond <= 0) return QStringLiteral("--");

    const double remaining = totalBytes - usedBytes;
    if (remaining <= 0) return QCoreApplication::translate("clashqt::app", "Used up");

    const qint64 seconds = qint64(remaining / bytesPerSecond);
    const qint64 days    = seconds / 86400;
    const qint64 hours   = (seconds % 86400) / 3600;
    const qint64 minutes = (seconds % 3600) / 60;

    if (days > 0) return QCoreApplication::translate("clashqt::app", "%1 d %2 h").arg(days).arg(hours);
    if (hours > 0) return QCoreApplication::translate("clashqt::app", "%1 h %2 min").arg(hours).arg(minutes);
    return QCoreApplication::translate("clashqt::app", "%1 min").arg(minutes);
}

QString number(double value) { return formatNumber(value); }

} // namespace calc
} // namespace clashqt
