#include "ui/pages/LogsPage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QShowEvent>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

namespace clashqt {
namespace {

// 内存缓冲条数上限。视图本身限 5000 行，缓冲略小于它，保证重建后
// 不会一次塞进超过视图上限的内容。
constexpr int kMaxEntries = 4000;
// 超出上限时一次裁掉这么多条，避免每来一行都做一次整体搬移
constexpr int kTrimChunk = 512;
// 高频日志的合并窗口：这段时间内的行合并成一次文本插入
constexpr int kFlushIntervalMs = 80;

// 等级权重，用于过滤比较
int levelWeight(const QString &level)
{
    if (level == QLatin1String("debug")) return 0;
    if (level == QLatin1String("info")) return 1;
    if (level == QLatin1String("warning")) return 2;
    if (level == QLatin1String("error")) return 3;
    return 1;
}

} // namespace

LogsPage::LogsPage(AppContext *context, QWidget *parent) : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("日志"),
        QStringLiteral("应用运行日志。日志同时写入数据目录下的 logs/ 文件。")));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    toolbar->addWidget(new QLabel(QStringLiteral("等级"), this));
    m_levelFilter = new QComboBox(this);
    m_levelFilter->addItems({QStringLiteral("debug"), QStringLiteral("info"),
                             QStringLiteral("warning"), QStringLiteral("error")});
    m_levelFilter->setCurrentText(QStringLiteral("info"));
    toolbar->addWidget(m_levelFilter);

    m_pauseScroll = new QCheckBox(QStringLiteral("暂停滚动"), this);
    toolbar->addWidget(m_pauseScroll);

    m_clearButton = new QPushButton(QStringLiteral("清空显示"), this);
    ui::decorate(m_clearButton, QStringLiteral("eraser"));
    toolbar->addWidget(m_clearButton);
    toolbar->addStretch(1);

    root->addLayout(toolbar);

    auto *card = ui::makeCard(QString(), this);
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_view = new QPlainTextEdit(card);
    m_view->setReadOnly(true);
    m_view->setMaximumBlockCount(5000); // 限制缓冲，避免长时间运行内存增长
    m_view->setLineWrapMode(QPlainTextEdit::NoWrap);
    cardLayout->addWidget(m_view);
    root->addWidget(card, 1);

    QObject::connect(m_levelFilter, &QComboBox::currentTextChanged, this,
                     &LogsPage::onFilterChanged);
    QObject::connect(m_clearButton, &QPushButton::clicked, this, &LogsPage::onClear);

    m_flushTimer.setSingleShot(true);
    m_flushTimer.setInterval(kFlushIntervalMs);
    QObject::connect(&m_flushTimer, &QTimer::timeout, this, &LogsPage::flushPending);

    QObject::connect(&AppLogger::instance(), &AppLogger::logEmitted, this,
                     &LogsPage::onLogEmitted);
}

bool LogsPage::shouldShow(const QString &level) const
{
    return levelWeight(level) >= levelWeight(m_levelFilter->currentText());
}

void LogsPage::onLogEmitted(const QString &level, const QString &category, const QString &time,
                            const QString &message)
{
    const QString text =
        QStringLiteral("[%1] [%2] [%3] %4").arg(time, level.toUpper(), category, message);

    // 缓冲与可见性无关：日志页不在前台时内核日志照样在刷，
    // 只有把这段窗口期的日志留住，切回日志页时才不会是空的。
    m_entries.append({level, text});
    if (m_entries.size() > kMaxEntries) {
        m_entries.remove(0, kTrimChunk);
        m_dirty = true; // 视图头部已与缓冲不一致，下次重建
    }

    if (!shouldShow(level)) return;

    if (!isVisible()) {
        m_dirty = true;
        return;
    }

    // 高频日志合并成一次插入：每条 appendPlainText 都会触发一次排版与滚动，
    // 开启 TUN 时每秒数百行会把界面拖慢。
    m_pending << text;
    if (!m_flushTimer.isActive()) m_flushTimer.start();
}

void LogsPage::flushPending()
{
    if (m_pending.isEmpty()) return;

    const QStringList lines = m_pending;
    m_pending.clear();

    if (!isVisible()) {
        // 等待期间页面被切走：留着下次重建时一起补
        m_dirty = true;
        return;
    }

    appendLines(lines);
}

void LogsPage::appendLines(const QStringList &lines)
{
    if (lines.isEmpty()) return;

    m_view->appendPlainText(lines.join(QLatin1Char('\n')));

    // 未暂停时自动滚到底部
    if (!m_pauseScroll->isChecked()) {
        QScrollBar *bar = m_view->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}

void LogsPage::rebuildView()
{
    QStringList lines;
    lines.reserve(m_entries.size());
    for (const Entry &entry : m_entries) {
        if (shouldShow(entry.level)) lines << entry.text;
    }

    m_view->setPlainText(lines.join(QLatin1Char('\n')));

    if (!m_pauseScroll->isChecked()) {
        QScrollBar *bar = m_view->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}

void LogsPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (!m_dirty) return;
    m_pending.clear();
    rebuildView();
    m_dirty = false;
}

void LogsPage::onFilterChanged()
{
    // 等级变了就把缓冲按新过滤条件重排，而不是清空：清空后用户切回
    // info 也看不到刚才的日志，等于把缓冲白留了。完整历史仍在日志文件里。
    m_pending.clear();
    m_flushTimer.stop();
    rebuildView();
    m_dirty = false;
}

void LogsPage::onClear()
{
    m_pending.clear();
    m_flushTimer.stop();
    m_entries.clear();
    m_view->clear();
    m_dirty = false;
}

} // namespace clashqt
