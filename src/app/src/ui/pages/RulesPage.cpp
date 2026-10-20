#include "ui/pages/RulesPage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {

RulesPage::RulesPage(AppContext *context, QWidget *parent) : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Rules"),
        tr("Active rules in match order. Double-click the \"Enabled\" column to disable a rule temporarily; this only affects the running session and is reset on restart."), QStringLiteral("rules")));

    auto *card = ui::makeCard(QString(), this);
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_table = new QTableWidget(card);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({QStringLiteral("#"), tr("Type"),
                                        tr("Value"), tr("Outbound"),
                                        tr("Enabled")});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    cardLayout->addWidget(m_table);
    root->addWidget(card, 1);

    QObject::connect(m_table, &QTableWidget::cellDoubleClicked, this, &RulesPage::onToggleRule);
}

void RulesPage::refresh()
{
    const QJsonArray rules =
        m_context->hub()->rulesStatus().value(QStringLiteral("rules")).toArray();

    m_table->setRowCount(rules.size());
    m_ruleTexts.clear();
    m_ruleTexts.reserve(rules.size());

    for (int row = 0; row < rules.size(); ++row) {
        const QJsonObject rule = rules.at(row).toObject();

        const QString type    = rule.value(QStringLiteral("type")).toString();
        const QString payload = rule.value(QStringLiteral("payload")).toString();
        const QString proxy   = rule.value(QStringLiteral("proxy")).toString();
        const bool disabled   = rule.value(QStringLiteral("extra")).toObject()
                                    .value(QStringLiteral("disabled")).toBool();

        m_ruleTexts << (payload.isEmpty() ? QStringLiteral("%1,%2").arg(type, proxy)
                                          : QStringLiteral("%1,%2,%3").arg(type, payload, proxy));

        m_table->setItem(row, 0,
                         new QTableWidgetItem(QString::number(
                             rule.value(QStringLiteral("index")).toInt())));
        m_table->setItem(row, 1, new QTableWidgetItem(type));
        m_table->setItem(row, 2, new QTableWidgetItem(payload.isEmpty() ? QStringLiteral("-")
                                                                        : payload));
        m_table->setItem(row, 3, new QTableWidgetItem(proxy));
        m_table->setItem(row, 4, new QTableWidgetItem(disabled ? tr("No")
                                                               : tr("Yes")));
        if (disabled) {
            for (int column = 0; column < 5; ++column) {
                if (QTableWidgetItem *item = m_table->item(row, column)) {
                    item->setForeground(QColor(0x9a, 0xa0, 0xa6));
                }
            }
        }
    }
}

void RulesPage::onToggleRule(int row, int column)
{
    // 仅「启用」列（第 4 列）响应双击切换
    if (row < 0 || row >= m_ruleTexts.size() || column != 4) return;

    const QJsonArray rules =
        m_context->hub()->rulesStatus().value(QStringLiteral("rules")).toArray();
    if (row >= rules.size()) return;

    const QJsonObject rule = rules.at(row).toObject();
    const bool disabled = rule.value(QStringLiteral("extra")).toObject()
                              .value(QStringLiteral("disabled")).toBool();

    // 禁用语义：把规则置为 disabled，使其在匹配时被跳过
    if (!m_context->hub()->setRuleDisabled(m_ruleTexts.at(row), !disabled)) {
        m_context->showToast(QStringLiteral("warning"), tr("Failed to update rule state"));
        return;
    }

    refresh();
}

} // namespace clashqt
