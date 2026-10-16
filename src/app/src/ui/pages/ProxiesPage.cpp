#include "ui/pages/ProxiesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"

#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {

ProxiesPage::ProxiesPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("代理"),
        QStringLiteral("选择代理组中的节点。GLOBAL 组在全局模式下决定所有流量的出口。")));

    // ---- 顶部操作条 -------------------------------------------------------
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    toolbar->addWidget(new QLabel(QStringLiteral("代理组"), this));
    m_groupCombo = new QComboBox(this);
    m_groupCombo->setMinimumWidth(240);
    toolbar->addWidget(m_groupCombo);

    m_summaryLabel = ui::makeHintLabel(QString(), this);
    toolbar->addWidget(m_summaryLabel, 1);

    m_testButton = new QPushButton(QStringLiteral("测试本组"), this);
    m_testAllButton = new QPushButton(QStringLiteral("测试全部组"), this);
    // 清除手动固定、交还给组的自动策略（URLTest/Fallback 按延迟选）
    m_autoButton = new QPushButton(QStringLiteral("恢复自动"), this);

    ui::decorate(m_testButton, QStringLiteral("bolt"));
    ui::decorate(m_testAllButton, QStringLiteral("speed"));
    ui::decorate(m_autoButton, QStringLiteral("check"));
    toolbar->addWidget(m_testButton);
    toolbar->addWidget(m_testAllButton);
    toolbar->addWidget(m_autoButton);

    root->addLayout(toolbar);

    // ---- 成员列表 ---------------------------------------------------------
    auto *card = ui::makeCard(QStringLiteral("节点"), this);
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_memberList = new QListWidget(card);
    m_memberList->setAlternatingRowColors(true);
    cardLayout->addWidget(m_memberList);
    root->addWidget(card, 1);

    QObject::connect(m_groupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &ProxiesPage::onGroupChanged);
    QObject::connect(m_memberList, &QListWidget::itemDoubleClicked, this,
                     &ProxiesPage::onItemActivated);
    QObject::connect(m_testButton, &QPushButton::clicked, this, &ProxiesPage::onTestCurrent);
    QObject::connect(m_testAllButton, &QPushButton::clicked, this, &ProxiesPage::onTestAll);
    QObject::connect(m_autoButton, &QPushButton::clicked, this, &ProxiesPage::onRestoreAuto);

    // 内核状态变化时自动刷新（例如切换节点后后端回写）
    QObject::connect(m_context->hub(), &core::Hub::configChanged, this,
                     &ProxiesPage::rebuildGroups);
}

void ProxiesPage::rebuildGroups()
{
    const QJsonObject proxies =
        m_context->hub()->proxiesStatus().value(QStringLiteral("proxies")).toObject();

    // 收集代理组：含有 all 字段的条目即为组
    QStringList groups;
    for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
        const QJsonObject entry = it.value().toObject();
        if (entry.contains(QStringLiteral("all")) && !entry.contains(QStringLiteral("unsupported"))) {
            groups << it.key();
        }
    }

    // GLOBAL 始终置顶，便于快速切换全局出口
    groups.sort();
    const int globalIndex = groups.indexOf(QStringLiteral("GLOBAL"));
    if (globalIndex > 0) groups.move(globalIndex, 0);

    const QString previous = m_currentGroup;

    // 阻断信号，避免批量填充时反复触发 currentIndexChanged
    m_groupCombo->blockSignals(true);
    m_groupCombo->clear();
    m_groupCombo->addItems(groups);

    if (!previous.isEmpty()) {
        const int index = groups.indexOf(previous);
        if (index >= 0) m_groupCombo->setCurrentIndex(index);
    }
    m_groupCombo->blockSignals(false);

    if (groups.isEmpty()) {
        m_memberList->clear();
        m_summaryLabel->setText(QStringLiteral("当前订阅没有任何代理组"));
        return;
    }

    onGroupChanged(m_groupCombo->currentIndex());
}

void ProxiesPage::refresh() { rebuildGroups(); }

void ProxiesPage::onGroupChanged(int index)
{
    m_memberList->clear();
    m_memberNames.clear();

    if (index < 0) return;

    const QString groupName = m_groupCombo->itemText(index);
    m_currentGroup = groupName;

    const QJsonObject proxies =
        m_context->hub()->proxiesStatus().value(QStringLiteral("proxies")).toObject();
    const QJsonObject group = proxies.value(groupName).toObject();

    const QString now   = group.value(QStringLiteral("now")).toString();
    const QString type  = group.value(QStringLiteral("type")).toString();

    m_summaryLabel->setText(
        QStringLiteral("类型 %1 · 当前 %2 · 共 %3 个成员")
            .arg(type.isEmpty() ? QStringLiteral("-") : type,
                 now.isEmpty() ? QStringLiteral("-") : now)
            .arg(group.value(QStringLiteral("all")).toArray().size()));

    const QJsonArray members = group.value(QStringLiteral("all")).toArray();
    for (const QJsonValue &value : members) {
        const QString name = value.toString();
        if (name.isEmpty()) continue;

        m_memberNames << name;

        const QJsonObject member = proxies.value(name).toObject();
        QString           suffix;

        if (member.contains(QStringLiteral("unsupported"))) {
            suffix = QStringLiteral("  [暂不支持]");
        } else {
            const QJsonArray history = member.value(QStringLiteral("history")).toArray();
            if (!history.isEmpty()) {
                const int delay = history.last().toObject().value(QStringLiteral("delay")).toInt();
                suffix = delay > 0 ? QStringLiteral("  %1 ms").arg(delay)
                                   : QStringLiteral("  超时");
            }
        }

        auto *item = new QListWidgetItem(QStringLiteral("%1%2%3")
                                            .arg(name == now ? QStringLiteral("● ") : QString(),
                                                 name, suffix),
                                        m_memberList);
        if (name == now) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
        // 把节点名挂在 item 上，激活时无需依赖显示文本（显示文本含前缀与延迟）
        item->setData(Qt::UserRole, name);
    }
}

void ProxiesPage::onItemActivated()
{
    QListWidgetItem *item = m_memberList->currentItem();
    if (!item || m_currentGroup.isEmpty()) return;

    const QString memberName = item->data(Qt::UserRole).toString();
    if (memberName.isEmpty()) return;

    QString error;
    if (!m_context->hub()->selectProxy(m_currentGroup, memberName, &error)) {
        m_context->showToast(QStringLiteral("error"),
                              QStringLiteral("切换节点失败：%1").arg(error));
        return;
    }

    // 记住这次选择：配置重载/下次启动都要以它为准
    m_context->saveProxySelections();

    m_context->showToast(QStringLiteral("info"),
                          QStringLiteral("%1 已切换到 %2").arg(m_currentGroup, memberName));
    onGroupChanged(m_groupCombo->currentIndex());
}

void ProxiesPage::onRestoreAuto()
{
    if (m_currentGroup.isEmpty()) return;

    QString error;
    if (!m_context->hub()->unfixProxy(m_currentGroup, &error)) {
        m_context->showToast(QStringLiteral("error"),
                              QStringLiteral("恢复自动选择失败：%1").arg(error));
        return;
    }

    // 配置里也要清掉，否则下次重载又把固定选择装回来
    m_context->saveProxySelections();

    m_context->showToast(QStringLiteral("info"),
                          QStringLiteral("%1 已恢复自动选择").arg(m_currentGroup));
    onGroupChanged(m_groupCombo->currentIndex());
}

void ProxiesPage::onTestCurrent()
{
    if (m_currentGroup.isEmpty()) return;

    const QJsonObject result = m_context->hub()->testGroupDelay(
        m_currentGroup, m_context->appConfig()->delayTestUrl(),
        m_context->appConfig()->delayTestTimeout());

    if (result.contains(QStringLiteral("message"))) {
        m_context->showToast(QStringLiteral("warning"),
                              QStringLiteral("测速失败：%1")
                                  .arg(result.value(QStringLiteral("message")).toString()));
        return;
    }

    m_context->showToast(QStringLiteral("info"),
                          QStringLiteral("已完成 %1 的延迟测试").arg(m_currentGroup));
    onGroupChanged(m_groupCombo->currentIndex());
}

void ProxiesPage::onTestAll()
{
    const QJsonObject proxies =
        m_context->hub()->proxiesStatus().value(QStringLiteral("proxies")).toObject();

    int tested = 0;
    for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
        const QJsonObject entry = it.value().toObject();
        if (!entry.contains(QStringLiteral("all"))) continue;
        m_context->hub()->testGroupDelay(it.key(), m_context->appConfig()->delayTestUrl(),
                                        m_context->appConfig()->delayTestTimeout());
        ++tested;
    }

    m_context->showToast(QStringLiteral("info"),
                          QStringLiteral("已测试 %1 个代理组").arg(tested));
    onGroupChanged(m_groupCombo->currentIndex());
}

} // namespace clashqt
