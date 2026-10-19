#pragma once

// ---------------------------------------------------------------------------
//  日志页
//  对应 clash-party 的 src/renderer/src/pages/logs.tsx。
//  实时显示应用与内核日志，支持按等级过滤与暂停滚动。
// ---------------------------------------------------------------------------

#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QPlainTextEdit;
class QPushButton;
class QShowEvent;

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

protected:
    // 页面重新可见时把隐藏期间积攒的日志一次性补进视图
    void showEvent(QShowEvent *event) override;

private:
    // 一条已格式化的日志：等级用于过滤，正文用于渲染
    struct Entry {
        QString level;
        QString text;
    };

    // 判断该条日志是否应显示（等级过滤）
    bool shouldShow(const QString &level) const;

    // 追加一批文本并按需滚到底部
    void appendLines(const QStringList &lines);
    // 用内存缓冲重建整个视图（显示隐藏期间的行、或切换过滤等级后重排）
    void rebuildView();
    // 把积攒的待显示行刷进视图
    void flushPending();

    AppContext      *m_context = nullptr;
    QPlainTextEdit  *m_view = nullptr;
    QComboBox       *m_levelFilter = nullptr;
    QCheckBox       *m_pauseScroll = nullptr;
    QPushButton     *m_clearButton = nullptr;

    // 视图只保存最近若干行，但内存缓冲必须独立存在：日志页不可见时
    // 内核仍在刷日志，若直接丢弃，用户切到日志页就只能看到「之后」的日志。
    QVector<Entry>  m_entries;
    // 待显示的行（合并高频日志，避免每条一次排版）
    QStringList     m_pending;
    QTimer          m_flushTimer;
    // 缓冲与视图不一致（隐藏期间新增、或头部被裁掉）时需要整屏重建
    bool            m_dirty = true;
};

} // namespace clashqt
