#include "ui/pages/ConnectionsPage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {

ConnectionsPage::ConnectionsPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("连接"),
        QStringLiteral("当前活动的连接。双击某一行可关闭该连接。")));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    m_summary = ui::makeHintLabel(QString(), this);
    toolbar->addWidget(m_summary, 1);

    m_autoRefresh = new QCheckBox(QStringLiteral("自动刷新（1 秒）"), this);
    m_autoRefresh->setChecked(true);
    toolbar->addWidget(m_autoRefresh);

    auto *closeSelected = new QPushButton(QStringLiteral("关闭选中"), this);
    auto *closeAll      = ui::makeDangerButton(QStringLiteral("关闭全部"), this);
    toolbar->addWidget(closeSelected);
    toolbar->addWidget(closeAll);

    root->addLayout(toolbar);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels({QStringLiteral("主机"), QStringLiteral("规则"),
                                        QStringLiteral("出站链"), QStringLiteral("网络"),
                                        QStringLiteral("上传"), QStringLiteral("下载"),
                                        QStringLiteral("↑速度"), QStringLiteral("↓速度")});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    root->addWidget(m_table, 1);

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(1000);
    QObject::connect(m_refreshTimer, &QTimer::timeout, this, &ConnectionsPage::refresh);
    m_refreshTimer->start();

    QObject::connect(m_autoRefresh, &QCheckBox::toggled, this,
                     &ConnectionsPage::onAutoRefreshToggled);
    QObject::connect(closeSelected, &QPushButton::clicked, this, &ConnectionsPage::onCloseSelected);
    QObject::connect(closeAll, &QPushButton::clicked, this, &ConnectionsPage::onCloseAll);
    QObject::connect(m_table, &QTableWidget::cellDoubleClicked, this,
                     [this](int, int) { onCloseSelected(); });
}

void ConnectionsPage::onAutoRefreshToggled(bool enabled)
{
    if (enabled) m_refreshTimer->start();
    else m_refreshTimer->stop();
}

void ConnectionsPage::refresh()
{
    const QJsonObject status = m_context->hub()->connectionsStatus();
    const QJsonArray   connections = status.value(QStringLiteral("connections")).toArray();

    m_summary->setText(
        QStringLiteral("活动连接 %1 · 累计上传 %2 · 累计下载 %3")
            .arg(connections.size())
            .arg(calc::traffic(qint64(status.value(QStringLiteral("uploadTotal")).toDouble())))
            .arg(calc::traffic(qint64(status.value(QStringLiteral("downloadTotal")).toDouble()))));

    // 仅在列表可见时刷新，避免不可见页面持续做拷贝
    if (!isVisible()) return;

    m_table->setRowCount(connections.size());
    m_connectionIds.clear();
    m_connectionIds.reserve(connections.size());

    for (int row = 0; row < connections.size(); ++row) {
        const QJsonObject connection = connections.at(row).toObject();
        const QJsonObject metadata  = connection.value(QStringLiteral("metadata")).toObject();

        const QString host =
            metadata.value(QStringLiteral("host")).toString().isEmpty()
                ? metadata.value(QStringLiteral("destinationIP")).toString()
                : metadata.value(QStringLiteral("host")).toString();
        const QString port    = metadata.value(QStringLiteral("destinationPort")).toString();
        QString chainText;
        for (const QJsonValue &value : connection.value(QStringLiteral("chains")).toArray()) {
            if (!chainText.isEmpty()) chainText += QStringLiteral(" → ");
            chainText += value.toString();
        }

        m_connectionIds << connection.value(QStringLiteral("id")).toString();

        const QString target = port.isEmpty() ? host
                                              : QStringLiteral("%1:%2").arg(host, port);

        m_table->setItem(row, 0, new QTableWidgetItem(target));
        m_table->setItem(row, 1,
                         new QTableWidgetItem(QStringLiteral("%1 %2")
                                                  .arg(connection.value(QStringLiteral("rule")).toString(),
                                                       connection.value(QStringLiteral("rulePayload")).toString())));
        m_table->setItem(row, 2, new QTableWidgetItem(chainText));
        m_table->setItem(row, 3,
                         new QTableWidgetItem(metadata.value(QStringLiteral("network")).toString().toUpper()));
        m_table->setItem(row, 4, new QTableWidgetItem(calc::traffic(
                                     qint64(connection.value(QStringLiteral("upload")).toDouble()))));
        m_table->setItem(row, 5, new QTableWidgetItem(calc::traffic(
                                     qint64(connection.value(QStringLiteral("download")).toDouble()))));
        m_table->setItem(row, 6, new QTableWidgetItem(calc::speed(
                                     qint64(connection.value(QStringLiteral("uploadSpeed")).toDouble()))));
        m_table->setItem(row, 7, new QTableWidgetItem(calc::speed(
                                     qint64(connection.value(QStringLiteral("downloadSpeed")).toDouble()))));
    }
}

void ConnectionsPage::onCloseSelected()
{
    const int row = m_table->currentRow();
    if (row < 0 || row >= m_connectionIds.size()) {
        m_context->showToast(QStringLiteral("warning"), QStringLiteral("请先选择一条连接"));
        return;
    }
    const QString id = m_connectionIds.at(row);
    if (id.isEmpty()) return;

    m_context->hub()->closeConnection(id);
    refresh();
}

void ConnectionsPage::onCloseAll()
{
    const int count = m_context->hub()->closeAllConnections();
    m_context->showToast(QStringLiteral("info"), QStringLiteral("已关闭 %1 条连接").arg(count));
    refresh();
}

} // namespace clashqt
