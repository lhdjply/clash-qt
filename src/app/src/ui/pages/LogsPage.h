#pragma once

// ---------------------------------------------------------------------------
//  日志页
//  对应 clash-party 的 src/renderer/src/pages/logs.tsx。
//  实时显示应用与内核日志，支持按等级过滤与暂停滚动。
// ---------------------------------------------------------------------------

#include <QWidget>

class QCheckBox;
class QComboBox;
class QPlainTextEdit;
class QPushButton;

namespace clashqt {

class AppContext;

class LogsPage : public QWidget {
    Q_OBJECT

public:
    explicit LogsPage(AppContext *context, QWidget *parent = nullptr);

private slots:
    void onLogEmitted(const QString &level, const QString &category, const QString &time,
                      const QString &message);
    void onFilterChanged();
    void onClear();

private:
    // 判断该条日志是否应显示（等级过滤）
    bool shouldShow(const QString &level) const;

    AppContext      *m_context = nullptr;
    QPlainTextEdit  *m_view = nullptr;
    QComboBox       *m_levelFilter = nullptr;
    QCheckBox       *m_pauseScroll = nullptr;
    QPushButton     *m_clearButton = nullptr;

    int m_lineCount = 0;
};

} // namespace clashqt
