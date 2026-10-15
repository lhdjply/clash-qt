#pragma once

// ---------------------------------------------------------------------------
//  应用日志
//
//  对应 clash-party 的 src/main/utils/logger.ts 与 logFile.ts。
//  职责：
//    * 把内核日志（core::Logger 的输出）落盘到 logs/core-YYYY-MM-DD.log
//    * 应用自身日志落盘到 logs/clash-qt-YYYY-MM-DD.log
//    * 通过信号把日志同时推送给界面（日志页）
//    * 按「保留天数」清理历史日志文件
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>

#include "constant/Constants.h"

namespace clashqt {

class AppLogger : public QObject {
    Q_OBJECT

public:
    static AppLogger &instance();

    // 安装内核日志接收器；应在启动早期调用一次
    void install();

    // 设置内核日志等级（同时影响内核内部过滤与界面过滤）
    void setLevel(core::LogLevel level);
    core::LogLevel level() const { return m_level; }

    // 是否禁用日志落盘（对应配置项 disableAppLog / disableCoreLog）
    void setAppLogDisabled(bool disabled) { m_appLogDisabled = disabled; }
    void setCoreLogDisabled(bool disabled) { m_coreLogDisabled = disabled; }

    // 单个日志文件最大体积（MB）与保留天数；超出时滚动
    void setMaxFileSizeMB(int megabytes);
    void setMaxLogDays(int days);

    // 清理超过保留天数的日志文件
    void purgeOldLogs();

    void info(const QString &message);
    void warn(const QString &message);
    void error(const QString &message);
    void debug(const QString &message);

signals:
    // 界面订阅：等级、分类名、时间文本、正文
    void logEmitted(const QString &level, const QString &category, const QString &time,
                    const QString &message);

private:
    AppLogger() = default;
    ~AppLogger() override = default;

    void writeToFile(const QString &level, const QString &category, const QString &message,
                     bool coreLog);
    void emitLog(const QString &level, const QString &category, const QString &message);
    // 超过体积上限时把当前日志改名归档
    void rotateIfNeeded(const QString &path);

    bool             m_installed      = false;
    bool             m_appLogDisabled  = false;
    bool             m_coreLogDisabled = false;
    int              m_maxFileSizeMB   = 10;
    int              m_maxLogDays      = 7;
    core::LogLevel m_level           = core::LogLevel::Info;
};

} // namespace clashqt
