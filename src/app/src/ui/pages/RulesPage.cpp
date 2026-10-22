#include "ui/pages/RulesPage.h"

#include "AppContext.h"
#include "ui/RuleList.h"
#include "ui/UiFactory.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QListView>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {
namespace {

// DOMAIN-SUFFIX 之类的规则类型在界面上写成 DomainSuffix
QString prettyType(const QString &type)
{
    QString result;
    const QStringList parts = type.split(QLatin1Char('-'), Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        result += part.left(1).toUpper();
        result += part.mid(1).toLower();
    }
    return result.isEmpty() ? type : result;
}

} // namespace

RulesPage::RulesPage(AppContext *context, QWidget *parent) : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Rules"),
        tr("Rules in match order, one card each. Turn the switch on the right off to disable a rule for this session; it is restored on restart."),
        QStringLiteral("rules")));

    m_model    = new RuleListModel(this);
    m_delegate = new RuleRowDelegate(this);

    m_list = new QListView(this);
    m_list->setModel(m_model);
    m_list->setItemDelegate(m_delegate);
    m_list->setFrameShape(QFrame::NoFrame);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_list->setUniformItemSizes(true);
    m_list->setMouseTracking(true);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 卡片自己画底，视图保持透明
    m_list->setStyleSheet(
        QStringLiteral("QListView { background: transparent; border: 0px; outline: 0px; }"));

    m_empty = new QLabel(tr("No rules loaded"), this);
    m_empty->setObjectName(QStringLiteral("hintLabel"));
    m_empty->setAlignment(Qt::AlignCenter);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_empty);
    m_stack->addWidget(m_list);
    root->addWidget(m_stack, 1);

    // 命中次数随时在变，页面可见时每秒跟一次
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    QObject::connect(m_timer, &QTimer::timeout, this, &RulesPage::refresh);
    m_timer->start();

    m_delegate->toggled = [this](int row) {
        const RuleRow &rule = m_model->rowAt(row);
        if (rule.ruleText.isEmpty()) return;
        // 拨一下就是翻转：停用的重新启用，正在用的临时停用
        applyRuleDisabled(rule.ruleText, !rule.disabled);
    };
}

void RulesPage::applyRuleDisabled(const QString &ruleText, bool disabled)
{
    if (!m_context->hub()->setRuleDisabled(ruleText, disabled)) {
        m_context->showToast(QStringLiteral("warning"), tr("Failed to update rule state"));
        return;
    }

    refresh();
}

void RulesPage::refresh()
{
    if (!isVisible()) return;

    const QJsonArray rules =
        m_context->hub()->rulesStatus().value(QStringLiteral("rules")).toArray();

    QVector<RuleRow> rows;
    rows.reserve(rules.size());

    qint64 totalHits = 0;

    for (const QJsonValue &value : rules) {
        const QJsonObject rule     = value.toObject();
        const QJsonObject extra    = rule.value(QStringLiteral("extra")).toObject();
        const QString     type     = rule.value(QStringLiteral("type")).toString();
        const QString     payload  = rule.value(QStringLiteral("payload")).toString();
        const QString     proxy    = rule.value(QStringLiteral("proxy")).toString();
        const bool        disabled = extra.value(QStringLiteral("disabled")).toBool();

        RuleRow row;
        row.type     = prettyType(type);
        row.proxy    = proxy;
        row.disabled = disabled;
        row.title    = payload.isEmpty() ? row.type : payload;
        row.hits     = qint64(extra.value(QStringLiteral("hitCount")).toDouble());

        // 停用接口按规则描述文本定位，格式与内核 Rule::description() 一致
        row.ruleText = payload.isEmpty() ? QStringLiteral("%1,%2").arg(type, proxy)
                                         : QStringLiteral("%1,%2,%3").arg(type, payload, proxy);

        QStringList lines;
        lines << QStringLiteral("#%1  %2")
                     .arg(rule.value(QStringLiteral("index")).toInt() + 1)
                     .arg(row.ruleText);
        lines << tr("%1 hits in this session").arg(row.hits);
        if (disabled) lines << tr("Disabled until restart");
        row.tooltip = lines.join(QLatin1Char('\n'));

        totalHits += row.hits;
        rows.append(row);
    }

    // 每条的占比要等总数出来才算得出来
    if (totalHits > 0) {
        for (RuleRow &row : rows) row.share = double(row.hits) / double(totalHits);
    }

    // 列表重建后把滚动位置还回去，免得每秒都跳回顶部
    const int scrollTop = m_list->verticalScrollBar()->value();
    m_model->setRows(std::move(rows));
    m_list->verticalScrollBar()->setValue(scrollTop);

    m_stack->setCurrentWidget(m_model->rowCount() > 0 ? static_cast<QWidget *>(m_list)
                                                      : static_cast<QWidget *>(m_empty));
}

} // namespace clashqt
