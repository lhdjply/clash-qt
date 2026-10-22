#include "ui/pages/ConnectionsPage.h"

#include "AppContext.h"
#include "ui/ConnectionList.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QAbstractSocket>
#include <QCheckBox>
#include <QDateTime>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

#include "hub/Hub.h"

namespace clashqt {

namespace {

// 内核给的地址常见 ::ffff:127.0.0.1 这种 IPv4 映射形式，显示时还原成点分
QString plainAddress(const QString &address)
{
    QHostAddress parsed;
    if (!parsed.setAddress(address) || parsed.protocol() != QAbstractSocket::IPv6Protocol)
        return address;

    const QIPv6Address v6 = parsed.toIPv6Address();
    for (int i = 0; i < 10; ++i)
        if (v6[i] != 0) return address;
    if (v6[10] != 0xFF || v6[11] != 0xFF) return address;

    const quint32 ip = parsed.toIPv4Address();
    return QStringLiteral("%1.%2.%3.%4")
        .arg(ip >> 24)
        .arg((ip >> 16) & 0xFF)
        .arg((ip >> 8) & 0xFF)
        .arg(ip & 0xFF);
}

} // namespace

ConnectionsPage::ConnectionsPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Connections"),
        tr("Active connections, one card each. Double-click a card, or use the × on its right, to close that connection."),
        QStringLiteral("connections")));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    m_summary = ui::makeHintLabel(QString(), this);
    toolbar->addWidget(m_summary, 1);

    m_autoRefresh = new QCheckBox(tr("Auto refresh (1 s)"), this);
    m_autoRefresh->setChecked(true);
    toolbar->addWidget(m_autoRefresh);

    auto *closeSelected = new QPushButton(tr("Close selected"), this);
    auto *closeAll      = ui::makeDangerButton(tr("Close all"), this);

    ui::decorate(closeSelected, QStringLiteral("close"));
    ui::decorate(closeAll, QStringLiteral("trash"));
    toolbar->addWidget(closeSelected);
    toolbar->addWidget(closeAll);

    root->addLayout(toolbar);

    m_model    = new ConnectionListModel(this);
    m_delegate = new ConnectionRowDelegate(this);

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
    // 卡片自己画底，视图保持透明；只作用于本视图，免得连带下拉框的弹层
    m_list->setStyleSheet(
        QStringLiteral("QListView { background: transparent; border: 0px; outline: 0px; }"));

    m_empty = new QLabel(tr("No active connections"), this);
    m_empty->setObjectName(QStringLiteral("hintLabel"));
    m_empty->setAlignment(Qt::AlignCenter);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_empty);
    m_stack->addWidget(m_list);
    root->addWidget(m_stack, 1);

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(1000);
    QObject::connect(m_refreshTimer, &QTimer::timeout, this, &ConnectionsPage::refresh);
    m_refreshTimer->start();

    m_delegate->closeRequested = [this](const QString &id) {
        if (id.isEmpty()) return;
        m_context->hub()->closeConnection(id);
        refresh();
    };

    QObject::connect(m_autoRefresh, &QCheckBox::toggled, this,
                     &ConnectionsPage::onAutoRefreshToggled);
    QObject::connect(closeSelected, &QPushButton::clicked, this, &ConnectionsPage::onCloseSelected);
    QObject::connect(closeAll, &QPushButton::clicked, this, &ConnectionsPage::onCloseAll);
    QObject::connect(m_list, &QListView::doubleClicked, this,
                     [this](const QModelIndex &) { onCloseSelected(); });
}

void ConnectionsPage::onAutoRefreshToggled(bool enabled)
{
    if (enabled) m_refreshTimer->start();
    else m_refreshTimer->stop();
}

QString ConnectionsPage::elapsedText(const QString &start) const
{
    QDateTime started = QDateTime::fromString(start, Qt::ISODateWithMs);
    if (!started.isValid()) started = QDateTime::fromString(start, Qt::ISODate);
    if (!started.isValid()) return QString();

    const qint64 seconds = started.secsTo(QDateTime::currentDateTime());
    if (seconds < 60) return tr("just now");
    if (seconds < 3600) return tr("%1 min ago").arg(seconds / 60);
    if (seconds < 86400) return tr("%1 h ago").arg(seconds / 3600);
    return tr("%1 d ago").arg(seconds / 86400);
}

void ConnectionsPage::refresh()
{
    const QJsonObject status = m_context->hub()->connectionsStatus();
    const QJsonArray   connections = status.value(QStringLiteral("connections")).toArray();

    m_summary->setText(
        tr("%1 active connection(s) · Total up %2 · Total down %3")
            .arg(connections.size())
            .arg(calc::traffic(qint64(status.value(QStringLiteral("uploadTotal")).toDouble())))
            .arg(calc::traffic(qint64(status.value(QStringLiteral("downloadTotal")).toDouble()))));

    // 仅在列表可见时刷新，避免不可见页面持续做拷贝
    if (!isVisible()) return;

    QVector<ConnectionRow> rows;
    rows.reserve(connections.size());

    for (const QJsonValue &value : connections) {
        const QJsonObject connection = value.toObject();
        const QJsonObject metadata   = connection.value(QStringLiteral("metadata")).toObject();

        ConnectionRow row;
        row.id = connection.value(QStringLiteral("id")).toString();

        // 进程名优先；没有就退回可执行文件名，再退回来源地址
        row.process = metadata.value(QStringLiteral("process")).toString();
        if (row.process.isEmpty()) {
            const QString path = metadata.value(QStringLiteral("processPath")).toString();
            if (!path.isEmpty()) row.process = QFileInfo(path).fileName();
        }
        if (row.process.isEmpty())
            row.process = plainAddress(metadata.value(QStringLiteral("sourceIP")).toString());

        const QString host = metadata.value(QStringLiteral("host")).toString();
        row.host = plainAddress(host.isEmpty()
                                    ? metadata.value(QStringLiteral("destinationIP")).toString()
                                    : host);
        row.port      = metadata.value(QStringLiteral("destinationPort")).toString();
        row.type      = metadata.value(QStringLiteral("type")).toString().toUpper();
        row.network   = metadata.value(QStringLiteral("network")).toString().toUpper();

        // 出站链按「组 → … → 节点」排列，最后一项才是真正的出口
        QStringList chainNames;
        for (const QJsonValue &chain : connection.value(QStringLiteral("chains")).toArray())
            chainNames << chain.toString();
        row.chain = chainNames.join(QStringLiteral(" → "));
        row.node  = chainNames.isEmpty() ? QString() : chainNames.last();

        const QString rule    = connection.value(QStringLiteral("rule")).toString();
        const QString payload = connection.value(QStringLiteral("rulePayload")).toString();
        row.rule = payload.isEmpty() ? rule : QStringLiteral("%1,%2").arg(rule, payload);

        row.upload   = calc::traffic(qint64(connection.value(QStringLiteral("upload")).toDouble()));
        row.download = calc::traffic(qint64(connection.value(QStringLiteral("download")).toDouble()));
        row.startedAt = connection.value(QStringLiteral("start")).toString();
        row.duration  = elapsedText(row.startedAt);

        rows.append(row);
    }

    // 新建立的排在最上面：列表每秒重排一次，位置得稳定
    std::stable_sort(rows.begin(), rows.end(), [](const ConnectionRow &a, const ConnectionRow &b) {
        return a.startedAt > b.startedAt;
    });

    // 刷新会重建数据，把当前选中与滚动位置记下来再还回去
    const int     selectedRow = m_list->currentIndex().row();
    const QString selectedId  = selectedRow >= 0 ? m_model->idAt(selectedRow) : QString();
    const int     scrollTop   = m_list->verticalScrollBar()->value();

    m_model->setRows(std::move(rows));

    const int restored = m_model->indexOfId(selectedId);
    if (restored >= 0) {
        m_list->selectionModel()->setCurrentIndex(
            m_model->index(restored),
            QItemSelectionModel::ClearAndSelect | QItemSelectionModel::NoUpdate);
    }
    m_list->verticalScrollBar()->setValue(scrollTop);

    m_stack->setCurrentWidget(m_model->rowCount() > 0 ? static_cast<QWidget *>(m_list)
                                                      : static_cast<QWidget *>(m_empty));
}

void ConnectionsPage::onCloseSelected()
{
    const QModelIndex index = m_list->currentIndex();
    if (!index.isValid()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a connection first"));
        return;
    }

    const QString id = m_model->idAt(index.row());
    if (id.isEmpty()) return;

    m_context->hub()->closeConnection(id);
    refresh();
}

void ConnectionsPage::onCloseAll()
{
    const int count = m_context->hub()->closeAllConnections();
    m_context->showToast(QStringLiteral("info"), tr("Closed %1 connection(s)").arg(count));
    refresh();
}

} // namespace clashqt
