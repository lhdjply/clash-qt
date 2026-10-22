#include "ui/pages/ProfilesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ProfileConfig.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QCheckBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QUrl>
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
    if (expire < now) return QCoreApplication::translate("clashqt::ProfilesPage", "Expired");
    return expire.toString(QStringLiteral("yyyy-MM-dd"))
           + QCoreApplication::translate("clashqt::ProfilesPage", " (%1 days)").arg(now.daysTo(expire));
}

QString formatUsage(const std::optional<SubscriptionUserInfo> &info)
{
    if (!info || info->total <= 0) return QStringLiteral("-");
    return QStringLiteral("%1 / %2")
        .arg(calc::traffic(info->download + info->upload), calc::traffic(info->total));
}

// ---------------------------------------------------------------------------
//  订阅对话框（添加 / 编辑共用）
//
//  两种模式的字段完全一致（名称、订阅链接、启动时自动更新），差别只在
//  标题、链接是否必填，以及编辑本地订阅时多一个「重新选择配置」。
//  合成一个类，免得两套几乎相同的界面各自漂移。
//
//  版面沿用主界面的构件（弹窗标题区 + 毛玻璃卡片 + 表单行 + 强调色主按钮），
//  这样弹窗和页面是一套观感，而不是原生控件的拼盘。
//
//  没有暴露 useProxy / interval 这类字段：它们在配置里能存，但内核侧尚未
//  使用（见 AppContext 的更新流程），摆出来会是「点得动但没作用」的假开关。
// ---------------------------------------------------------------------------
class SubscriptionDialog : public QDialog {
public:
    enum class Mode { Add, Edit };

    SubscriptionDialog(Mode mode, const ProfileItem &item, QWidget *parent)
        : QDialog(parent), m_mode(mode), m_item(item)
    {
        const bool adding = (m_mode == Mode::Add);
        m_remote          = adding || m_item.type == QLatin1String("remote");

        setObjectName(QStringLiteral("subscriptionDialog"));
        setWindowTitle(adding ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Add profile") : QCoreApplication::translate("clashqt::SubscriptionDialog", "Edit profile"));
        setMinimumWidth(560);

        auto *root = new QVBoxLayout(this);
        root->setContentsMargins(20, 18, 20, 16);
        root->setSpacing(16);

        root->addWidget(ui::makeDialogHeader(
            adding ? QStringLiteral("plus") : QStringLiteral("edit"),
            adding ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Add profile") : QCoreApplication::translate("clashqt::SubscriptionDialog", "Edit profile"),
            adding ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Enter the subscription URL; it will be downloaded and activated immediately.")
                   : (m_remote
                          ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Change the name or subscription URL. If this is the active profile, a changed URL is fetched again immediately.")
                          : QCoreApplication::translate("clashqt::SubscriptionDialog", "A local profile uses an imported config file, which you can replace below.")),
            this));

        auto *card       = ui::makeCard(QCoreApplication::translate("clashqt::SubscriptionDialog", "Profile info"),
                                      this, QStringLiteral("profiles"));
        auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

        m_name = new QLineEdit(item.name, card);
        m_name->setClearButtonEnabled(true);
        if (adding) {
            m_name->setPlaceholderText(QCoreApplication::translate("clashqt::SubscriptionDialog", "Optional; defaults to the subscription's domain"));
        }
        cardLayout->addWidget(ui::makeFormRow(QCoreApplication::translate("clashqt::SubscriptionDialog", "Name"), m_name, QString(), card));

        if (m_remote) {
            m_url = new QLineEdit(item.url, card);
            m_url->setClearButtonEnabled(true);
            m_url->setPlaceholderText(QStringLiteral("https://example.com/sub?token=..."));
            // 链接通常很长，默认把光标停在行首，免得一进来只看到尾巴
            m_url->setCursorPosition(0);
            cardLayout->addWidget(
                ui::makeFormRow(QCoreApplication::translate("clashqt::SubscriptionDialog", "Subscription URL"), m_url, QString(), card));

            m_autoUpdate = new QCheckBox(QCoreApplication::translate("clashqt::SubscriptionDialog", "Update this profile on startup"), card);
            // 新增时与 AppContext::addSubscription 的默认一致（默认开启）
            m_autoUpdate->setChecked(adding ? true : item.autoUpdate);
            cardLayout->addWidget(ui::makeFormRow(
                QString(), m_autoUpdate,
                QCoreApplication::translate("clashqt::SubscriptionDialog", "When enabled, the profile is refreshed automatically at startup."), card));
        } else {
            auto *fileRow  = new QWidget(card);
            auto *fileLine = new QHBoxLayout(fileRow);
            fileLine->setContentsMargins(0, 0, 0, 0);
            fileLine->setSpacing(8);

            m_fileLabel = new QLabel(item.file.isEmpty() ? QStringLiteral("-") : item.file,
                                     fileRow);
            m_fileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
            fileLine->addWidget(m_fileLabel, 1);

            auto *pick = new QPushButton(QCoreApplication::translate("clashqt::SubscriptionDialog", "Choose another file"), fileRow);
            ui::decorate(pick, QStringLiteral("folder"));
            fileLine->addWidget(pick);

            cardLayout->addWidget(ui::makeFormRow(
                QCoreApplication::translate("clashqt::SubscriptionDialog", "Config file"), fileRow,
                QCoreApplication::translate("clashqt::SubscriptionDialog", "Picking a new file replaces this profile's content; its id and active state are preserved."),
                card));

            QObject::connect(pick, &QPushButton::clicked, this, [this]() {
                const QString path = QFileDialog::getOpenFileName(
                    this, QCoreApplication::translate("clashqt::SubscriptionDialog", "Choose a config file"), QString(),
                    QCoreApplication::translate("clashqt::SubscriptionDialog", "Clash config (*.yaml *.yml);;All files (*)"));
                if (path.isEmpty()) return;
                m_replacementFile = path;
                m_fileLabel->setText(QFileInfo(path).fileName());
            });
        }

        root->addWidget(card);

        // 按钮排在卡片之外、右对齐：主操作是强调色实心按钮
        auto *actions = new QHBoxLayout();
        actions->setSpacing(8);
        actions->addStretch(1);

        auto *cancel = new QPushButton(QCoreApplication::translate("clashqt::SubscriptionDialog", "Cancel"), this);
        ui::decorate(cancel, QStringLiteral("close"));
        auto *confirm = ui::makePrimaryButton(
            adding ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Add") : QCoreApplication::translate("clashqt::SubscriptionDialog", "Save"), this);
        ui::decorate(confirm, adding ? QStringLiteral("plus") : QStringLiteral("check"));
        confirm->setDefault(true);

        actions->addWidget(cancel);
        actions->addWidget(confirm);
        root->addLayout(actions);

        QObject::connect(confirm, &QPushButton::clicked, this, &SubscriptionDialog::accept);
        QObject::connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    }

    QString name() const { return m_name->text().trimmed(); }
    QString url() const { return m_url ? m_url->text().trimmed() : QString(); }
    bool    autoUpdate() const { return m_autoUpdate && m_autoUpdate->isChecked(); }
    QString replacementFile() const { return m_replacementFile; }

    // 只有真正改动过的字段才写回，避免无谓地重写 profile.yaml
    QJsonObject patch() const
    {
        QJsonObject object;
        if (name() != m_item.name) object.insert(QStringLiteral("name"), name());
        if (m_remote) {
            if (url() != m_item.url) object.insert(QStringLiteral("url"), url());
            if (autoUpdate() != m_item.autoUpdate) {
                object.insert(QStringLiteral("autoUpdate"), autoUpdate());
            }
        }
        return object;
    }

    bool urlChanged() const { return m_remote && url() != m_item.url; }

    void accept() override
    {
        // 添加时名称可以留空（后端会按订阅域名生成），编辑时不允许清空
        if (name().isEmpty() && m_mode == Mode::Edit) {
            QMessageBox::warning(this, QCoreApplication::translate("clashqt::SubscriptionDialog", "Name required"),
                                 QCoreApplication::translate("clashqt::SubscriptionDialog", "Give this profile a name."));
            return;
        }
        if (m_remote) {
            const QUrl parsed(url());
            if (!parsed.isValid()
                || (parsed.scheme() != QLatin1String("http")
                    && parsed.scheme() != QLatin1String("https"))) {
                QMessageBox::warning(
                    this, QCoreApplication::translate("clashqt::SubscriptionDialog", "Invalid subscription URL"),
                    m_mode == Mode::Add
                        ? QCoreApplication::translate("clashqt::SubscriptionDialog", "Enter a subscription URL starting with http/https.")
                        : QCoreApplication::translate("clashqt::SubscriptionDialog", "The subscription URL must start with http/https."));
                return;
            }
        }
        QDialog::accept();
    }

private:
    Mode        m_mode = Mode::Add;
    ProfileItem m_item;
    bool        m_remote = false;

    QLineEdit   *m_name = nullptr;
    QLineEdit   *m_url = nullptr;
    QCheckBox   *m_autoUpdate = nullptr;
    QLabel      *m_fileLabel = nullptr;
    QString      m_replacementFile;
};

} // namespace

ProfilesPage::ProfilesPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Profiles"),
        tr("Manage config sources. The active profile is merged with \"Config overrides\" before use."), QStringLiteral("profiles")));

    // 工具栏用容器包一层：英文标签比中文长，默认左右内边距会把整行顶出
    // 横向滚动条（默认窗口宽度下「删除」会被切掉），容器上挂一条更紧的
    // 内边距规则，见 UiFactory 的 QWidget#profileToolbar
    auto *toolbarHost = new QWidget(this);
    toolbarHost->setObjectName(QStringLiteral("profileToolbar"));
    auto *toolbar = new QHBoxLayout(toolbarHost);
    toolbar->setContentsMargins(0, 0, 0, 0);
    toolbar->setSpacing(6);

    auto *addRemote = ui::makePrimaryButton(tr("Add profile"), this);
    auto *addLocal  = new QPushButton(tr("Import local config"), this);
    auto *editBtn   = new QPushButton(tr("Edit"), this);
    auto *update    = new QPushButton(tr("Update selected"), this);
    auto *updateAll = new QPushButton(tr("Update all"), this);
    auto *switchBtn = new QPushButton(tr("Set active"), this);
    auto *removeBtn = ui::makeDangerButton(tr("Delete"), this);

    ui::decorate(addRemote, QStringLiteral("plus"));
    ui::decorate(addLocal, QStringLiteral("folder"));
    ui::decorate(editBtn, QStringLiteral("edit"));
    ui::decorate(update, QStringLiteral("refresh"));
    ui::decorate(updateAll, QStringLiteral("clouddown"));
    ui::decorate(switchBtn, QStringLiteral("target"));
    ui::decorate(removeBtn, QStringLiteral("trash"));

    editBtn->setToolTip(tr("Edit the name or URL (or double-click a row)"));

    toolbar->addWidget(addRemote);
    toolbar->addWidget(addLocal);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(update);
    toolbar->addWidget(updateAll);
    toolbar->addWidget(switchBtn);
    toolbar->addWidget(removeBtn);
    toolbar->addStretch(1);
    root->addWidget(toolbarHost);

    m_emptyHint = ui::makeHintLabel(
        tr("No profiles yet. Click \"Add profile\" to enter a subscription URL, or \"Import local config\" to pick an existing Clash config file."),
        this);
    root->addWidget(m_emptyHint);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({tr("Active"), tr("Name"),
                                        tr("Type"), tr("Traffic"),
                                        tr("Expires"), tr("Updated")});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    // 名称吃掉剩余宽度，其余列给固定初值：默认列宽（约 100px）放不下
    // 「1.84 GB / 100 GB」「2027-01-14（84 天）」这类内容，会被截成省略号；
    // 而按内容自适应又会让表格撑出横向滚动条，所以用可拖拽的固定初值
    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    const struct {
        int column;
        int width;
    } widths[] = {
        {0, 56},   // 当前
        {2, 80},   // 类型
        {3, 150},  // 流量
        {4, 160},  // 到期
        {5, 150},  // 更新时间
    };
    for (const auto &entry : widths) {
        m_table->horizontalHeader()->setSectionResizeMode(entry.column,
                                                         QHeaderView::Interactive);
        m_table->setColumnWidth(entry.column, entry.width);
    }
    root->addWidget(m_table, 1);

    QObject::connect(addRemote, &QPushButton::clicked, this, &ProfilesPage::onAddSubscription);
    QObject::connect(addLocal, &QPushButton::clicked, this, &ProfilesPage::onAddLocal);
    QObject::connect(editBtn, &QPushButton::clicked, this, &ProfilesPage::onEditSelected);
    QObject::connect(update, &QPushButton::clicked, this, &ProfilesPage::onUpdateSelected);
    QObject::connect(updateAll, &QPushButton::clicked, this, &ProfilesPage::onUpdateAll);
    QObject::connect(switchBtn, &QPushButton::clicked, this, &ProfilesPage::onSwitchSelected);
    QObject::connect(removeBtn, &QPushButton::clicked, this, &ProfilesPage::onRemoveSelected);

    // 双击一行直接进编辑，省去「先选中再点按钮」两步
    QObject::connect(m_table, &QTableWidget::cellDoubleClicked, this,
                     [this](int, int) { onEditSelected(); });

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
        if (item.type == QLatin1String("remote")) typeText = tr("Remote");
        else if (item.type == QLatin1String("local")) typeText = tr("Local");
        else if (item.type == QLatin1String("plugin")) typeText = tr("Plugin");

        m_table->setItem(row, 0, currentCell);

        auto *nameItem = new QTableWidgetItem(item.name);
        // 列表里没有链接列，编辑时又要对照着看，因此挂在名称的悬浮提示上
        if (!item.url.isEmpty()) nameItem->setToolTip(item.url);
        else if (!item.file.isEmpty()) nameItem->setToolTip(item.file);

        m_table->setItem(row, 1, nameItem);
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
    // 与远程订阅同一套字段，因此复用编辑用的弹窗（模式不同而已）——
    // 原先连着弹两个原生输入框问「链接」「名称」，既丑又多一步
    ProfileItem blank;
    blank.type       = QStringLiteral("remote");
    blank.autoUpdate = true;

    SubscriptionDialog dialog(SubscriptionDialog::Mode::Add, blank, this);
    if (dialog.exec() != QDialog::Accepted) return;

    QString error;
    if (!m_context->addSubscription(dialog.url(), dialog.name(), &error)) {
        QMessageBox::warning(this, tr("Add failed"), error);
        return;
    }

    // addSubscription 固定开启自动更新；这里尊重用户在弹窗里的选择
    if (!dialog.autoUpdate()) {
        const QString id = m_context->profileConfig()->current();
        if (!id.isEmpty()) {
            m_context->profileConfig()->updateItem(id,
                                                   QJsonObject{{QStringLiteral("autoUpdate"), false}});
        }
    }
    refresh();
}

void ProfilesPage::onAddLocal()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Choose a config file"), QString(),
        tr("Clash config (*.yaml *.yml);;All files (*)"));
    if (filePath.isEmpty()) return;

    QString error;
    if (!m_context->addLocalProfile(filePath, QString(), &error)) {
        QMessageBox::warning(this, tr("Import failed"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onEditSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a profile first"));
        return;
    }

    const ProfileItem item = m_context->profileConfig()->item(id);
    if (item.id.isEmpty()) return;

    SubscriptionDialog dialog(SubscriptionDialog::Mode::Edit, item, this);
    if (dialog.exec() != QDialog::Accepted) return;

    const QJsonObject patch      = dialog.patch();
    const QString     newFile    = dialog.replacementFile();
    const bool        urlChanged = dialog.urlChanged();
    const bool        isCurrent  = m_context->profileConfig()->current() == id;

    if (patch.isEmpty() && newFile.isEmpty()) return; // 什么都没改

    if (!patch.isEmpty() && !m_context->profileConfig()->updateItem(id, patch)) {
        QMessageBox::warning(this, tr("Save failed"), tr("Failed to write profile config"));
        return;
    }

    // 本地订阅换了文件：内容替换（校验 → 写入 → 当前订阅则重载内核）
    if (!newFile.isEmpty()) {
        QString error;
        if (!m_context->replaceProfileContent(id, newFile, &error)) {
            QMessageBox::warning(this, tr("Replace failed"), error);
        }
        refresh();
        return;
    }

    // 远程订阅改了链接：当前订阅要立刻按新链接拉一份，否则界面显示的还是旧内容
    if (urlChanged && isCurrent) {
        QString error;
        if (!m_context->updateSubscription(id, &error)) {
            // 链接已经保存成功，只是内容没拉下来，提示即可，不必回滚
            QMessageBox::warning(
                this, tr("URL saved"),
                tr("URL saved, but fetching the new content failed:\n\n%1\n\nTry \"Update selected\" again later.")
                    .arg(error));
        }
    }

    refresh();
}

void ProfilesPage::onUpdateSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a profile first"));
        return;
    }

    QString error;
    if (!m_context->updateSubscription(id, &error)) {
        QMessageBox::warning(this, tr("Update failed"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onUpdateAll()
{
    QStringList errors;
    const int   count = m_context->updateAllSubscriptions(&errors);

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, tr("Some profiles failed to update"),
                             tr("%1 succeeded, %2 failed:\n\n%3")
                                 .arg(count)
                                 .arg(errors.size())
                                 .arg(errors.join(QStringLiteral("\n"))));
    } else {
        m_context->showToast(QStringLiteral("info"),
                             tr("Updated %1 profile(s)").arg(count));
    }
    refresh();
}

void ProfilesPage::onSwitchSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a profile first"));
        return;
    }

    QString error;
    if (!m_context->switchProfile(id, &error)) {
        QMessageBox::warning(this, tr("Switch failed"), error);
        return;
    }
    refresh();
}

void ProfilesPage::onRemoveSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), tr("Select a profile first"));
        return;
    }

    const ProfileItem item = m_context->profileConfig()->item(id);
    const auto        answer = QMessageBox::question(
        this, tr("Delete profile"),
        tr("Delete \"%1\"? Its config file will be removed as well.").arg(item.name));
    if (answer != QMessageBox::Yes) return;

    if (!m_context->profileConfig()->removeItem(id)) {
        QMessageBox::warning(this, tr("Delete failed"), tr("Profile not found"));
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
                             tr("Profile deleted, but reloading the config failed: %1").arg(error));
    }
    refresh();
}

} // namespace clashqt
