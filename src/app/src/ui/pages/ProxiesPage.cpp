#include "ui/pages/ProxiesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"

#include <QApplication>
#include <QComboBox>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

#include "hub/Hub.h"

namespace clashqt {
namespace {

// ---------------------------------------------------------------------------
//  列表行的数据角色
//
//  行不再靠文本承载信息：名称、协议徽章、延迟各自放在角色里，由下面的委托
//  画出来。切换节点时只读 kRoleName，不受显示格式影响。
// ---------------------------------------------------------------------------
constexpr int kRoleName          = Qt::UserRole;      // 真实成员名（切换代理用）
constexpr int kRoleProtocol      = Qt::UserRole + 1;  // 徽章文字：VLESS / HY2 / 组
constexpr int kRoleProtocolColor = Qt::UserRole + 2;  // 徽章主色
constexpr int kRoleDelay         = Qt::UserRole + 3;  // 毫秒；缺失=未测，-1=超时
constexpr int kRoleCurrent       = Qt::UserRole + 4;  // 当前选中
constexpr int kRoleUnsupported   = Qt::UserRole + 5;  // 协议尚未实现

struct ProtocolBadge {
    QString text;
    QColor  color;
};

// 协议 → 徽章。按传输族上色，扫一眼就能分出几类：
//   TLS 隧道（VLESS / VMess / Trojan）  紫 / 靛 / 橙
//   QUIC 家族（HY / HY2 / TUIC）        粉 / 青
//   经典加密（SS / SSR / Snell）        蓝 / 灰蓝
//   直连与拒绝                          石板灰 / 红
// 未收录的协议回退成「原名大写 + 中性灰」，保证每行都有标签，不留空。
ProtocolBadge badgeFor(const QString &type, bool group)
{
    if (group) return {QStringLiteral("组"), ui::accent()};

    static const QHash<QString, ProtocolBadge> kBadges = {
        {QStringLiteral("Vless"),        {QStringLiteral("VLESS"), QColor(0x8B, 0x5C, 0xF6)}},
        {QStringLiteral("Vmess"),        {QStringLiteral("VMESS"), QColor(0x63, 0x66, 0xF1)}},
        {QStringLiteral("Trojan"),       {QStringLiteral("TROJAN"), QColor(0xF5, 0x9E, 0x0B)}},
        {QStringLiteral("Shadowsocks"),  {QStringLiteral("SS"), QColor(0x0E, 0xA5, 0xE9)}},
        {QStringLiteral("ShadowsocksR"), {QStringLiteral("SSR"), QColor(0x02, 0x84, 0xC7)}},
        {QStringLiteral("Snell"),        {QStringLiteral("SNELL"), QColor(0x64, 0x74, 0x8B)}},
        {QStringLiteral("Hysteria"),     {QStringLiteral("HY"), QColor(0xEC, 0x48, 0x99)}},
        {QStringLiteral("Hysteria2"),    {QStringLiteral("HY2"), QColor(0xEC, 0x48, 0x99)}},
        {QStringLiteral("Tuic"),         {QStringLiteral("TUIC"), QColor(0x14, 0xB8, 0xA6)}},
        {QStringLiteral("WireGuard"),    {QStringLiteral("WG"), QColor(0xEF, 0x44, 0x44)}},
        {QStringLiteral("AnyTLS"),       {QStringLiteral("ANYTLS"), QColor(0xA8, 0x55, 0xF7)}},
        {QStringLiteral("Socks5"),       {QStringLiteral("SOCKS5"), QColor(0x94, 0xA3, 0xB8)}},
        {QStringLiteral("Http"),         {QStringLiteral("HTTP"), QColor(0x94, 0xA3, 0xB8)}},
        {QStringLiteral("Ssh"),          {QStringLiteral("SSH"), QColor(0x94, 0xA3, 0xB8)}},
        {QStringLiteral("Direct"),       {QStringLiteral("DIRECT"), QColor(0x64, 0x74, 0x8B)}},
        {QStringLiteral("Reject"),       {QStringLiteral("REJECT"), QColor(0xEF, 0x44, 0x44)}},
        {QStringLiteral("RejectDrop"),   {QStringLiteral("REJECT"), QColor(0xEF, 0x44, 0x44)}},
    };

    const auto it = kBadges.constFind(type);
    if (it != kBadges.constEnd()) return *it;
    return {type.isEmpty() ? QStringLiteral("节点") : type.toUpper(), ui::mutedColor()};
}

// ---------------------------------------------------------------------------
//  行绘制：当前标记 + 名称 + 协议徽章 + 延迟
//
//  背景仍交给样式表（选中 / 悬停 / 斑马纹），这里只清掉文字后自己排内容，
//  这样列表的交互与主题（浅色 / 深色）都不需要另外维护一套样式。
// ---------------------------------------------------------------------------
class ProxyRowDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        // 背景 / 选中 / 悬停 / 斑马纹交给样式表，但**不能**调
        // QStyledItemDelegate::paint —— 它内部会再次 initStyleOption，
        // 把已经清空的文字重新填回模型里的节点名，结果每行文字画两遍（残影）。
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.text.clear();
        opt.icon = QIcon();
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

        const QString name = index.data(kRoleName).toString();
        const QString badge = index.data(kRoleProtocol).toString();
        const bool    current = index.data(kRoleCurrent).toBool();
        const bool    unsupported = index.data(kRoleUnsupported).toBool();
        const QVariant delayValue = index.data(kRoleDelay);

        QColor badgeColor = index.data(kRoleProtocolColor).value<QColor>();
        if (!badgeColor.isValid()) badgeColor = ui::mutedColor();

        const ui::Theme &theme = ui::theme();

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        // 与样式表里 QListWidget::item 的左右 padding 对齐
        const QRect row  = option.rect.adjusted(10, 1, -12, -1);
        const int   midY = row.center().y();

        // ---- 右端：延迟 ----
        int right = row.right();
        if (delayValue.isValid()) {
            const int     delay = delayValue.toInt();
            const QString text  = delay < 0 ? QStringLiteral("超时")
                                            : QStringLiteral("%1 ms").arg(delay);
            QColor        color = theme.danger;
            if (delay >= 0) {
                if (delay < 200) color = QColor(0x2F, 0xA8, 0x7C);      // 绿：直连级
                else if (delay < 500) color = QColor(0xE0, 0x9B, 0x36); // 橙：可用但偏慢
                else color = QColor(0xD9, 0x4B, 0x4B);                  // 红：慢
            }

            QFont font = option.font;
            font.setPointSizeF(qMax(7.5, font.pointSizeF() - 1.0));
            painter->setFont(font);
            const int width = QFontMetrics(font).horizontalAdvance(text);
            painter->setPen(color);
            painter->drawText(QRect(right - width, row.top(), width, row.height()),
                              Qt::AlignRight | Qt::AlignVCenter, text);
            right -= width + 14;
        }

        // ---- 右端：协议徽章（圆角胶囊）----
        QFont badgeFont = option.font;
        badgeFont.setPointSizeF(qMax(7.5, badgeFont.pointSizeF() - 1.5));
        badgeFont.setBold(true);
        const int   badgeW = QFontMetrics(badgeFont).horizontalAdvance(badge) + 16;
        const int   badgeH = 18;
        const QRect badgeRect(right - badgeW, midY - badgeH / 2, badgeW, badgeH);

        // 浅色底上把文字压深一档，否则 9pt 的彩色字对比度偏低
        const QColor badgeText = theme.dark ? badgeColor : badgeColor.darker(118);
        QColor       badgeBg   = badgeColor;
        badgeBg.setAlpha(theme.dark ? 56 : 40); // 半透明底：两套主题都不刺眼
        painter->setPen(Qt::NoPen);
        painter->setBrush(badgeBg);
        painter->drawRoundedRect(badgeRect, badgeH / 2.0, badgeH / 2.0);
        painter->setFont(badgeFont);
        painter->setPen(badgeText);
        painter->drawText(badgeRect, Qt::AlignCenter, badge);

        // ---- 左端：当前节点圆点 + 名称 ----
        int left = row.left();
        if (current) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(theme.accent);
            painter->drawEllipse(QPointF(left + 4, midY + 0.5), 3.2, 3.2);
            left += 14;
        }

        QFont nameFont = option.font;
        if (current) nameFont.setBold(true);
        painter->setFont(nameFont);
        painter->setPen(unsupported ? theme.textMuted : theme.text);

        // 名称过长时从中间省略：节点名常常是「地区 + 后缀」，两端都有信息
        const int nameWidth = qMax(20, badgeRect.left() - 10 - left);
        const QString elided =
            QFontMetrics(nameFont).elidedText(name, Qt::ElideMiddle, nameWidth);
        painter->drawText(QRect(left, row.top(), nameWidth, row.height()),
                          Qt::AlignLeft | Qt::AlignVCenter, elided);

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 30)); // 给徽章留出上下呼吸
        return size;
    }
};

} // namespace

ProxiesPage::ProxiesPage(AppContext *context, QWidget *parent)
    : QWidget(parent), m_context(context)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    root->addWidget(ui::makePageHeader(
        QStringLiteral("代理"),
        QStringLiteral("选择代理组中的节点。右侧徽章是协议类型，"
                       "颜色相同的节点属于同一传输族；GLOBAL 组在全局模式下决定所有流量的出口。")));

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
    // 协议徽章与延迟由委托绘制（见文件顶部 ProxyRowDelegate）
    m_memberList->setItemDelegate(new ProxyRowDelegate(m_memberList));
    m_memberList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
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

    // 当前节点在列表里用圆点 + 加粗标出，这里不再重复写名字：
    // 节点名普遍很长，塞进工具条会挤成两三行，把按钮推得上下不对齐。
    m_summaryLabel->setText(
        QStringLiteral("类型 %1 · 共 %2 个成员")
            .arg(type.isEmpty() ? QStringLiteral("-") : type)
            .arg(group.value(QStringLiteral("all")).toArray().size()));
    m_summaryLabel->setToolTip(now.isEmpty()
                                   ? QString()
                                   : QStringLiteral("当前节点：%1").arg(now));

    const QJsonArray members = group.value(QStringLiteral("all")).toArray();
    for (const QJsonValue &value : members) {
        const QString name = value.toString();
        if (name.isEmpty()) continue;

        m_memberNames << name;

        const QJsonObject member = proxies.value(name).toObject();
        const QString     type   = member.value(QStringLiteral("type")).toString();

        // 成员也可能是另一个代理组（组里套组）：它没有 history，也没有协议
        const bool          isGroup     = member.contains(QStringLiteral("all"));
        const bool          unsupported = member.contains(QStringLiteral("unsupported"));
        const ProtocolBadge badge       = badgeFor(type, isGroup);

        auto *item = new QListWidgetItem(name, m_memberList);
        // 显示文本仅用于无障碍与检索，实际内容由 ProxyRowDelegate 绘制
        item->setData(kRoleName, name);
        item->setData(kRoleProtocol, badge.text);
        item->setData(kRoleProtocolColor, badge.color);
        item->setData(kRoleCurrent, name == now);
        item->setData(kRoleUnsupported, unsupported);

        QStringList tip;
        tip << name;
        tip << (isGroup ? QStringLiteral("代理组 · %1").arg(type)
                        : QStringLiteral("协议 · %1").arg(
                              type.isEmpty() ? QStringLiteral("未知") : type));
        if (unsupported) {
            tip << QStringLiteral("当前实现尚不支持该协议，选中后连接会明确失败");
        }

        const QJsonArray history = member.value(QStringLiteral("history")).toArray();
        if (!history.isEmpty()) {
            const int delay = history.last().toObject().value(QStringLiteral("delay")).toInt();
            // 与参照实现一致：<=0 视为超时（history 里超时通常记 0 或负数）
            item->setData(kRoleDelay, delay > 0 ? delay : -1);
            tip << (delay > 0 ? QStringLiteral("上次延迟 %1 ms").arg(delay)
                              : QStringLiteral("上次测试超时"));
        }

        if (isGroup) {
            const QString inner = member.value(QStringLiteral("now")).toString();
            if (!inner.isEmpty()) tip << QStringLiteral("当前指向 %1").arg(inner);
        }

        item->setToolTip(tip.join(QLatin1Char('\n')));
    }
}

void ProxiesPage::onItemActivated()
{
    QListWidgetItem *item = m_memberList->currentItem();
    if (!item || m_currentGroup.isEmpty()) return;

    const QString memberName = item->data(kRoleName).toString();
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
