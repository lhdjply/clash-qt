#include "ui/pages/ProfilesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ProfileConfig.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QDateTime>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace clashqt {
namespace {

QString formatUpdated(qint64 timestamp)
{
    if (timestamp <= 0) return QStringLiteral("-");
    return QDateTime::fromSecsSinceEpoch(timestamp).toString(QStringLiteral("yyyy-MM-dd HH:mm"));
}

QString formatExpire(qint64 timestamp)
{
    if (timestamp <= 0) return QStringLiteral("-");
    const QDateTime expire = QDateTime::fromSecsSinceEpoch(timestamp);
    const QDateTime now    = QDateTime::currentDateTime();
    if (expire < now) return QStringLiteral("已过期");
    return expire.toString(QStringLiteral("yyyy-MM-dd"))
           + QStringLiteral("（%1 天）").arg(now.daysTo(expire));
}

QString formatUsage(const std::optional<SubscriptionUserInfo> &info)
{
    if (!info || info->total <= 0) return QStringLiteral("-");
    return QStringLiteral("%1 / %2")
        .arg(calc::traffic(info->download + info->upload), calc::traffic(info->total));
}

} // namespace

ProfilesPage::ProfilesPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("订阅"),
        QStringLiteral("管理配置来源。当前选中的订阅会与「配置覆写」合并后生效。")));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    auto *addRemote = ui::makePrimaryButton(QStringLiteral("添加订阅"), this);
    auto *addLocal  = new QPushButton(QStringLiteral("导入本地配置"), this);
    auto *update    = new QPushButton(QStringLiteral("更新选中"), this);
    auto *updateAll = new QPushButton(QStringLiteral("更新全部"), this);
    auto *switchBtn = new QPushButton(QStringLiteral("设为当前"), this);
    auto *removeBtn = ui::makeDangerButton(QStringLiteral("删除"), this);

    ui::decorate(addRemote, QStringLiteral("plus"));
    ui::decorate(addLocal, QStringLiteral("folder"));
    ui::decorate(update, QStringLiteral("refresh"));
    ui::decorate(updateAll, QStringLiteral("clouddown"));
    ui::decorate(switchBtn, QStringLiteral("target"));
    ui::decorate(removeBtn, QStringLiteral("trash"));

    toolbar->addWidget(addRemote);
    toolbar->addWidget(addLocal);
    toolbar->addWidget(update);
    toolbar->addWidget(updateAll);
    toolbar->addWidget(switchBtn);
    toolbar->addWidget(removeBtn);
    toolbar->addStretch(1);
    root->addLayout(toolbar);

    m_emptyHint = ui::makeHintLabel(
        QStringLiteral("还没有任何订阅。点击「添加订阅」填写订阅链接，"
                       "或「导入本地配置」选择一个已有的 Clash 配置文件。"),
        this);
    root->addWidget(m_emptyHint);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({QStringLiteral("当前"), QStringLiteral("名称"),
                                        QStringLiteral("类型"), QStringLiteral("流量"),
                                        QStringLiteral("到期"), QStringLiteral("更新时间")});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    root->addWidget(m_table, 1);

    QObject::connect(addRemote, &QPushButton::clicked, this, &ProfilesPage::onAddSubscription);
    QObject::connect(addLocal, &QPushButton::clicked, this, &ProfilesPage::onAddLocal);
    QObject::connect(update, &QPushButton::clicked, this, &ProfilesPage::onUpdateSelected);
    QObject::connect(updateAll, &QPushButton::clicked, this, &ProfilesPage::onUpdateAll);
    QObject::connect(switchBtn, &QPushButton::clicked, this, &ProfilesPage::onSwitchSelected);
    QObject::connect(removeBtn, &QPushButton::clicked, this, &ProfilesPage::onRemoveSelected);

    QObject::connect(m_context->profileConfig(), &ProfileConfig::configChanged, this,
                     &ProfilesPage::refresh);
}

void ProfilesPage::refresh()
{
    const QList<ProfileItem> items = m_context->profileConfig()->items();
    const QString            current = m_context->profileConfig()->current();

    m_table->setRowCount(items.size());
    for (int row = 0; row < items.size(); ++row) {
        const ProfileItem &item = items.at(row);

        auto *currentCell = new QTableWidgetItem(item.id == current ? QStringLiteral("●") : QString());
        currentCell->setData(Qt::UserRole, item.id);
        currentCell->setTextAlignment(Qt::AlignCenter);

        QString typeText = item.type;
        if (item.type == QLatin1String("remote")) typeText = QStringLiteral("远程");
        else if (item.type == QLatin1String("local")) typeText = QStringLiteral("本地");
        else if (item.type == QLatin1String("plugin")) typeText = QStringLiteral("插件");

        m_table->setItem(row, 0, currentCell);
        m_table->setItem(row, 1, new QTableWidgetItem(item.name));
        m_table->setItem(row, 2, new QTableWidgetItem(typeText));
        m_table->setItem(row, 3, new QTableWidgetItem(formatUsage(item.extra)));
        m_table->setItem(row, 4, new QTableWidgetItem(
                                     item.extra ? formatExpire(item.extra->expire)
                                                : QStringLiteral("-")));
        m_table->setItem(row, 5, new QTableWidgetItem(formatUpdated(item.updated)));
    }

    m_emptyHint->setVisible(items.isEmpty());
    m_table->setVisible(!items.isEmpty());
}

QString ProfilesPage::selectedId() const
{
    const int row = m_table->currentRow();
    if (row < 0) return QString();
    QTableWidgetItem *item = m_table->item(row, 0);
    return item ? item->data(Qt::UserRole).toString() : QString();
}

void ProfilesPage::onAddSubscription()
{
    bool ok = false;
    const QString url = QInputDialog::getText(this, QStringLiteral("添加订阅"),
                                             QStringLiteral("订阅链接（http/https）"),
                                             QLineEdit::Normal, QString(), &ok);
    if (!ok || url.trimmed().isEmpty()) return;

    bool          nameOk = false;
    const QString name =
        QInputDialog::getText(this, QStringLiteral("添加订阅"), QStringLiteral("显示名称（可留空）"),
                              QLineEdit::Normal, QString(), &nameOk);
    if (!nameOk) return;

    QString error;
    if (!m_context->addSubscription(url, name, &error)) {
        QMessageBox::warning(this, QStringLiteral("添加失败"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onAddLocal()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, QStringLiteral("选择配置文件"), QString(),
        QStringLiteral("Clash 配置 (*.yaml *.yml);;所有文件 (*)"));
    if (filePath.isEmpty()) return;

    QString error;
    if (!m_context->addLocalProfile(filePath, QString(), &error)) {
        QMessageBox::warning(this, QStringLiteral("导入失败"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onUpdateSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), QStringLiteral("请先选择一个订阅"));
        return;
    }

    QString error;
    if (!m_context->updateSubscription(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("更新失败"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onUpdateAll()
{
    QStringList errors;
    const int   count = m_context->updateAllSubscriptions(&errors);

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("部分订阅更新失败"),
                             QStringLiteral("成功 %1 个，失败 %2 个：\n\n%3")
                                 .arg(count)
                                 .arg(errors.size())
                                 .arg(errors.join(QStringLiteral("\n"))));
    } else {
        m_context->showToast(QStringLiteral("info"),
                             QStringLiteral("已更新 %1 个订阅").arg(count));
    }
    refresh();
}

void ProfilesPage::onSwitchSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), QStringLiteral("请先选择一个订阅"));
        return;
    }

    QString error;
    if (!m_context->switchProfile(id, &error)) {
        QMessageBox::warning(this, QStringLiteral("切换失败"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onRemoveSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), QStringLiteral("请先选择一个订阅"));
        return;
    }

    const ProfileItem item = m_context->profileConfig()->item(id);
    const auto        answer = QMessageBox::question(
        this, QStringLiteral("删除订阅"),
        QStringLiteral("确定要删除「%1」吗？该订阅的配置文件也会被删除。").arg(item.name));
    if (answer != QMessageBox::Yes) return;

    if (!m_context->profileConfig()->removeItem(id)) {
        QMessageBox::warning(this, QStringLiteral("删除失败"), QStringLiteral("订阅不存在"));
        return;
    }

    // 删除的是当前订阅时，内核需要按新的订阅重新生成配置
    if (m_context->profileConfig()->current() != id) {
        refresh();
        return;
    }

    QString error;
    if (!m_context->reloadKernel(&error)) {
        m_context->showToast(QStringLiteral("warning"),
                             QStringLiteral("订阅已删除，但配置重载失败：%1").arg(error));
    }
    refresh();
}

} // namespace clashqt
