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
#include <QPushButton>
#include <QVBoxLayout>

namespace clashqt {
namespace {

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
    if (!isVisible() || !shouldShow(level)) return;

    m_view->appendPlainText(QStringLiteral("[%1] [%2] [%3] %4")
                                .arg(time, level.toUpper(), category, message));

    // 未暂停时自动滚到底部
    if (!m_pauseScroll->isChecked()) {
        QScrollBar *bar = m_view->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}

void LogsPage::onFilterChanged()
{
    // 切换等级后清空显示，避免「过去不显示、现在显示」的混排难以阅读。
    // 完整历史仍在日志文件中。
    m_view->clear();
}

void LogsPage::onClear() { m_view->clear(); }

} // namespace clashqt
