#include "ui/pages/ProxiesPage.h"

#include "AppContext.h"
#include "config/AppConfig.h"
#include "ui/Icons.h"
#include "ui/UiFactory.h"
#include "utils/AppLogger.h"

#include <QApplication>
#include <QCollator>
#include <QComboBox>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>

#include <algorithm>
#include <utility>

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
constexpr int kRoleTesting       = Qt::UserRole + 6;  // 正在测速（行内显示「测试中」）

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

// 行内的小字号（延迟 / 徽章）。按点值算，控件若用像素字号也能退化处理。
QFont scaledFont(const QFont &base, qreal delta, bool bold = false)
{
    QFont font = base;
    if (base.pointSizeF() > 0) {
        font.setPointSizeF(qMax(7.5, base.pointSizeF() - delta));
    } else if (base.pixelSize() > 0) {
        font.setPixelSize(qMax(10, base.pixelSize() - int(delta * 1.33)));
    }
    font.setBold(bold);
    return font;
}

// 右侧延迟文字。测速进行中优先显示「测试中」，与委托、命中测试共用同一套规则。
QString delayTextFor(bool testing, const QVariant &delayValue)
{
    if (testing) return QStringLiteral("测试中");
    if (!delayValue.isValid()) return QString();
    return delayValue.toInt() < 0 ? QStringLiteral("超时")
                                  : QStringLiteral("%1 ms").arg(delayValue.toInt());
}

// 一行里各元素的矩形。绘制与点击命中**共用这一份排版**，
// 否则「看到的位置」和「点到的位置」迟早会错开。
struct RowGeometry {
    QRect name;
    QRect test;  // 行内测速按钮的热区
    QRect badge;
    QRect delay;
};

constexpr int kTestButtonSize = 20;  // 热区边长（图标 14）
// 同时进行的测速数：每个测速占一个线程 + 一条到节点的连接。开太大在弱网下
// 会互相抢带宽、延迟全偏高；5 个在「68 个节点」这种规模下大约十几秒跑完。
constexpr int kMaxConcurrentTests = 5;
constexpr int kRowPaddingX    = 10; // 与样式表 QListWidget::item 的 padding 对齐

RowGeometry layoutRow(const QRect &itemRect, const QFont &baseFont, const QString &badgeText,
                      const QString &delayText, bool current)
{
    RowGeometry g;
    const QRect row   = itemRect.adjusted(kRowPaddingX, 1, -12, -1);
    const int   midY  = row.center().y();
    int         right = row.right();

    // 延迟占一个**固定宽度**的槽位：若按实际文字宽度右对齐，带延迟的行会把
    // 徽章和测速按钮整体左推，几十行看下来图标是锯齿状。槽宽取三种最长文案。
    const QFont delayFont = scaledFont(baseFont, 1.0);
    const QFontMetrics delayMetrics(delayFont);
    const int slotWidth = qMax(delayMetrics.horizontalAdvance(QStringLiteral("8888 ms")),
                               qMax(delayMetrics.horizontalAdvance(QStringLiteral("测试中")),
                                    delayMetrics.horizontalAdvance(QStringLiteral("超时"))));
    g.delay = QRect(right - slotWidth, row.top(), slotWidth, row.height());
    right -= slotWidth + 10;

    const QFont badgeFont = scaledFont(baseFont, 1.5, true);
    const int   badgeW    = QFontMetrics(badgeFont).horizontalAdvance(badgeText) + 16;
    const int   badgeH    = 18;
    g.badge = QRect(right - badgeW, midY - badgeH / 2, badgeW, badgeH);
    right -= badgeW + 12;

    g.test  = QRect(right - kTestButtonSize, midY - kTestButtonSize / 2, kTestButtonSize,
                    kTestButtonSize);
    right  -= kTestButtonSize + 4;

    const int left = row.left() + (current ? 14 : 0);
    g.name = QRect(left, row.top(), qMax(20, right - left), row.height());
    return g;
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

        const QString  name        = index.data(kRoleName).toString();
        const QString  badge       = index.data(kRoleProtocol).toString();
        const bool     current     = index.data(kRoleCurrent).toBool();
        const bool     unsupported = index.data(kRoleUnsupported).toBool();
        const bool     testing     = index.data(kRoleTesting).toBool();
        const QVariant delayValue  = index.data(kRoleDelay);

        QColor badgeColor = index.data(kRoleProtocolColor).value<QColor>();
        if (!badgeColor.isValid()) badgeColor = ui::mutedColor();

        const ui::Theme &theme   = ui::theme();
        const bool       hovered = option.state & QStyle::State_MouseOver;

        const QString delayText = delayTextFor(testing, delayValue);
        QColor        delayColor = theme.textMuted;
        if (testing) {
            delayColor = theme.accent;
        } else if (!delayText.isEmpty()) {
            const int delay = delayValue.toInt();
            delayColor = theme.danger;
            if (delay >= 0) {
                if (delay < 200) delayColor = QColor(0x2F, 0xA8, 0x7C);      // 绿：直连级
                else if (delay < 500) delayColor = QColor(0xE0, 0x9B, 0x36); // 橙：可用但偏慢
                else delayColor = QColor(0xD9, 0x4B, 0x4B);                  // 红：慢
            }
        }

        const RowGeometry g = layoutRow(option.rect, option.font, badge, delayText, current);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

        // ---- 右端：延迟 / 测试中 ----
        if (!delayText.isEmpty()) {
            painter->setFont(scaledFont(option.font, 1.0));
            painter->setPen(delayColor);
            painter->drawText(g.delay, Qt::AlignRight | Qt::AlignVCenter, delayText);
        }

        // ---- 右端：协议徽章（圆角胶囊）----
        const QColor badgeText = theme.dark ? badgeColor : badgeColor.darker(118);
        QColor       badgeBg   = badgeColor;
        badgeBg.setAlpha(theme.dark ? 56 : 40); // 半透明底：两套主题都不刺眼
        painter->setPen(Qt::NoPen);
        painter->setBrush(badgeBg);
        painter->drawRoundedRect(g.badge, 9, 9);
        painter->setFont(scaledFont(option.font, 1.5, true));
        painter->setPen(badgeText);
        painter->drawText(g.badge, Qt::AlignCenter, badge);

        // ---- 行内测速按钮：平时淡色，悬停到该行时点亮 ----
        const QPixmap bolt = ui::icons::pixmap(
            QStringLiteral("bolt"), hovered ? theme.accent : theme.textMuted, 14);
        if (!bolt.isNull()) {
            painter->drawPixmap(
                g.test.center() - QPoint(bolt.width() / 2, bolt.height() / 2), bolt);
        }

        // ---- 左端：当前节点圆点 + 名称 ----
        if (current) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(theme.accent);
            painter->drawEllipse(QPointF(g.name.left() - 10, g.name.center().y() + 0.5), 3.2,
                                 3.2);
        }

        QFont nameFont = option.font;
        if (current) nameFont.setBold(true);
        painter->setFont(nameFont);
        painter->setPen(unsupported ? theme.textMuted : theme.text);

        // 名称过长时从中间省略：节点名常常是「地区 + 后缀」，两端都有信息
        const QString elided =
            QFontMetrics(nameFont).elidedText(name, Qt::ElideMiddle, g.name.width());
        painter->drawText(g.name, Qt::AlignLeft | Qt::AlignVCenter, elided);

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

    // ---- 排序：只影响显示顺序，不改变订阅里成员的原始排列 -----------------
    toolbar->addWidget(new QLabel(QStringLiteral("排序"), this));
    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItem(QStringLiteral("默认顺序"), int(SortMode::Default));
    m_sortCombo->addItem(QStringLiteral("名称"), int(SortMode::Name));
    m_sortCombo->addItem(QStringLiteral("类型"), int(SortMode::Type));
    m_sortCombo->addItem(QStringLiteral("延迟"), int(SortMode::Delay));
    m_sortCombo->setToolTip(
        QStringLiteral("按名称、协议类型或最近一次测速延迟排列节点。\n"
                       "延迟按升序，未测与超时排在最后；切换排序不会改变当前选中的节点。"));
    // 恢复上次的排序选择；配置里没有或值无效时回到默认顺序
    m_sortMode = sortModeFromKey(m_context->appConfig()->proxySort());
    const int savedSort = m_sortCombo->findData(int(m_sortMode));
    m_sortCombo->setCurrentIndex(savedSort >= 0 ? savedSort : 0);
    toolbar->addWidget(m_sortCombo);

    m_testButton = new QPushButton(QStringLiteral("测试本组"), this);
    m_testAllButton = new QPushButton(QStringLiteral("测试全部组"), this);

    ui::decorate(m_testButton, QStringLiteral("bolt"));
    ui::decorate(m_testAllButton, QStringLiteral("speed"));
    toolbar->addWidget(m_testButton);
    toolbar->addWidget(m_testAllButton);

    root->addLayout(toolbar);

    // ---- 成员列表 ---------------------------------------------------------
    auto *card = ui::makeCard(QStringLiteral("节点"), this);
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_memberList = new QListWidget(card);
    m_memberList->setAlternatingRowColors(true);
    // 协议徽章与延迟由委托绘制（见文件顶部 ProxyRowDelegate）
    m_memberList->setItemDelegate(new ProxyRowDelegate(m_memberList));
    m_memberList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 行内测速按钮是绘制出来的，需要 hover 状态才能点亮；点击则走 eventFilter
    m_memberList->viewport()->setMouseTracking(true);
    m_memberList->viewport()->installEventFilter(this);
    cardLayout->addWidget(m_memberList);
    root->addWidget(card, 1);

    QObject::connect(m_groupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &ProxiesPage::onGroupChanged);
    QObject::connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &ProxiesPage::onSortChanged);
    QObject::connect(m_memberList, &QListWidget::itemDoubleClicked, this,
                     &ProxiesPage::onItemActivated);
    QObject::connect(m_testButton, &QPushButton::clicked, this, &ProxiesPage::onTestCurrent);
    QObject::connect(m_testAllButton, &QPushButton::clicked, this, &ProxiesPage::onTestAll);

    // 内核状态变化时自动刷新（例如切换节点后后端回写）
    QObject::connect(m_context->hub(), &core::Hub::configChanged, this,
                     &ProxiesPage::rebuildGroups);

    // 单节点测速结果（在 Hub 的线程里测完，回到界面线程发信号）
    QObject::connect(m_context->hub(), &core::Hub::delayTested, this,
                     [this](const QString &name, int delayMs, const QString &error) {
                         if (m_activeTests > 0) --m_activeTests;
                         finishTest(name, delayMs >= 0 ? delayMs : -1, error);
                     });
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
    // 重建列表（配置重载、批量测速收尾都会走到这里）时保留滚动位置，
    // 否则几十个节点的列表每次都被甩回顶部
    const int scrollValue = m_memberList->verticalScrollBar()->value();

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
        item->setData(kRoleTesting, m_testing.contains(name));

        QStringList tip;
        tip << name;
        tip << (isGroup ? QStringLiteral("代理组 · %1").arg(type)
                        : QStringLiteral("协议 · %1").arg(
                              type.isEmpty() ? QStringLiteral("未知") : type));
        if (unsupported) {
            tip << QStringLiteral("当前实现尚不支持该协议，选中后连接会明确失败");
        }

        const QJsonArray history = member.value(QStringLiteral("history")).toArray();
        if (m_failedTests.contains(name)) {
            item->setData(kRoleDelay, -1);
            tip << QStringLiteral("最近一次测速超时");
        } else if (!history.isEmpty()) {
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
        tip << QStringLiteral("点击行内闪电图标可单独测速");

        item->setToolTip(tip.join(QLatin1Char('\n')));
    }

    // 排序必须在成员全部建好之后做：比较依据是行里的名称/类型/延迟角色
    sortRows();

    m_memberList->verticalScrollBar()->setValue(scrollValue);
}

void ProxiesPage::onSortChanged(int index)
{
    if (!m_sortCombo || index < 0) return;

    m_sortMode = SortMode(m_sortCombo->itemData(index).toInt());

    // 写入应用配置并立即落盘：重启后仍按上次选择排列
    if (AppConfig *config = m_context->appConfig()) {
        config->setProxySort(sortKey(m_sortMode));
        config->save();
    }

    // 重建一次列表：这样切回「默认顺序」时能精确回到订阅里的原始排列，
    // 而不是把上一次排序的结果当成原始顺序。
    onGroupChanged(m_groupCombo->currentIndex());
}

QString ProxiesPage::sortKey(SortMode mode)
{
    switch (mode) {
    case SortMode::Name:  return QStringLiteral("name");
    case SortMode::Type:  return QStringLiteral("type");
    case SortMode::Delay: return QStringLiteral("delay");
    case SortMode::Default:
    default:              return QStringLiteral("default");
    }
}

ProxiesPage::SortMode ProxiesPage::sortModeFromKey(const QString &key)
{
    const QString normalized = key.trimmed().toLower();
    if (normalized == QLatin1String("name")) return SortMode::Name;
    if (normalized == QLatin1String("type")) return SortMode::Type;
    if (normalized == QLatin1String("delay")) return SortMode::Delay;
    return SortMode::Default;
}

void ProxiesPage::sortRows()
{
    if (m_sortMode == SortMode::Default) return;

    const int count = m_memberList->count();
    if (count < 2) return;

    QVector<QListWidgetItem *> rows;
    rows.reserve(count);
    while (m_memberList->count() > 0) rows << m_memberList->takeItem(0);

    // 名称按区域规则比较，并开启数字模式：节点 2 排在节点 10 之前
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);

    const auto nameOf = [](QListWidgetItem *item) {
        return item->data(kRoleName).toString();
    };
    const auto compareName = [&collator, &nameOf](QListWidgetItem *a, QListWidgetItem *b) {
        return collator.compare(nameOf(a), nameOf(b));
    };

    // 延迟排序键：0=有测速值（按毫秒升序），1=未测，2=超时。
    // 未测与超时没有可比较的大小，统一沉到列表末尾，避免它们在顶部来回跳。
    const auto delayKey = [](QListWidgetItem *item) {
        const QVariant value = item->data(kRoleDelay);
        if (!value.isValid()) return std::pair{1, 0};
        const int delay = value.toInt();
        if (delay < 0) return std::pair{2, 0};
        return std::pair{0, delay};
    };

    std::stable_sort(rows.begin(), rows.end(), [&](QListWidgetItem *a, QListWidgetItem *b) {
        switch (m_sortMode) {
        case SortMode::Name:
            return compareName(a, b) < 0;

        case SortMode::Type: {
            const int cmp = collator.compare(a->data(kRoleProtocol).toString(),
                                             b->data(kRoleProtocol).toString());
            if (cmp != 0) return cmp < 0;
            return compareName(a, b) < 0; // 同类型内按名称，结果稳定可预期
        }

        case SortMode::Delay: {
            const auto ka = delayKey(a);
            const auto kb = delayKey(b);
            if (ka.first != kb.first) return ka.first < kb.first;
            if (ka.second != kb.second) return ka.second < kb.second;
            return compareName(a, b) < 0; // 延迟相同按名称
        }

        case SortMode::Default:
        default:
            return false;
        }
    });

    for (QListWidgetItem *item : rows) m_memberList->addItem(item);
}

bool ProxiesPage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_memberList->viewport()) return QWidget::eventFilter(watched, event);

    switch (event->type()) {
    case QEvent::MouseButtonRelease: {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() != Qt::LeftButton) break;
        const QString name = testButtonAt(mouse->position().toPoint());
        if (!name.isEmpty()) {
            testSingleNode(name);
            return true; // 吞掉：点按钮不应该同时触发「切换节点」
        }
        break;
    }
    case QEvent::MouseMove: {
        auto *mouse = static_cast<QMouseEvent *>(event);
        const bool over = !testButtonAt(mouse->position().toPoint()).isEmpty();
        const Qt::CursorShape shape = over ? Qt::PointingHandCursor : Qt::ArrowCursor;
        if (m_memberList->viewport()->cursor().shape() != shape) {
            m_memberList->viewport()->setCursor(shape);
        }
        break;
    }
    case QEvent::Leave:
        m_memberList->viewport()->unsetCursor();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(watched, event);
}

QString ProxiesPage::testButtonAt(const QPoint &pos) const
{
    QListWidgetItem *item = m_memberList->itemAt(pos);
    if (!item) return QString();

    const QString name = item->data(kRoleName).toString();
    if (name.isEmpty()) return QString();

    const QString  badge = item->data(kRoleProtocol).toString();
    const RowGeometry g =
        layoutRow(m_memberList->visualItemRect(item), m_memberList->font(), badge,
                  delayTextFor(item->data(kRoleTesting).toBool(), item->data(kRoleDelay)),
                  item->data(kRoleCurrent).toBool());

    // 热区比图标略大，20px 的图标直接点太考验准头
    return g.test.adjusted(-4, -4, 4, 4).contains(pos) ? name : QString();
}

void ProxiesPage::testSingleNode(const QString &name)
{
    if (name.isEmpty() || m_testing.contains(name) || m_testQueue.contains(name)) return;
    enqueueTest(name);
    startNextTests();
}

void ProxiesPage::enqueueTest(const QString &name)
{
    if (name.isEmpty() || m_testing.contains(name) || m_testQueue.contains(name)) return;

    m_testing.insert(name);
    m_testQueue.enqueue(name);

    // 入队就切「测试中」：批量测速时几十个节点排着队，等轮到才变状态
    // 会让人以为按钮没反应
    for (int row = 0; row < m_memberList->count(); ++row) {
        QListWidgetItem *item = m_memberList->item(row);
        if (item->data(kRoleName).toString() != name) continue;
        item->setData(kRoleTesting, true);
        m_memberList->viewport()->update(m_memberList->visualItemRect(item));
        break;
    }
}

void ProxiesPage::startNextTests()
{
    while (m_activeTests < kMaxConcurrentTests && !m_testQueue.isEmpty()) {
        const QString name = m_testQueue.dequeue();

        if (m_context->hub()->testProxyDelayAsync(
                name, m_context->appConfig()->delayTestUrl(),
                m_context->appConfig()->delayTestTimeout())) {
            ++m_activeTests;
            continue;
        }

        // 排队期间配置被重载、节点消失：直接记为失败，别让它一直挂着
        finishTest(name, -1, QStringLiteral("节点已不存在"));
    }
}

void ProxiesPage::finishTest(const QString &name, int delayMs, const QString &error)
{
    m_testing.remove(name);
    if (delayMs >= 0) {
        m_failedTests.remove(name);
    } else {
        m_failedTests.insert(name);
    }
    setRowDelay(name, delayMs >= 0 ? delayMs : -1);

    if (m_batchNames.remove(name)) {
        if (delayMs >= 0) ++m_batchOk;

        // 批量任务：中途失败不逐条弹窗（几十个节点会刷屏），只在全部结束时汇总。
        // 批量测速只回填各行的延迟，不改动当前选中的节点，因此不需要重建列表。
        if (m_batchNames.isEmpty()) {
            const int total = m_batchTotal;
            const int ok    = m_batchOk;
            m_batchTotal = 0;
            m_batchOk    = 0;

            // 按延迟排序时，测速结果全部回填后重排一次；批量中途不动列表，
            // 否则每测完一个节点整张表都会跳一下。
            if (m_sortMode == SortMode::Delay) sortRows();

            m_context->showToast(
                QStringLiteral("info"),
                QStringLiteral("测速完成：%1/%2 个节点可用").arg(ok).arg(total));
        }
    } else if (!error.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"),
                             QStringLiteral("%1 测速失败：%2").arg(name, error));
    }

    startNextTests();
}

void ProxiesPage::startBatch(const QStringList &names)
{
    QStringList accepted;
    for (const QString &name : names) {
        if (name.isEmpty() || m_testing.contains(name) || m_testQueue.contains(name)) continue;
        accepted << name;
    }

    if (accepted.isEmpty()) {
        m_context->showToast(QStringLiteral("info"),
                             QStringLiteral("这些节点正在测速，或没有可测的节点"));
        return;
    }

    m_batchNames = QSet<QString>(accepted.begin(), accepted.end());
    m_batchTotal = accepted.size();
    m_batchOk    = 0;

    for (const QString &name : accepted) enqueueTest(name);
    startNextTests();

    m_context->showToast(
        QStringLiteral("info"),
        QStringLiteral("开始测试 %1 个节点（同时 %2 个，结果逐个回填）")
            .arg(accepted.size())
            .arg(kMaxConcurrentTests));
}

void ProxiesPage::setRowDelay(const QString &name, int delayMs)
{
    for (int row = 0; row < m_memberList->count(); ++row) {
        QListWidgetItem *item = m_memberList->item(row);
        if (item->data(kRoleName).toString() != name) continue;
        item->setData(kRoleTesting, false);
        item->setData(kRoleDelay, delayMs);
        m_memberList->viewport()->update(m_memberList->visualItemRect(item));
        break;
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

void ProxiesPage::onTestCurrent()
{
    if (m_currentGroup.isEmpty()) return;

    // 只测当前组里列出来的节点；协议未实现的跳过——它们必然失败，
    // 只会给最后的汇总数字添噪声
    QStringList names;
    for (int row = 0; row < m_memberList->count(); ++row) {
        QListWidgetItem *item = m_memberList->item(row);
        if (item->data(kRoleUnsupported).toBool()) continue;
        const QString name = item->data(kRoleName).toString();
        if (!name.isEmpty()) names << name;
    }
    startBatch(names);
}

void ProxiesPage::onTestAll()
{
    // 所有组的成员取并集（同一个节点常被多个组引用，按名字去重）
    const QJsonObject proxies =
        m_context->hub()->proxiesStatus().value(QStringLiteral("proxies")).toObject();

    QStringList   names;
    QSet<QString> seen;
    for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
        const QJsonObject entry = it.value().toObject();
        if (!entry.contains(QStringLiteral("all"))) continue;

        for (const QJsonValue &value : entry.value(QStringLiteral("all")).toArray()) {
            const QString name = value.toString();
            if (name.isEmpty() || seen.contains(name)) continue;

            const QJsonObject member = proxies.value(name).toObject();
            if (member.contains(QStringLiteral("unsupported"))) continue;

            seen.insert(name);
            names << name;
        }
    }

    startBatch(names);
}

} // namespace clashqt
