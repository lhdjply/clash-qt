#include "ui/pages/ProfilesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "config/ProfileConfig.h"
#include "ui/UiFactory.h"
#include "utils/Calc.h"

#include <QCheckBox>
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
        setWindowTitle(adding ? QStringLiteral("添加订阅") : QStringLiteral("编辑订阅"));
        setMinimumWidth(560);

        auto *root = new QVBoxLayout(this);
        root->setContentsMargins(20, 18, 20, 16);
        root->setSpacing(16);

        root->addWidget(ui::makeDialogHeader(
            adding ? QStringLiteral("plus") : QStringLiteral("edit"),
            adding ? QStringLiteral("添加订阅") : QStringLiteral("编辑订阅"),
            adding ? QStringLiteral("填写订阅链接，程序会立即下载并切换到这个订阅。")
                   : (m_remote
                          ? QStringLiteral("修改名称或订阅链接。链接变更后，若它是当前订阅，"
                                           "会立即按新链接重新拉取。")
                          : QStringLiteral("本地订阅的内容来自导入的配置文件，可在下面替换。")),
            this));

        auto *card       = ui::makeCard(QStringLiteral("订阅信息"), this);
        auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

        m_name = new QLineEdit(item.name, card);
        m_name->setClearButtonEnabled(true);
        if (adding) {
            m_name->setPlaceholderText(QStringLiteral("可留空，默认按订阅域名命名"));
        }
        cardLayout->addWidget(ui::makeFormRow(QStringLiteral("名称"), m_name, QString(), card));

        if (m_remote) {
            m_url = new QLineEdit(item.url, card);
            m_url->setClearButtonEnabled(true);
            m_url->setPlaceholderText(QStringLiteral("https://example.com/sub?token=..."));
            // 链接通常很长，默认把光标停在行首，免得一进来只看到尾巴
            m_url->setCursorPosition(0);
            cardLayout->addWidget(
                ui::makeFormRow(QStringLiteral("订阅链接"), m_url, QString(), card));

            m_autoUpdate = new QCheckBox(QStringLiteral("启动时自动更新该订阅"), card);
            // 新增时与 AppContext::addSubscription 的默认一致（默认开启）
            m_autoUpdate->setChecked(adding ? true : item.autoUpdate);
            cardLayout->addWidget(ui::makeFormRow(
                QString(), m_autoUpdate,
                QStringLiteral("开启后，程序启动时会自动下载该订阅的最新配置。"), card));
        } else {
            auto *fileRow  = new QWidget(card);
            auto *fileLine = new QHBoxLayout(fileRow);
            fileLine->setContentsMargins(0, 0, 0, 0);
            fileLine->setSpacing(8);

            m_fileLabel = new QLabel(item.file.isEmpty() ? QStringLiteral("-") : item.file,
                                     fileRow);
            m_fileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
            fileLine->addWidget(m_fileLabel, 1);

            auto *pick = new QPushButton(QStringLiteral("重新选择配置"), fileRow);
            ui::decorate(pick, QStringLiteral("folder"));
            fileLine->addWidget(pick);

            cardLayout->addWidget(ui::makeFormRow(
                QStringLiteral("配置文件"), fileRow,
                QStringLiteral("选择新文件后会替换该订阅的内容，id 与「当前订阅」状态保持不变。"),
                card));

            QObject::connect(pick, &QPushButton::clicked, this, [this]() {
                const QString path = QFileDialog::getOpenFileName(
                    this, QStringLiteral("选择配置文件"), QString(),
                    QStringLiteral("Clash 配置 (*.yaml *.yml);;所有文件 (*)"));
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

        auto *cancel = new QPushButton(QStringLiteral("取消"), this);
        ui::decorate(cancel, QStringLiteral("close"));
        auto *confirm = ui::makePrimaryButton(
            adding ? QStringLiteral("添加") : QStringLiteral("保存"), this);
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
            QMessageBox::warning(this, QStringLiteral("名称不能为空"),
                                 QStringLiteral("请给这个订阅起一个名称。"));
            return;
        }
        if (m_remote) {
            const QUrl parsed(url());
            if (!parsed.isValid()
                || (parsed.scheme() != QLatin1String("http")
                    && parsed.scheme() != QLatin1String("https"))) {
                QMessageBox::warning(
                    this, QStringLiteral("订阅链接无效"),
                    m_mode == Mode::Add
                        ? QStringLiteral("请填写以 http/https 开头的订阅链接。")
                        : QStringLiteral("订阅链接需要以 http/https 开头，请检查后重试。"));
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
        QStringLiteral("订阅"),
        QStringLiteral("管理配置来源。当前选中的订阅会与「配置覆写」合并后生效。")));

    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    auto *addRemote = ui::makePrimaryButton(QStringLiteral("添加订阅"), this);
    auto *addLocal  = new QPushButton(QStringLiteral("导入本地配置"), this);
    auto *editBtn   = new QPushButton(QStringLiteral("编辑"), this);
    auto *update    = new QPushButton(QStringLiteral("更新选中"), this);
    auto *updateAll = new QPushButton(QStringLiteral("更新全部"), this);
    auto *switchBtn = new QPushButton(QStringLiteral("设为当前"), this);
    auto *removeBtn = ui::makeDangerButton(QStringLiteral("删除"), this);

    ui::decorate(addRemote, QStringLiteral("plus"));
    ui::decorate(addLocal, QStringLiteral("folder"));
    ui::decorate(editBtn, QStringLiteral("edit"));
    ui::decorate(update, QStringLiteral("refresh"));
    ui::decorate(updateAll, QStringLiteral("clouddown"));
    ui::decorate(switchBtn, QStringLiteral("target"));
    ui::decorate(removeBtn, QStringLiteral("trash"));

    editBtn->setToolTip(QStringLiteral("修改名称 / 订阅链接（双击某一行也可以）"));

    toolbar->addWidget(addRemote);
    toolbar->addWidget(addLocal);
    toolbar->addWidget(editBtn);
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
        if (item.type == QLatin1String("remote")) typeText = QStringLiteral("远程");
        else if (item.type == QLatin1String("local")) typeText = QStringLiteral("本地");
        else if (item.type == QLatin1String("plugin")) typeText = QStringLiteral("插件");

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
        QMessageBox::warning(this, QStringLiteral("添加失败"), error);
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

void ProfilesPage::onEditSelected()
{
    const QString id = selectedId();
    if (id.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"), QStringLiteral("请先选择一个订阅"));
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
        QMessageBox::warning(this, QStringLiteral("保存失败"), QStringLiteral("写入订阅配置失败"));
        return;
    }

    // 本地订阅换了文件：内容替换（校验 → 写入 → 当前订阅则重载内核）
    if (!newFile.isEmpty()) {
        QString error;
        if (!m_context->replaceProfileContent(id, newFile, &error)) {
            QMessageBox::warning(this, QStringLiteral("替换失败"), error);
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
                this, QStringLiteral("链接已保存"),
                QStringLiteral("链接已保存，但拉取新订阅内容失败：\n\n%1\n\n"
                               "可稍后用「更新选中」重试。")
                    .arg(error));
        }
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
