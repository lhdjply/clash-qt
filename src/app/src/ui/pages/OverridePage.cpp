#include "ui/pages/OverridePage.h"

#include "AppContext.h"
#include "ui/UiFactory.h"

#include <QCheckBox>
#include <QCoreApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHideEvent>
#include <QShowEvent>
#include <QTableWidget>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {
namespace {

constexpr char kDangerColor[] = "color:#E23B36;";

// ---------------------------------------------------------------------------
//  规则结构化解析 / 拼装
//
//  与参照实现的 simple-rules.ts 同一套约定：
//    · 只在**顶层**逗号处切分（AND/OR 等逻辑规则内部有括号与逗号）
//    · MATCH 没有「内容」段
//    · 尾部只认 no-resolve / src 两个修饰符
// ---------------------------------------------------------------------------
struct ParsedRule {
    QString     type;
    QString     payload;
    QString     target;
    QStringList options;
};

QStringList splitTopLevel(const QString &rule)
{
    QStringList parts;
    int         depth = 0;
    int         start = 0;
    for (int i = 0; i < rule.size(); ++i) {
        const QChar ch = rule.at(i);
        if (ch == QLatin1Char('(')) ++depth;
        if (ch == QLatin1Char(')')) --depth;
        if (ch == QLatin1Char(',') && depth == 0) {
            parts << rule.mid(start, i - start).trimmed();
            start = i + 1;
        }
    }
    parts << rule.mid(start).trimmed();
    return parts;
}

ParsedRule parseRule(const QString &text)
{
    ParsedRule  rule;
    QStringList parts = splitTopLevel(text.trimmed());
    if (parts.isEmpty()) return rule;

    const int minimum = (parts.first() == QLatin1String("MATCH")) ? 2 : 3;
    while (parts.size() > minimum) {
        const QString option = parts.last();
        if (option != QLatin1String("no-resolve") && option != QLatin1String("src")) break;
        rule.options.prepend(option);
        parts.removeLast();
    }

    rule.type   = parts.takeFirst();
    rule.target = parts.isEmpty() ? QString() : parts.takeLast();
    rule.payload = parts.join(QLatin1Char(','));
    return rule;
}

QString buildRule(const ParsedRule &rule)
{
    QStringList parts;
    parts << rule.type;
    if (rule.type != QLatin1String("MATCH") && !rule.payload.isEmpty()) parts << rule.payload;
    parts << rule.target;
    if (!rule.options.isEmpty()) parts << rule.options.first();
    return parts.join(QLatin1Char(','));
}

// ---------------------------------------------------------------------------
//  规则类型目录
//
//  label      界面上的中文名（不懂语法的人看这个）
//  hint       内容输入框的示例
//  candidates 可选候选值（| 分隔），空表示自由输入
//  needValue  是否需要填「内容」（MATCH 不需要）
// ---------------------------------------------------------------------------
struct RuleTypeInfo {
    const char *type;
    const char *label;
    const char *hint;
    const char *candidates;
    bool        needValue;
    bool        noResolve;
    bool        src;
};

const RuleTypeInfo kRuleTypes[] = {
    {"DOMAIN-SUFFIX", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Domain suffix (incl. subdomains)"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. example.com"), "", true, false, false},
    {"DOMAIN", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Domain (exact)"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. www.example.com"), "", true, false, false},
    {"DOMAIN-KEYWORD", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Domain keyword"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. google"), "", true, false, false},
    {"DOMAIN-REGEX", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Domain regex"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. ^ads\\..*"), "", true, false, false},
    {"GEOSITE", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Geosite category"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. cn / netflix / telegram"), "", true, false, false},
    {"GEOIP", QT_TRANSLATE_NOOP("clashqt::OverridePage", "IP country"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. CN / US / JP"), "CN|US|JP|HK|SG|TW|UK|DE", true, true, true},
    {"IP-CIDR", QT_TRANSLATE_NOOP("clashqt::OverridePage", "IPv4 CIDR"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. 10.0.0.0/8"), "", true, true, true},
    {"IP-CIDR6", QT_TRANSLATE_NOOP("clashqt::OverridePage", "IPv6 CIDR"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. 2001:db8::/32"), "", true, true, true},
    {"IP-ASN", QT_TRANSLATE_NOOP("clashqt::OverridePage", "ASN"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. 13335"), "", true, true, true},
    {"DST-PORT", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Destination port"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. 443"), "", true, false, false},
    {"SRC-PORT", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Source port"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. 1234"), "", true, false, false},
    {"NETWORK", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Network type"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "tcp or udp"), "tcp|udp", true, false, false},
    {"PROCESS-NAME", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Process name"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. telegram"), "", true, false, false},
    {"PROCESS-PATH", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Process path"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "e.g. /usr/bin/curl"), "", true, false, false},
    {"RULE-SET", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Rule set"), QT_TRANSLATE_NOOP("clashqt::OverridePage", "Choose a rule set from the config"), "", true, true, false},
    {"MATCH", QT_TRANSLATE_NOOP("clashqt::OverridePage", "Catch-all (match all remaining traffic)"), QT_TRANSLATE_NOOP("clashqt::OverridePage", ""), "", false, false, false},
};

const RuleTypeInfo *findType(const QString &type)
{
    for (const RuleTypeInfo &info : kRuleTypes) {
        if (type == QLatin1String(info.type)) return &info;
    }
    return nullptr;
}

QString typeLabel(const QString &type)
{
    if (const RuleTypeInfo *info = findType(type))
        return QCoreApplication::translate("clashqt::OverridePage", info->label);
    // 不在目录里的（AND / OR / SUB-RULE 等手写规则）直接显示类型名
    return type;
}

QToolButton *makeRowButton(QWidget *parent, const QString &text, const QString &tip)
{
    auto *button = new QToolButton(parent);
    button->setText(text);
    button->setToolTip(tip);
    button->setAutoRaise(true);
    button->setFixedSize(26, 24);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

// ---------------------------------------------------------------------------
//  规则编辑弹窗
//
//  不加 Q_OBJECT：内部只用 lambda 连接，不需要自有信号槽，省掉一个 moc 单元。
// ---------------------------------------------------------------------------
class RuleDialog : public QDialog {
public:
    RuleDialog(QWidget *parent, const ParsedRule &initial, const QStringList &targets,
               const QStringList &ruleSets)
        : QDialog(parent), m_ruleSets(ruleSets)
    {
        setWindowTitle(initial.type.isEmpty() ? QCoreApplication::translate("clashqt::RuleDialog", "New rule")
                                              : QCoreApplication::translate("clashqt::RuleDialog", "Edit rule"));
        setModal(true);
        setMinimumWidth(480);

        auto *root = new QVBoxLayout(this);
        root->setContentsMargins(20, 18, 20, 16);
        root->setSpacing(12);

        auto *header = new QLabel(
            QCoreApplication::translate("clashqt::RuleDialog", "Follow the prompts; the rule syntax is assembled when you save."), this);
        header->setWordWrap(true);
        root->addWidget(header);

        auto *form = new QFormLayout();
        form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
        form->setHorizontalSpacing(12);
        form->setVerticalSpacing(10);

        m_type = new QComboBox(this);
        for (const RuleTypeInfo &info : kRuleTypes) {
            const QString type = QString::fromUtf8(info.type);
            m_type->addItem(QCoreApplication::translate("clashqt::OverridePage", info.label), type);
            // 语法名放进 tooltip：界面上保持中文可读，需要核对时悬停即可
            m_type->setItemData(m_type->count() - 1, type, Qt::ToolTipRole);
        }
        // 手写型规则（AND/OR/SUB-RULE…）也要能编辑：不在目录里就补一项
        if (!initial.type.isEmpty() && !findType(initial.type)) {
            m_type->addItem(QCoreApplication::translate("clashqt::RuleDialog", "%1 (cannot be edited structurally)").arg(initial.type),
                            initial.type);
        }
        form->addRow(QCoreApplication::translate("clashqt::RuleDialog", "Rule type"), m_type);

        // 内容控件有两种形态：自由输入用 QLineEdit，有候选值（GEOIP / NETWORK /
        // RULE-SET）用 QComboBox。同一行里按需切换显示，避免出现"点开是空列表"
        // 的下拉框，也不会留下一个收不起的箭头空位。
        m_payloadHost   = new QWidget(this);
        auto *payloadRow = new QHBoxLayout(m_payloadHost);
        payloadRow->setContentsMargins(0, 0, 0, 0);
        payloadRow->setSpacing(0);
        m_payloadEdit = new QLineEdit(m_payloadHost);
        m_payloadEdit->setObjectName(QStringLiteral("payloadEdit"));
        m_payloadCombo = new QComboBox(m_payloadHost);
        m_payloadCombo->setEditable(true);
        m_payloadCombo->setInsertPolicy(QComboBox::NoInsert);
        payloadRow->addWidget(m_payloadEdit);
        payloadRow->addWidget(m_payloadCombo);
        form->addRow(QCoreApplication::translate("clashqt::RuleDialog", "Content"), m_payloadHost);

        m_target = new QComboBox(this);
        m_target->setEditable(true);
        m_target->setInsertPolicy(QComboBox::NoInsert);
        m_target->addItems(targets);
        m_target->lineEdit()->setPlaceholderText(QCoreApplication::translate("clashqt::RuleDialog", "Choose a policy or proxy group"));
        form->addRow(QCoreApplication::translate("clashqt::RuleDialog", "Target"), m_target);

        m_option = new QComboBox(this);
        m_option->addItem(QCoreApplication::translate("clashqt::RuleDialog", "Default"), QString());
        m_option->addItem(QCoreApplication::translate("clashqt::RuleDialog", "Do not resolve domain (no-resolve)"), QStringLiteral("no-resolve"));
        m_option->addItem(QCoreApplication::translate("clashqt::RuleDialog", "Match source address (src)"), QStringLiteral("src"));
        form->addRow(QCoreApplication::translate("clashqt::RuleDialog", "Options"), m_option);
        // 该行在不需要时整体隐藏（见 syncType），避免出现一个灰掉的空选项
        m_optionRow = qobject_cast<QWidget *>(form->labelForField(m_option));

        root->addLayout(form);

        m_hint = new QLabel(this);
        m_hint->setWordWrap(true);
        m_hint->setStyleSheet(QStringLiteral("color:%1;").arg(ui::mutedColor().name()));
        root->addWidget(m_hint);

        m_error = new QLabel(this);
        m_error->setWordWrap(true);
        m_error->setStyleSheet(QString::fromLatin1(kDangerColor));
        m_error->hide();
        root->addWidget(m_error);

        // 自己排按钮：QDialogButtonBox 的标准按钮文字跟随 Qt 翻译，未加载时
        // 会显示英文 "Cancel"，且"保存"在左不符合习惯
        auto *actions = new QHBoxLayout();
        actions->setContentsMargins(0, 0, 0, 0);
        actions->setSpacing(8);
        actions->addStretch(1);
        auto *cancel = new QPushButton(QCoreApplication::translate("clashqt::RuleDialog", "Cancel"), this);
        auto *ok     = ui::makePrimaryButton(QCoreApplication::translate("clashqt::RuleDialog", "Save"), this);
        ok->setDefault(true);
        actions->addWidget(cancel);
        actions->addWidget(ok);
        root->addLayout(actions);

        QObject::connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        QObject::connect(ok, &QPushButton::clicked, this, [this]() { commit(); });

        // ---- 初值 ----
        const QString type = initial.type.isEmpty() ? QStringLiteral("DOMAIN-SUFFIX")
                                                    : initial.type;
        const int typeIndex = m_type->findData(type);
        if (typeIndex >= 0) m_type->setCurrentIndex(typeIndex);
        m_target->setCurrentText(initial.target.isEmpty() ? QStringLiteral("DIRECT")
                                                          : initial.target);
        if (!initial.options.isEmpty()) {
            const int optionIndex = m_option->findData(initial.options.first());
            if (optionIndex >= 0) m_option->setCurrentIndex(optionIndex);
        }
        m_initialPayload = initial.payload;

        QObject::connect(m_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                         [this]() { syncType(true); });
        syncType(false);
        m_payloadEdit->setText(initial.payload);
        m_payloadCombo->setCurrentText(initial.payload);
    }

    ParsedRule value() const { return m_value; }

private:
    // resetPayload=false 时保留当前内容（初始化阶段）；true 表示用户切了类型
    void syncType(bool typeChanged)
    {
        const QString      type = m_type->currentData().toString();
        const RuleTypeInfo *info = findType(type);

        const QString keep = typeChanged ? QString() : m_initialPayload;

        const bool hasCandidates =
            (type == QLatin1String("RULE-SET") && !m_ruleSets.isEmpty())
            || (info && info->candidates && *info->candidates);

        m_payloadCombo->clear();
        if (type == QLatin1String("RULE-SET")) {
            m_payloadCombo->addItems(m_ruleSets);
            m_payloadCombo->lineEdit()->setPlaceholderText(
                m_ruleSets.isEmpty() ? QCoreApplication::translate("clashqt::RuleDialog", "No rule-providers in the config")
                                     : QCoreApplication::translate("clashqt::RuleDialog", "Choose a rule set"));
        } else if (hasCandidates) {
            m_payloadCombo->addItems(
                QString::fromUtf8(info->candidates).split(QLatin1Char('|'), Qt::SkipEmptyParts));
        }
        m_payloadCombo->setCurrentText(keep);
        m_payloadCombo->setVisible(hasCandidates);

        m_payloadEdit->setText(keep);
        m_payloadEdit->setPlaceholderText(
            QCoreApplication::translate("clashqt::OverridePage", info && info->hint ? info->hint : ""));
        m_payloadEdit->setVisible(!hasCandidates);

        const bool editable = !info || info->needValue;
        m_payloadEdit->setEnabled(editable);
        m_payloadCombo->setEnabled(editable);

        // MATCH 与域名类规则不需要匹配方式：整行隐藏，别让用户对着灰控件猜
        const bool showOption = info && (info->noResolve || info->src);
        m_option->setVisible(showOption);
        if (m_optionRow) m_optionRow->setVisible(showOption);
        m_option->setEnabled(showOption);
        const QString option = m_option->currentData().toString();
        if ((option == QLatin1String("no-resolve") && info && !info->noResolve)
            || (option == QLatin1String("src") && info && !info->src)) {
            m_option->setCurrentIndex(0);
        }

        QString hint;
        if (type == QLatin1String("MATCH")) {
            hint = QCoreApplication::translate("clashqt::RuleDialog", "A catch-all rule matches all remaining traffic; keep it last among your custom rules.");
        } else if (type == QLatin1String("DOMAIN-SUFFIX")) {
            hint = QCoreApplication::translate("clashqt::RuleDialog", "example.com also matches all of its subdomains.");
        } else if (type == QLatin1String("IP-CIDR")) {
            hint = QCoreApplication::translate("clashqt::RuleDialog", "For LAN traffic, set Options to no-resolve.");
        } else if (type == QLatin1String("RULE-SET")) {
            hint = QCoreApplication::translate("clashqt::RuleDialog", "Rule sets come from rule-providers in the profile.");
        }
        m_hint->setText(hint);
        m_hint->setVisible(!hint.isEmpty());
    }

    void commit()
    {
        m_error->hide();

        const QString      type = m_type->currentData().toString();
        const RuleTypeInfo *info = findType(type);

        ParsedRule rule;
        rule.type = type;
        rule.payload = (m_payloadCombo->isVisible() ? m_payloadCombo->currentText()
                                                   : m_payloadEdit->text())
                           .trimmed();
        rule.target  = m_target->currentText().trimmed();
        const QString option = m_option->currentData().toString();
        if (!option.isEmpty()) rule.options << option;

        if (rule.target.isEmpty()) {
            m_error->setText(QCoreApplication::translate("clashqt::RuleDialog", "Choose what handles this rule (policy / proxy group)."));
            m_error->show();
            return;
        }
        if ((!info || info->needValue) && rule.payload.isEmpty()) {
            m_error->setText(QCoreApplication::translate("clashqt::RuleDialog", "Enter the content to match; it is what the rule checks."));
            m_error->show();
            return;
        }
        if (type == QLatin1String("IP-CIDR") && !rule.payload.contains(QLatin1Char('/'))) {
            m_error->setText(QCoreApplication::translate("clashqt::RuleDialog", "IPv4 CIDR needs a prefix length, e.g. 10.0.0.0/8."));
            m_error->show();
            return;
        }

        m_value = rule;
        accept();
    }

    QComboBox  *m_type = nullptr;
    QWidget    *m_payloadHost = nullptr;
    QLineEdit  *m_payloadEdit = nullptr;
    QComboBox  *m_payloadCombo = nullptr;
    QComboBox  *m_target = nullptr;
    QComboBox  *m_option = nullptr;
    QLabel     *m_hint = nullptr;
    QWidget    *m_optionRow = nullptr;
    QLabel     *m_error = nullptr;
    QStringList m_ruleSets;
    QString     m_initialPayload;
    ParsedRule  m_value;
};

} // namespace

OverridePage::OverridePage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        tr("Overrides"),
        tr("Adds custom rules on top of the profile's rules. Pick a type, content and target when adding — no rule syntax needed. The list order is the match priority; changes are saved and applied automatically."), QStringLiteral("code")));

    auto *card       = ui::makeCard(tr("Custom rules"), this, QStringLiteral("rules"));
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_profileLabel = ui::makeHintLabel(QString(), card);
    cardLayout->addWidget(m_profileLabel);

    auto *toolbar = new QHBoxLayout();
    toolbar->setContentsMargins(0, 0, 0, 0);
    toolbar->setSpacing(8);

    auto *addButton = new QPushButton(tr("New rule"), card);
    ui::decorate(addButton, QStringLiteral("plus"));

    toolbar->addWidget(addButton);
    toolbar->addStretch(1);
    m_summaryLabel = ui::makeHintLabel(QString(), card);
    toolbar->addWidget(m_summaryLabel);
    cardLayout->addLayout(toolbar);

    m_table = new QTableWidget(card);
    m_table->setObjectName(QStringLiteral("overrideTable"));
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({tr("Enabled"), tr("Rule type"),
                                        tr("Content"), tr("Target"),
                                        tr("Actions")});
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setWordWrap(false);

    QHeaderView *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setHighlightSections(false);
    m_table->setMinimumHeight(180);
    cardLayout->addWidget(m_table);

    cardLayout->addWidget(ui::makeHintLabel(
        tr("Double-click any row to edit; uncheck it to disable (content is kept but not matched). Changes are saved and applied automatically."),
        card));

    m_statusLabel = ui::makeHintLabel(QString(), card);
    cardLayout->addWidget(m_statusLabel);

    root->addWidget(card);
    root->addStretch(1);

    QObject::connect(addButton, &QPushButton::clicked, this, [this]() { editRule(-1); });
    QObject::connect(m_table, &QTableWidget::cellDoubleClicked, this,
                     [this](int row, int) { editRule(row); });

    // 改动后 500ms 内没有新动作才落盘：连点删除、连续排序只重载一次
    m_saveTimer = new QTimer(this);
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(500);
    QObject::connect(m_saveTimer, &QTimer::timeout, this, &OverridePage::persist);

    refresh();
}

void OverridePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refresh();
}

void OverridePage::hideEvent(QHideEvent *event)
{
    // 还没到防抖时间就切走了：立刻写完，否则这些改动会落在切页之后的
    // 订阅上（订阅切换会改变 rulePath 的目标文件）
    if (m_saveTimer && m_saveTimer->isActive()) {
        m_saveTimer->stop();
        persist();
    }
    QWidget::hideEvent(event);
}

QStringList OverridePage::targetNames() const
{
    QStringList groups;
    QStringList nodes;

    const QJsonObject root    = m_context->hub()->proxiesStatus();
    const QJsonObject proxies = root.value(QStringLiteral("proxies")).toObject();
    for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
        const QString name = it.key();
        if (name == QLatin1String("DIRECT") || name == QLatin1String("REJECT")) continue;
        // 组带 all 字段（成员列表），节点没有
        if (it.value().toObject().contains(QStringLiteral("all"))) {
            groups << name;
        } else {
            nodes << name;
        }
    }
    groups.sort();
    nodes.sort();

    QStringList result{QStringLiteral("DIRECT"), QStringLiteral("REJECT")};
    result << groups << nodes;
    return result;
}

QStringList OverridePage::ruleSetNames() const
{
    return m_context->hub()->config().ruleProviders().keys();
}

void OverridePage::refresh()
{
    const QString profile = m_context->currentProfileName();
    m_profileLabel->setText(
        profile.isEmpty()
            ? tr("No profile selected; custom rules cannot be saved")
            : tr("Applies to profile: %1 (rules are stored per profile and differ between profiles)")
                  .arg(profile));

    const AppContext::RuleOverride value = m_context->ruleOverride();

    QSet<QString> disabled;
    for (const QString &rule : value.disabled) disabled.insert(rule);

    m_rules.clear();
    for (const QString &text : value.prepend) {
        m_rules.append({text, !disabled.contains(text)});
    }

    rebuildTable();
    updateSummary();
    m_statusLabel->setStyleSheet(QString());
    m_statusLabel->clear();
}

void OverridePage::updateSummary()
{
    int enabled = 0;
    for (const RuleEntry &entry : m_rules) {
        if (entry.enabled) ++enabled;
    }
    m_summaryLabel->setText(
        tr("%1 rules, %2 enabled").arg(m_rules.size()).arg(enabled));
}

void OverridePage::rebuildTable()
{
    m_rebuilding = true;

    const int keepRow = m_table->currentRow();
    m_table->setRowCount(0);

    for (int row = 0; row < m_rules.size(); ++row) {
        const RuleEntry  &entry  = m_rules.at(row);
        const ParsedRule  parsed = parseRule(entry.text);

        m_table->insertRow(row);

        auto *checkHost   = new QWidget(m_table);
        auto *checkLayout = new QHBoxLayout(checkHost);
        checkLayout->setContentsMargins(0, 0, 0, 0);
        checkLayout->setAlignment(Qt::AlignCenter);
        auto *checkBox = new QCheckBox(checkHost);
        checkBox->setChecked(entry.enabled);
        checkBox->setToolTip(entry.enabled ? tr("Enabled; uncheck to disable")
                                           : tr("Disabled: not matched after saving"));
        checkLayout->addWidget(checkBox);
        m_table->setCellWidget(row, 0, checkHost);

        QObject::connect(checkBox, &QCheckBox::toggled, this, [this, row](bool on) {
            if (m_rebuilding || row < 0 || row >= m_rules.size()) return;
            m_rules[row].enabled = on;
            for (int col = 1; col <= 3; ++col) {
                if (QTableWidgetItem *item = m_table->item(row, col)) {
                    item->setForeground(on ? QBrush() : QBrush(ui::mutedColor()));
                }
            }
            updateSummary();
            scheduleSave();
        });

        const auto makeCell = [this, row, &entry](int column, const QString &text,
                                                  const QString &tip) {
            auto *item = new QTableWidgetItem(text);
            item->setToolTip(tip);
            if (!entry.enabled) item->setForeground(ui::mutedColor());
            m_table->setItem(row, column, item);
        };

        makeCell(1, typeLabel(parsed.type),
                 tr("%1 (%2)\nRule: %3")
                     .arg(typeLabel(parsed.type), parsed.type, entry.text));
        makeCell(2, parsed.payload.isEmpty() ? QStringLiteral("—") : parsed.payload,
                 parsed.payload);
        makeCell(3, parsed.target, parsed.target);

        auto *ops    = new QWidget(m_table);
        auto *layout = new QHBoxLayout(ops);
        layout->setContentsMargins(2, 0, 2, 0);
        layout->setSpacing(2);

        auto *up = makeRowButton(ops, QStringLiteral("↑"), tr("Move up (higher priority)"));
        auto *down =
            makeRowButton(ops, QStringLiteral("↓"), tr("Move down (lower priority)"));
        auto *edit = makeRowButton(ops, QStringLiteral("✎"), tr("Edit this rule"));
        auto *remove = makeRowButton(ops, QStringLiteral("✕"), tr("Delete this rule"));
        up->setEnabled(row > 0);
        down->setEnabled(row + 1 < m_rules.size());

        const QString mutedStyle = QStringLiteral("color:%1;").arg(ui::mutedColor().name());
        if (row == 0) up->setStyleSheet(mutedStyle);
        if (row + 1 >= m_rules.size()) down->setStyleSheet(mutedStyle);

        QObject::connect(up, &QToolButton::clicked, this, [this, row]() { moveRule(row, -1); });
        QObject::connect(down, &QToolButton::clicked, this, [this, row]() { moveRule(row, 1); });
        QObject::connect(edit, &QToolButton::clicked, this, [this, row]() { editRule(row); });
        QObject::connect(remove, &QToolButton::clicked, this, [this, row]() { removeRule(row); });

        layout->addWidget(up);
        layout->addWidget(down);
        layout->addWidget(edit);
        layout->addWidget(remove);
        m_table->setCellWidget(row, 4, ops);
    }

    if (keepRow >= 0 && keepRow < m_table->rowCount()) {
        m_table->selectRow(keepRow);
    }

    m_rebuilding = false;
    updateTableHeight();
}

void OverridePage::updateTableHeight()
{
    const int header = m_table->horizontalHeader()->sizeHint().height();
    const int rows   = qMax(1, int(m_rules.size()));
    // 180~420 像素之间按行数伸缩：少的时候不留空白，多的时候靠表格内部滚动
    m_table->setFixedHeight(qBound(180, header + rows * 34 + 6, 420));
}

void OverridePage::editRule(int row)
{
    const bool isNew = (row < 0 || row >= m_rules.size());

    ParsedRule initial;
    if (isNew) {
        initial.type   = QStringLiteral("DOMAIN-SUFFIX");
        initial.target = QStringLiteral("DIRECT");
    } else {
        initial = parseRule(m_rules.at(row).text);
    }

    RuleDialog dialog(this, initial, targetNames(), ruleSetNames());
    if (dialog.exec() != QDialog::Accepted) return;

    const QString text = buildRule(dialog.value());
    if (text.isEmpty()) return;

    if (isNew) {
        m_rules.append({text, true});
    } else {
        m_rules[row].text = text;
    }

    rebuildTable();
    if (!isNew) m_table->selectRow(row);
    updateSummary();
    scheduleSave();
}

void OverridePage::removeRule(int row)
{
    if (row < 0 || row >= m_rules.size()) return;
    m_rules.removeAt(row);
    rebuildTable();
    updateSummary();
    scheduleSave();
}

void OverridePage::moveRule(int row, int delta)
{
    const int target = row + delta;
    if (row < 0 || row >= m_rules.size() || target < 0 || target >= m_rules.size()) return;
    m_rules.swapItemsAt(row, target);
    rebuildTable();
    m_table->selectRow(target);
    scheduleSave(); // 顺序即优先级，移动后要重新下发
}

void OverridePage::scheduleSave()
{
    // 这里不写「正在保存…」：防抖期间反复改会一直在两个字之间跳，
    // 只在真正落盘后给一次结果
    m_statusLabel->setStyleSheet(QString());
    if (m_saveTimer) m_saveTimer->start();
}

void OverridePage::persist()
{
    // 保留其他入口维护的 append / delete，本页只管自定义规则与停用状态
    AppContext::RuleOverride value = m_context->ruleOverride();
    value.prepend.clear();
    value.disabled.clear();

    for (const RuleEntry &entry : m_rules) {
        const QString text = entry.text.trimmed();
        if (text.isEmpty()) continue;
        value.prepend << text;
        if (!entry.enabled) value.disabled << text;
    }

    QString error;
    if (!m_context->saveRuleOverride(value, &error)) {
        m_statusLabel->setStyleSheet(QString::fromLatin1(kDangerColor));
        m_statusLabel->setText(error.isEmpty() ? tr("Save failed") : error);
        return;
    }

    // 不 refresh()：内存里的列表就是刚刚写下去的内容，重建表格只会
    // 打断正在进行的选取与滚动位置
    m_statusLabel->setStyleSheet(QString());
    m_statusLabel->setText(tr("Applied automatically: %1 rules, %2 disabled")
                               .arg(value.prepend.size())
                               .arg(value.disabled.size()));
}

} // namespace clashqt
