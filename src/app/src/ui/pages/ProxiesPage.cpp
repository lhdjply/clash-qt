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
    if (group) return {QCoreApplication::translate("clashqt::ProxiesPage", "Group"), ui::accent()};

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
    return {type.isEmpty() ? QCoreApplication::translate("clashqt::ProxiesPage", "Nodes") : type.toUpper(), ui::mutedColor()};
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

// 右侧延迟文字。测速进行中优先显示「测试中」；没测过的显示「测试」，
// 那个位置本身就是重测按钮，与委托、命中测试共用同一套规则。
QString delayTextFor(bool testing, const QVariant &delayValue)
{
    if (testing) return QCoreApplication::translate("clashqt::ProxiesPage", "Testing");
    if (!delayValue.isValid()) return QCoreApplication::translate("clashqt::ProxiesPage", "Test");
    return delayValue.toInt() < 0 ? QCoreApplication::translate("clashqt::ProxiesPage", "Timeout")
                                  : QStringLiteral("%1 ms").arg(delayValue.toInt());
}

// 网格里一格的排版。绘制与点击命中共用这一份矩形，
// 否则「看到的位置」和「点到的位置」迟早会错开。
struct CellGeometry {
    QRect card;
    QRect name;
    QRect badge; // 协议类型胶囊
    QRect delay; // 延迟；没测过时这里显示「测试」
};

// 同时进行的测速数：每个测速占一个线程 + 一条到节点的连接。开太大在弱网下
// 会互相抢带宽、延迟全偏高；5 个在「68 个节点」这种规模下大约十几秒跑完。
constexpr int kMaxConcurrentTests = 5;

constexpr int kCellInsetX   = 3; // 卡片与格子边缘之间的留白
constexpr int kCellInsetY   = 4;
constexpr int kCardPaddingX = 9;
constexpr int kDelayGap     = 4;
constexpr int kBadgeHeight  = 16;
constexpr int kBadgeGap     = 4;

constexpr int kCellHeight   = 52;  // 一行卡片占的高度（含上下留白）
constexpr int kMinCellWidth = 210; // 每格最小宽度，决定一排放几张卡片

// 协议胶囊的字体：比名称小两号，深色底上也不至于糊成一团
QFont badgeFont(const QFont &base) { return scaledFont(base, 2.5, true); }

// 延迟槽的宽度：取三种最长文案，英文的 Timeout 比中文「超时」宽不少，
// 写死数值在英文界面会把字截断
int delaySlotWidth(const QFont &baseFont)
{
    const QFontMetrics metrics(scaledFont(baseFont, 1.0));
    return qMax(metrics.horizontalAdvance(QStringLiteral("8888 ms")),
                qMax(metrics.horizontalAdvance(
                         QCoreApplication::translate("clashqt::ProxiesPage", "Testing")),
                     metrics.horizontalAdvance(
                         QCoreApplication::translate("clashqt::ProxiesPage", "Timeout"))));
}

CellGeometry layoutCell(const QRect &cellRect, const QFont &baseFont, const QString &badgeText)
{
    CellGeometry g;
    g.card = cellRect.adjusted(kCellInsetX, kCellInsetY, -kCellInsetX, -kCellInsetY);

    const int midY  = g.card.center().y();
    int       right = g.card.right() - kCardPaddingX;

    const int slot  = delaySlotWidth(baseFont);
    g.delay = QRect(right - slot, g.card.top(), slot, g.card.height());
    right -= slot + kDelayGap;

    if (!badgeText.isEmpty()) {
        const int width = QFontMetrics(badgeFont(baseFont)).horizontalAdvance(badgeText) + 10;
        g.badge = QRect(right - width, midY - kBadgeHeight / 2, width, kBadgeHeight);
        right -= width + kBadgeGap;
    }

    const int left = g.card.left() + kCardPaddingX;
    g.name = QRect(left, g.card.top(), qMax(20, right - left), g.card.height());
    return g;
}

// ---------------------------------------------------------------------------
//  卡片绘制：名称 + 延迟，选中的卡片用强调色描边与浅底标出
//
//  网格里不使用样式表的 item 背景（见列表的局部样式表），整张卡片由这里
//  画出来，浅色与深色两套主题才能保持同一个观感。
// ---------------------------------------------------------------------------
class ProxyCardDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        const QString  name        = index.data(kRoleName).toString();
        const QString  badge       = index.data(kRoleProtocol).toString();
        const bool     current     = index.data(kRoleCurrent).toBool();
        const bool     unsupported = index.data(kRoleUnsupported).toBool();
        const bool     testing     = index.data(kRoleTesting).toBool();
        const QVariant delayValue  = index.data(kRoleDelay);

        QColor badgeColor = index.data(kRoleProtocolColor).value<QColor>();
        if (!badgeColor.isValid()) badgeColor = ui::mutedColor();

        const ui::Theme &theme   = ui::theme();
        const bool       hovered = (option.state & QStyle::State_MouseOver) != 0;

        const QString delayText = delayTextFor(testing, delayValue);
        const bool    untested  = !testing && !delayValue.isValid();

        QColor delayColor = theme.textMuted;
        if (testing) {
            delayColor = theme.accent;
        } else if (untested) {
            // 没测过的就是一颗按钮，指着这张卡时才点亮
            delayColor = hovered ? theme.accent : theme.textMuted;
        } else {
            const int delay = delayValue.toInt();
            delayColor = theme.danger;
            if (delay >= 0) {
                if (delay < 200) delayColor = QColor(0x2F, 0xA8, 0x7C);      // 绿：直连级
                else if (delay < 500) delayColor = QColor(0xE0, 0x9B, 0x36); // 橙：可用但偏慢
                else delayColor = QColor(0xD9, 0x4B, 0x4B);                  // 红：慢
            }
        }

        const CellGeometry g = layoutCell(option.rect, option.font, badge);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setClipRect(option.rect);

        // ---- 卡片底：当前节点用强调色描边 + 浅底，悬停时换一边框色 ----
        const QColor border = current ? theme.accent
                                      : (hovered ? theme.cardBorder : theme.separator);
        painter->setPen(QPen(border, current ? 1.6 : 1.0));
        painter->setBrush(current ? theme.accentSoft : (hovered ? theme.hover : theme.card));
        painter->drawRoundedRect(QRectF(g.card).adjusted(0.5, 0.5, -0.5, -0.5), 7, 7);

        // ---- 右端：延迟 / 超时 / 测试中；没测过则是「测试」按钮 ----
        if (!delayText.isEmpty()) {
            painter->setFont(scaledFont(option.font, 1.0));
            painter->setPen(delayColor);
            painter->drawText(g.delay, Qt::AlignRight | Qt::AlignVCenter, delayText);
        }

        // ---- 协议胶囊：颜色区分传输族，和列表模式下是同一套映射 ----
        if (!g.badge.isNull()) {
            QColor background = badgeColor;
            background.setAlpha(theme.dark ? 56 : 40);
            painter->setPen(Qt::NoPen);
            painter->setBrush(background);
            painter->drawRoundedRect(g.badge, kBadgeHeight / 2.0, kBadgeHeight / 2.0);

            painter->setFont(badgeFont(option.font));
            painter->setPen(theme.dark ? badgeColor : badgeColor.darker(118));
            painter->drawText(g.badge, Qt::AlignCenter, badge);
        }

        // ---- 名称：当前节点加粗，长名字从中间省略 ----
        QFont nameFont = option.font;
        if (current) nameFont.setBold(true);
        painter->setFont(nameFont);
        painter->setPen(unsupported ? theme.textMuted : theme.text);

        // 节点名普遍是「地区 + 线路」的长名字，从右侧省略能保住地区前缀
        const QString elided =
            QFontMetrics(nameFont).elidedText(name, Qt::ElideRight, g.name.width());
        painter->drawText(g.name, Qt::AlignLeft | Qt::AlignVCenter, elided);

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const override
    {
        // 必须与 gridSize 一致，否则整格按这个尺寸排、卡片只画一小块
        if (const auto *view = qobject_cast<const QListView *>(option.widget)) {
            if (view->gridSize().width() > 0) return view->gridSize();
        }
        return QSize(kMinCellWidth, kCellHeight);
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
        tr("Proxies"),
        tr("Pick a node from the proxy group: click a card to switch to it. The text on the right is the last measured latency; click it to test that node again, or click Test on a node that has not been measured yet. In global mode, the GLOBAL group decides the exit for all traffic."), QStringLiteral("proxy")));

    // ---- 顶部操作条 -------------------------------------------------------
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(8);

    toolbar->addWidget(new QLabel(tr("Proxy group"), this));
    m_groupCombo = new QComboBox(this);
    m_groupCombo->setMinimumWidth(150);
    toolbar->addWidget(m_groupCombo);

    // 组的类型、成员数与当前节点放进下拉框的悬停提示：这几项在多语言下
    // 很容易把工具条撑得比窗口还宽（德语、俄语尤其明显），而右侧的测速
    // 按钮又不能压缩，所以这里不再单占一个标签
    toolbar->addStretch(1);

    // ---- 排序：只影响显示顺序，不改变订阅里成员的原始排列 -----------------
    toolbar->addWidget(new QLabel(tr("Sort"), this));
    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItem(tr("Default order"), int(SortMode::Default));
    m_sortCombo->addItem(tr("Name"), int(SortMode::Name));
    m_sortCombo->addItem(tr("Type"), int(SortMode::Type));
    m_sortCombo->addItem(tr("Latency"), int(SortMode::Delay));
    m_sortCombo->setToolTip(
        tr("Sort nodes by name, protocol type or latest measured latency.\nLatency sorts ascending; untested and timed-out nodes go last. Sorting never changes the selected node."));
    // 恢复上次的排序选择；配置里没有或值无效时回到默认顺序
    m_sortMode = sortModeFromKey(m_context->appConfig()->proxySort());
    const int savedSort = m_sortCombo->findData(int(m_sortMode));
    m_sortCombo->setCurrentIndex(savedSort >= 0 ? savedSort : 0);
    toolbar->addWidget(m_sortCombo);

    m_testButton = new QPushButton(tr("Test this group"), this);
    m_testAllButton = new QPushButton(tr("Test all groups"), this);

    ui::decorate(m_testButton, QStringLiteral("bolt"));
    ui::decorate(m_testAllButton, QStringLiteral("speed"));
    toolbar->addWidget(m_testButton);
    toolbar->addWidget(m_testAllButton);

    root->addLayout(toolbar);

    // ---- 成员列表 ---------------------------------------------------------
    auto *card = ui::makeCard(tr("Nodes"), this, QStringLiteral("server"));
    auto *cardLayout = qobject_cast<QVBoxLayout *>(card->layout());

    m_memberList = new QListWidget(card);
    // 节点按卡片排成网格：列数随窗口宽度变化，格子大小由 updateGridSize 统一
    m_memberList->setViewMode(QListView::IconMode);
    m_memberList->setResizeMode(QListView::Adjust);
    m_memberList->setMovement(QListView::Static);
    m_memberList->setFlow(QListView::LeftToRight);
    m_memberList->setWrapping(true);
    m_memberList->setSpacing(0);
    m_memberList->setUniformItemSizes(false); // 列宽随窗口变，不能缓存第一次的尺寸
    m_memberList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_memberList->setAlternatingRowColors(false);
    m_memberList->setItemDelegate(new ProxyCardDelegate(m_memberList));
    m_memberList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_memberList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 卡片底色与选中描边都由委托绘制，这里把样式表给的 item 背景压掉
    m_memberList->setStyleSheet(QStringLiteral(
        "QListWidget { background: transparent; border: 0px; outline: 0px; }"
        "QListWidget::item { padding: 0px; border: 0px; }"
        "QListWidget::item:hover { background: transparent; }"
        "QListWidget::item:selected { background: transparent; }"));
    // 卡片里的测速按钮是绘制出来的，需要 hover 状态才能点亮；点击则走 eventFilter
    m_memberList->viewport()->setMouseTracking(true);
    m_memberList->viewport()->installEventFilter(this);
    cardLayout->addWidget(m_memberList);
    root->addWidget(card, 1);
    updateGridSize();

    QObject::connect(m_groupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &ProxiesPage::onGroupChanged);
    QObject::connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     &ProxiesPage::onSortChanged);
    // 网格里点一下卡片就切过去，双击同样有效（有人习惯双击）
    QObject::connect(m_memberList, &QListWidget::itemClicked, this,
                     [this](QListWidgetItem *item) { activateItem(item); });
    QObject::connect(m_memberList, &QListWidget::itemDoubleClicked, this,
                     [this](QListWidgetItem *item) { activateItem(item); });
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

    // GLOBAL 是内核自带的组，只在全局模式下参与路由；规则/直连模式下把它
    // 列出来只会误导（点它不会影响分流）。核心始终提供它（/proxies 与参照
    // 实现一致），这里按模式决定是否展示。
    const bool globalMode = m_context->outboundMode() == QLatin1String("global");

    // 收集代理组：含有 all 字段的条目即为组
    QStringList groups;
    for (auto it = proxies.constBegin(); it != proxies.constEnd(); ++it) {
        const QString name = it.key();
        if (name == QLatin1String("GLOBAL") && !globalMode) continue;
        const QJsonObject entry = it.value().toObject();
        if (entry.contains(QStringLiteral("all")) && !entry.contains(QStringLiteral("unsupported"))) {
            groups << name;
        }
    }

    // GLOBAL 始终置顶，便于快速切换全局出口
    groups.sort();
    const int globalIndex = groups.indexOf(QStringLiteral("GLOBAL"));
    if (globalIndex > 0) groups.move(globalIndex, 0);

    // 模式切换时把当前组挪到该模式下真正生效的那一个：切进全局模式直接停在
    // GLOBAL，切回规则/直连模式则回落到第一个配置组（GLOBAL 已不在列表里，
    // 继续留着 m_currentGroup 会指向一个看不见的组）。
    QString previous = m_currentGroup;
    if (globalMode != m_globalModeShown) {
        m_globalModeShown = globalMode;
        previous          = globalMode ? QStringLiteral("GLOBAL") : QString();
    }

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
        m_groupCombo->setToolTip(tr("The active profile has no proxy groups"));
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

    updateGridSize();
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

    // 当前节点在列表里用圆点 + 加粗标出，工具条只放下拉框本身：
    // 组合信息进悬停提示，避免长语言把工具条撑宽
    QString summary = tr("Type %1 · %2 members")
                          .arg(type.isEmpty() ? QStringLiteral("-") : type)
                          .arg(group.value(QStringLiteral("all")).toArray().size());
    if (!now.isEmpty()) summary = tr("Current node: %1").arg(now) + QLatin1Char('\n') + summary;
    m_groupCombo->setToolTip(summary);

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
        tip << (isGroup ? tr("Proxy group · %1").arg(type)
                        : tr("Protocol · %1").arg(
                              type.isEmpty() ? tr("Unknown") : type));
        if (unsupported) {
            tip << tr("This protocol is not supported yet; connections will fail explicitly");
        }

        const QJsonArray history = member.value(QStringLiteral("history")).toArray();
        if (m_failedTests.contains(name)) {
            item->setData(kRoleDelay, -1);
            tip << tr("Last latency test timed out");
        } else if (!history.isEmpty()) {
            const int delay = history.last().toObject().value(QStringLiteral("delay")).toInt();
            // 与参照实现一致：<=0 视为超时（history 里超时通常记 0 或负数）
            item->setData(kRoleDelay, delay > 0 ? delay : -1);
            tip << (delay > 0 ? tr("Last latency %1 ms").arg(delay)
                              : tr("Last test timed out"));
        }

        if (isGroup) {
            const QString inner = member.value(QStringLiteral("now")).toString();
            if (!inner.isEmpty()) tip << tr("Currently pointing at %1").arg(inner);
        }
        tip << tr("Click the text on the right of a card to test that node");

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
    case QEvent::Resize:
        // 窗口宽度变了就重新算一排放几张卡片
        updateGridSize();
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

    const CellGeometry g =
        layoutCell(m_memberList->visualItemRect(item), m_memberList->font(),
                   item->data(kRoleProtocol).toString());

    // 热区比文字略大，只点那几个字太考验准头
    return g.delay.adjusted(-6, -6, 6, 6).contains(pos) ? name : QString();
}

void ProxiesPage::updateGridSize()
{
    const int viewportWidth = m_memberList->viewport()->width();
    if (viewportWidth <= 0) return;

    // 列数由可用宽度决定，余量用整除摊掉，右侧才不会留一条空白。
    // 右端要扣掉滚动条的宽度：它盖在视口里，按整幅宽度算会多排一列，
    // 结果最后一列永远放不下，整行白白少一张卡片
    const int usable    = qMax(kMinCellWidth, viewportWidth - 14);
    const int cols      = qMax(1, usable / kMinCellWidth);
    const int cellWidth = usable / cols;

    if (m_memberList->gridSize().width() == cellWidth) return;
    m_memberList->setGridSize(QSize(cellWidth, kCellHeight));
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
        finishTest(name, -1, tr("Node no longer exists"));
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
                tr("Latency test finished: %1/%2 nodes available").arg(ok).arg(total));
        }
    } else if (!error.isEmpty()) {
        m_context->showToast(QStringLiteral("warning"),
                             tr("%1 latency test failed: %2").arg(name, error));
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
                             tr("These nodes are being tested, or none can be tested"));
        return;
    }

    m_batchNames = QSet<QString>(accepted.begin(), accepted.end());
    m_batchTotal = accepted.size();
    m_batchOk    = 0;

    for (const QString &name : accepted) enqueueTest(name);
    startNextTests();

    m_context->showToast(
        QStringLiteral("info"),
        tr("Testing %1 nodes (%2 at a time, results fill in as they finish)")
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

void ProxiesPage::activateItem(QListWidgetItem *item)
{
    if (!item || m_currentGroup.isEmpty()) return;

    const QString memberName = item->data(kRoleName).toString();
    if (memberName.isEmpty()) return;

    // 点的就是当前节点：不用再走一遍切换，省得刷两条提示
    if (item->data(kRoleCurrent).toBool()) return;

    QString error;
    if (!m_context->hub()->selectProxy(m_currentGroup, memberName, &error)) {
        m_context->showToast(QStringLiteral("error"),
                              tr("Failed to switch node: %1").arg(error));
        return;
    }

    // 记住这次选择：配置重载/下次启动都要以它为准
    m_context->saveProxySelections();

    m_context->showToast(QStringLiteral("info"),
                          tr("%1 switched to %2").arg(m_currentGroup, memberName));
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
