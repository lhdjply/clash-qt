#include "ui/UiFactory.h"
#include <QCoreApplication>

#include "ui/Icons.h"
#include "utils/AppLogger.h"

#include <QApplication>
#include <QEvent>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QListWidget>
#include <QWidget>

#include <algorithm>
#include <cstring>

namespace clashqt {
namespace ui {
namespace {

// ---------------------------------------------------------------------------
//  配色
//
//  全部颜色集中在 Theme 里，浅 / 深各一套；其余代码只引用 Theme，不再散落
//  颜色字面量，改配色只需改这两处。
// ---------------------------------------------------------------------------
Theme makeLightTheme()
{
    Theme t;

    // 背景渐变：极浅的蓝灰，给半透明卡片提供「透过玻璃」的层次
    t.bgTop        = QColor(0xF7, 0xF9, 0xFD);
    t.bgBottom     = QColor(0xE6, 0xEC, 0xF7);
    t.glow         = QColor(0x4F, 0x8C, 0xFF, 44);

    t.sider        = QColor(0xFF, 0xFF, 0xFF, 150);
    t.card         = QColor(0xFF, 0xFF, 0xFF, 198);
    t.cardBorder   = QColor(0xFF, 0xFF, 0xFF, 200);
    t.separator    = QColor(0x1F, 0x2A, 0x3C, 26);

    t.text         = QColor(0x1B, 0x20, 0x2B);
    t.textMuted    = QColor(0x6B, 0x73, 0x84);

    t.accent       = QColor(0x3B, 0x6E, 0xF5);
    t.accentHover  = QColor(0x4F, 0x8C, 0xFF);
    t.accentSoft   = QColor(0x3B, 0x6E, 0xF5, 40);
    t.danger       = QColor(0xD9, 0x30, 0x26);
    t.dangerHover  = QColor(0xE8, 0x46, 0x3C);
    t.onAccent     = QColor(0xFF, 0xFF, 0xFF);

    t.hover        = QColor(0x1F, 0x2A, 0x3C, 22);
    t.field        = QColor(0xFF, 0xFF, 0xFF, 175);
    t.scrollHandle = QColor(0x1F, 0x2A, 0x3C, 58);
    t.scrollHandleHover = QColor(0x3B, 0x6E, 0xF5, 165);
    t.tooltipBg    = QColor(0xFF, 0xFF, 0xFF, 250);
    t.shadow       = QColor(0x1F, 0x2A, 0x3C, 42);

    // 原生控件（下拉框 / 勾选框 / 输入框）与列表斑马纹
    t.base         = QColor(0xFF, 0xFF, 0xFF);
    t.button       = QColor(0xF4, 0xF6, 0xFB);
    t.altRow       = QColor(0x1F, 0x2A, 0x3C, 10);
    t.textDisabled = QColor(0x9A, 0xA2, 0xAF);
    return t;
}

Theme makeDarkTheme()
{
    Theme t;
    t.dark = true;

    t.bgTop        = QColor(0x15, 0x17, 0x1C);
    t.bgBottom     = QColor(0x20, 0x23, 0x2B);
    t.glow         = QColor(0x6E, 0x8C, 0xFF, 36);

    t.sider        = QColor(0xFF, 0xFF, 0xFF, 14);
    t.card         = QColor(0xFF, 0xFF, 0xFF, 17);
    t.cardBorder   = QColor(0xFF, 0xFF, 0xFF, 30);
    t.separator    = QColor(0xFF, 0xFF, 0xFF, 22);

    t.text         = QColor(0xE8, 0xEA, 0xEF);
    t.textMuted    = QColor(0x9A, 0xA1, 0xAE);

    t.accent       = QColor(0x5B, 0x8D, 0xFF);
    t.accentHover  = QColor(0x74, 0x9F, 0xFF);
    t.accentSoft   = QColor(0x5B, 0x8D, 0xFF, 56);
    t.danger       = QColor(0xF0, 0x6B, 0x62);
    t.dangerHover  = QColor(0xF5, 0x84, 0x7B);
    t.onAccent     = QColor(0xFF, 0xFF, 0xFF);

    t.hover        = QColor(0xFF, 0xFF, 0xFF, 20);
    t.field        = QColor(0xFF, 0xFF, 0xFF, 14);
    t.scrollHandle = QColor(0xFF, 0xFF, 0xFF, 46);
    t.scrollHandleHover = QColor(0x5B, 0x8D, 0xFF, 170);
    t.tooltipBg    = QColor(0x24, 0x27, 0x2F, 250);
    t.shadow       = QColor(0x00, 0x00, 0x00, 130);

    // 原生控件与列表斑马纹（深色下压暗，避免浅色控件突兀地亮着）
    t.base         = QColor(0x26, 0x28, 0x2C);
    t.button       = QColor(0x2C, 0x2F, 0x33);
    t.altRow       = QColor(0xFF, 0xFF, 0xFF, 12);
    t.textDisabled = QColor(0x6B, 0x70, 0x78);
    return t;
}

// 当前生效的主题。makeCard 等构件在创建时取色，因此 applyTheme 必须
// 先于窗口构造（见 main.cpp 的启动顺序）。
Theme g_theme = makeLightTheme();

QString css(const QColor &color)
{
    return QStringLiteral("rgba(%1,%2,%3,%4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(QString::number(color.alphaF(), 'f', 3));
}

QString soft(const QColor &color, double alpha)
{
    return QStringLiteral("rgba(%1,%2,%3,%4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(QString::number(alpha, 'f', 3));
}

// 统一使用 Qt 的 Fusion 风格：外观不随桌面环境变化，且输入框 / 下拉框 /
// 勾选框 / 数字框等一律交给 Fusion 绘制（样式表不去覆盖它们），
// 避免出现「自定义控件 + 原生控件」混杂的观感。
void ensureFusionStyle()
{
    if (!qApp) return;
    QStyle *current = QApplication::style();
    if (current && current->objectName() == QLatin1String("fusion")) return;
    if (QStyle *fusion = QStyleFactory::create(QStringLiteral("Fusion"))) {
        QApplication::setStyle(fusion); // 所有权移交 QApplication
    }
}

// Fusion 依据调色板绘制控件，因此主题切换必须同步调色板，否则会出现
// 「浅色控件 + 深色背景」的错配。
//
// 这里**逐项显式指定全部角色**，绝不从桌面主题继承：Qt 6.8 起 Fusion 会
// 跟随系统深浅色，若沿用 style()->standardPalette()，在深色桌面上浅色主题
// 会拿到一份深色调色板，于是出现「背景是浅色的，但下拉框变深、普通 QLabel
// 文字变白看不见、列表斑马纹变成深色横条」这种半深半浅的错配。
QPalette paletteFor(const Theme &t)
{
    const bool dark = t.dark;

    // 三四级立体边框：Fusion 用它们给未覆盖样式的控件描边 / 做立体感，
    // 深色主题里应比底色略亮，浅色主题里应比底色略暗
    const QColor light    = dark ? t.base.lighter(165) : t.base;
    const QColor midlight = dark ? t.base.lighter(132) : t.base.darker(106);
    const QColor mid      = dark ? t.base.darker(112) : t.base.darker(122);
    const QColor darkEdge = dark ? t.base.darker(145) : t.base.darker(150);
    const QColor shadow   = dark ? t.base.darker(200) : t.base.darker(220);

    QPalette p;
    p.setColor(QPalette::Window, t.bgBottom);
    p.setColor(QPalette::WindowText, t.text);
    p.setColor(QPalette::Base, t.base);
    p.setColor(QPalette::AlternateBase, t.altRow);
    p.setColor(QPalette::Text, t.text);
    p.setColor(QPalette::Button, t.button);
    p.setColor(QPalette::ButtonText, t.text);
    p.setColor(QPalette::BrightText, t.danger);
    p.setColor(QPalette::PlaceholderText, t.textMuted);
    p.setColor(QPalette::Light, light);
    p.setColor(QPalette::Midlight, midlight);
    p.setColor(QPalette::Mid, mid);
    p.setColor(QPalette::Dark, darkEdge);
    p.setColor(QPalette::Shadow, shadow);
    p.setColor(QPalette::ToolTipBase, t.tooltipBg);
    p.setColor(QPalette::ToolTipText, t.text);
    // 焦点框 / 选中项 / 链接统一跟随强调色，与卡片、按钮的配色一致
    p.setColor(QPalette::Highlight, t.accent);
    p.setColor(QPalette::HighlightedText, t.onAccent);
    p.setColor(QPalette::Link, t.accent);
    p.setColor(QPalette::LinkVisited, t.accent.darker(120));
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    p.setColor(QPalette::Accent, t.accent);
#endif

    // 禁用态只压暗文字，控件底色仍取主题底色：否则禁用的输入框会变成
    // 一块与主题无关的灰。
    p.setColor(QPalette::Disabled, QPalette::Text, t.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::WindowText, t.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, t.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, t.textDisabled);
    return p;
}

// 样式表模板。@token 在 buildStyleSheet 里替换为主题色 —— Qt 没有 CSS 变量，
// 用占位符替换可以保持模板可读、配色集中。
const char kStyleSheetTemplate[] = R"QSS(
/* ---------- 根容器：透明，让主窗口自绘的渐变透出 ---------- */
QWidget#centralRoot,
QWidget#pageHost,
QStackedWidget,
QScrollArea,
QScrollArea > QWidget > QWidget { background: transparent; }
QScrollArea { border: 0px; }

/* ---------- 侧栏 ---------- */
QWidget#sider { background: @sider; border-right: 1px solid @separator; }
QWidget#brand { background: transparent; }
QLabel#brandName { font-size: 15px; font-weight: 600; color: @text; }
QLabel#brandVersion { font-size: 11px; color: @muted; }

/* ---------- 卡片 ---------- */
QGroupBox#card {
    background: @card;
    border: 1px solid @cardBorder;
    border-radius: 14px;
    margin: 0px;
    padding: 0px;
}
QWidget#cardHeader { background: transparent; }
QLabel#cardIcon { background: transparent; }
QLabel#cardTitle { font-size: 13px; font-weight: 600; color: @text; }

/* ---------- 页面标题 ---------- */
QWidget#pageHeader { background: transparent; }
QLabel#pageBadge { background: @accentSoft; border-radius: 13px; }
QLabel#pageTitle { font-size: 21px; font-weight: 600; color: @text; }
QLabel#pageDescription { font-size: 12px; color: @muted; }

/* ---------- 文字 ---------- */
QLabel#hintLabel { font-size: 12px; color: @muted; }
QLabel#formLabel { font-size: 12px; color: @muted; }
QLabel#statValue { font-size: 16px; font-weight: 600; color: @text; }

/* ---------- 按钮 ---------- */
QPushButton {
    background: @field;
    border: 1px solid @cardBorder;
    border-radius: 9px;
    padding: 6px 14px;
    color: @text;
}
QPushButton:hover { background: @hover; border-color: @accentBorder; }
QPushButton:pressed { background: @accentSoft; }
QPushButton:disabled { color: @muted; background: transparent; border-color: @separator; }
QPushButton#primaryButton {
    background: @accent; border: 1px solid @accent; color: @onAccent; font-weight: 500;
}
QPushButton#primaryButton:hover { background: @accentHover; border-color: @accentHover; }
QPushButton#primaryButton:pressed { background: @accent; }
QPushButton#dangerButton {
    background: transparent; border: 1px solid @danger; color: @danger;
}
QPushButton#dangerButton:hover { background: @dangerSoft; border-color: @danger; }

/* 订阅页工具栏：七个按钮排一行，默认内边距在英文下顶到横向滚动条 */
QWidget#profileToolbar QPushButton { padding-left: 10px; padding-right: 10px; }

/* ---------- 侧栏：模式分段 / 订阅卡 / 连接卡 ---------- */
QWidget#segmentBar { background: @hover; border-radius: 10px; }
QPushButton#segment {
    background: transparent; border: 0px; border-radius: 8px;
    padding: 6px 0px; color: @muted; font-size: 13px;
}
QPushButton#segment:hover { background: @hover; color: @text; }
QPushButton#segment:checked { background: @accent; color: @onAccent; font-weight: 500; }

QPushButton#profileCard {
    background: @card; border: 1px solid @cardBorder; border-radius: 12px;
    padding: 0px; text-align: left;
}
QPushButton#profileCard:hover { background: @hover; }
QPushButton#profileCard[active="true"] { background: @accent; border-color: @accent; }
QPushButton#profileCard[active="true"]:hover { background: @accentHover; }
QLabel#profileName { background: transparent; color: @text; font-size: 13px; font-weight: 600; }
QLabel#profileName[active="true"] { color: #FFFFFF; }
QLabel#profileMeta { background: transparent; color: @muted; font-size: 12px; }
QLabel#profileMeta[active="true"] { color: rgba(255,255,255,0.88); }
QProgressBar#profileBar { background: @separator; border: 0px; border-radius: 2px; }
QProgressBar#profileBar[active="true"] { background: rgba(255,255,255,0.34); }
QProgressBar#profileBar::chunk { background: @accentBorder; border-radius: 2px; }
QProgressBar#profileBar[active="true"]::chunk { background: #FFFFFF; }
QToolButton#profileRefresh { background: transparent; border: 0px; border-radius: 6px; }
QToolButton#profileRefresh:hover { background: @hover; }
QToolButton#profileRefresh:pressed { background: @accentSoft; }
QToolButton#profileRefresh[active="true"]:hover { background: rgba(255,255,255,0.20); }
QToolButton#profileRefresh[active="true"]:pressed { background: rgba(255,255,255,0.32); }
QToolButton#profileRefresh:disabled { background: transparent; }

QPushButton#trafficCard {
    background: @card; border: 1px solid @cardBorder; border-radius: 12px;
    padding: 0px; text-align: left;
}
QPushButton#trafficCard:hover { background: @hover; }
QPushButton#trafficCard[active="true"] { background: @accent; border-color: @accent; }
QPushButton#trafficCard[active="true"]:hover { background: @accentHover; }
QLabel#trafficTitle { background: transparent; font-size: 13px; font-weight: 600; color: @text; }
QLabel#trafficTitle[active="true"] { color: #FFFFFF; }
QLabel#speedValue { background: transparent; font-size: 12px; color: @muted; }
QLabel#speedValue[active="true"] { color: rgba(255,255,255,0.88); }

/* ---------- 表格 / 列表 ---------- */
QTableWidget {
    background: transparent; border: 0px; gridline-color: @separator;
    alternate-background-color: @altRow;
    selection-background-color: @accentSoft; selection-color: @text;
}
QTableWidget::item { padding: 5px 8px; border: 0px; }
QTableWidget::item:hover { background: @hover; }
QHeaderView { background: transparent; }
QHeaderView::section {
    background: transparent; border: 0px;
    border-bottom: 1px solid @cardBorder;
    padding: 7px 8px; color: @muted;
}
QTableCornerButton::section { background: transparent; border: 0px; }
/* 斑马纹由样式表给色：列表开了 alternatingRowColors，若不指定就会回落到
   调色板的 AlternateBase，桌面主题一变就冒出一条深色横条 */
QListWidget {
    background: transparent; border: 0px; outline: 0px;
    alternate-background-color: @altRow;
}
QListWidget::item { padding: 7px 10px; border-radius: 8px; color: @text; }
QListWidget::item:hover { background: @hover; }
QListWidget::item:selected { background: @accentSoft; color: @text; }

/* ---------- 滚动条：刻意改为细条（Fusion 的带箭头样式偏旧）---------- */
QScrollBar:vertical { background: transparent; width: 11px; margin: 2px 3px 2px 0px; }
QScrollBar::handle:vertical { background: @scroll; border-radius: 4px; min-height: 32px; }
QScrollBar::handle:vertical:hover { background: @scrollHover; }
QScrollBar:horizontal { background: transparent; height: 11px; margin: 0px 2px 3px 2px; }
QScrollBar::handle:horizontal { background: @scroll; border-radius: 4px; min-width: 32px; }
QScrollBar::handle:horizontal:hover { background: @scrollHover; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; }
QScrollBar::up-arrow, QScrollBar::down-arrow,
QScrollBar::left-arrow, QScrollBar::right-arrow { width: 0px; height: 0px; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

/* ---------- 文本输入 ---------- */
QPlainTextEdit, QTextEdit {
    background: @field; border: 1px solid @cardBorder; border-radius: 10px;
    padding: 6px;
    selection-background-color: @accent; selection-color: @onAccent;
}

/* ---------- 状态栏与提示 ---------- */
QStatusBar { background: transparent; color: @muted; }
QStatusBar::item { border: 0px; }
QStatusBar QLabel { color: @muted; }
QToolTip {
    background: @tooltipBg; color: @text; border: 1px solid @cardBorder;
    border-radius: 8px; padding: 5px 9px;
}

/* ---------- 弹窗 ---------- */
/* 弹窗底色沿用主窗口那层渐变，避免各平台给出一块原生灰；卡片叠在上面
   就是和主界面一致的观感。QMessageBox 等同为 QDialog，一并受益。 */
QDialog { background: @windowBg; }
QLabel#dialogBadge { background: @accentSoft; border-radius: 11px; }
QLabel#dialogTitle { font-size: 16px; font-weight: 600; color: @text; }

/* ---------- 图表容器 ---------- */
QChartView { background: transparent; border: 0px; }
)QSS";

// 图标是按主题色渲染成位图的，不随样式表变化。因此把「图标名 + 颜色 + 尺寸」
// 记在部件的动态属性上，主题切换时据此重取（见 refreshIcons）。
const char kIconNameProperty[] = "clashqt_icon_name";
const char kIconColorProperty[] = "clashqt_icon_color";
const char kIconSizeProperty[] = "clashqt_icon_size";

void applyIconToLabel(QLabel *label, const QString &name, const QColor &color, int size)
{
    if (!label) return;
    label->setProperty(kIconNameProperty, name);
    label->setProperty(kIconColorProperty, color);
    label->setProperty(kIconSizeProperty, size);
    label->setPixmap(icons::pixmap(name, color, size));
}

void applyIconToButton(QPushButton *button, const QString &name, const QColor &color)
{
    if (!button) return;
    button->setProperty(kIconNameProperty, name);
    button->setProperty(kIconColorProperty, color);
    button->setProperty(kIconSizeProperty, 16);
    button->setIcon(icons::get(name, color, 16));
}

// 主题切换后刷新所有已创建图标；导航项不是部件，按「文本即标题」单独处理
void refreshIcons()
{
    if (!qApp) return;
    const Theme &t = g_theme;

    for (QWidget *widget : qApp->allWidgets()) {
        const QString name = widget->property(kIconNameProperty).toString();
        if (!name.isEmpty()) {
            const QColor color = widget->property(kIconColorProperty).value<QColor>();
            const int    size  = widget->property(kIconSizeProperty).toInt();
            const int    px    = size > 0 ? size : 16;
            if (auto *button = qobject_cast<QPushButton *>(widget)) {
                button->setIcon(icons::get(name, color, px));
            } else if (auto *label = qobject_cast<QLabel *>(widget)) {
                label->setPixmap(icons::pixmap(name, color, px));
            }
            continue;
        }

        // 导航列表：选中项是实心强调色底（图标取白），其余取正文色
        auto *list = qobject_cast<QListWidget *>(widget);
        if (!list || list->objectName() != QLatin1String("navList")) continue;
        for (int i = 0; i < list->count(); ++i) {
            QListWidgetItem *item = list->item(i);
            if (!item) continue;
            const QColor color = (i == list->currentRow()) ? t.onAccent : t.text;
            // 图标名存在 data role 里：item 文本会随语言变化，不能拿它反查
            const QString name = item->data(icons::IconNameRole).toString();
            if (!name.isEmpty()) item->setIcon(icons::get(name, color, 18));
        }
    }
}

// 滚轮过滤器：只拦滚轮，其它事件原样放行。
//
// 关键在「不接受」这个状态：QWidget 不接受滚轮事件时，Qt 会把它沿父链继续
// 往上传（见 QWidgetWindow 的滚轮分发），于是滚轮最终落到页面滚动区上 ——
// 数值不会被改动，页面仍能正常滚动。直接吞掉（accept）反而会让滚轮失效。
class WheelGuard : public QObject {
public:
    explicit WheelGuard(QObject *parent) : QObject(parent) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::Wheel) return QObject::eventFilter(watched, event);

        event->ignore();
        return true; // 不再进入部件自身的滚轮处理
    }
};

QString buildStyleSheet(const Theme &t)
{
    struct Substitution {
        const char *token;
        QString     value;
    };

    const Substitution subs[] = {
        {"@sider", css(t.sider)},
        {"@card", css(t.card)},
        {"@cardBorder", css(t.cardBorder)},
        {"@separator", css(t.separator)},
        {"@text", t.text.name()},
        {"@muted", t.textMuted.name()},
        {"@accent", t.accent.name()},
        {"@accentHover", t.accentHover.name()},
        {"@accentSoft", css(t.accentSoft)},
        {"@accentBorder", soft(t.accent, 0.45)},
        {"@danger", t.danger.name()},
        {"@dangerSoft", soft(t.danger, 0.16)},
        {"@onAccent", t.onAccent.name()},
        {"@hover", css(t.hover)},
        {"@field", css(t.field)},
        {"@altRow", css(t.altRow)},
        {"@scroll", css(t.scrollHandle)},
        {"@scrollHover", css(t.scrollHandleHover)},
        {"@tooltipBg", css(t.tooltipBg)},
        // 弹窗底色：与主窗口同向的渐变，用 qlineargradient 直接写在样式里
        {"@windowBg",
         QStringLiteral("qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2)")
             .arg(t.bgTop.name(), t.bgBottom.name())},
    };

    QString sheet = QString::fromUtf8(kStyleSheetTemplate);
    // 必须按 token 长度降序替换：@card 是 @cardBorder 的前缀，
    // 若先替换短 token 会把长 token 破坏成「颜色 + Border」
    QList<Substitution> ordered(subs, subs + std::size(subs));
    std::sort(ordered.begin(), ordered.end(),
              [](const Substitution &a, const Substitution &b) {
                  return std::strlen(a.token) > std::strlen(b.token);
              });
    for (const Substitution &sub : ordered) {
        sheet.replace(QLatin1String(sub.token), sub.value);
    }
    return sheet;
}

} // namespace

const Theme &theme() { return g_theme; }
QColor       accent() { return g_theme.accent; }
QColor       textColor() { return g_theme.text; }
QColor       mutedColor() { return g_theme.textMuted; }

QGroupBox *makeCard(const QString &title, QWidget *parent, const QString &iconName)
{
    auto *card = new QGroupBox(parent);
    card->setObjectName(QStringLiteral("card"));
    // QGroupBox 自带标题只能放纯文本，无法带图标，因此标题留空，
    // 由下面自绘的表头承担（表头是布局里的第一个元素）
    card->setTitle(QString());

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 16);
    layout->setSpacing(12);

    if (!title.isEmpty()) {
        auto *header = new QWidget(card);
        header->setObjectName(QStringLiteral("cardHeader"));
        auto *headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(0, 0, 0, 0);
        headerLayout->setSpacing(9);

        // 翻译后的标题不能再反查图标表，调用方显式给出图标名时优先用它
        const QString icon = iconName;
        if (!icon.isEmpty()) {
            auto *iconLabel = new QLabel(header);
            iconLabel->setObjectName(QStringLiteral("cardIcon"));
            iconLabel->setFixedSize(18, 18);
            applyIconToLabel(iconLabel, icon, g_theme.accent, 18);
            headerLayout->addWidget(iconLabel);
        }

        auto *titleLabel = new QLabel(title, header);
        titleLabel->setObjectName(QStringLiteral("cardTitle"));
        headerLayout->addWidget(titleLabel, 1);

        layout->addWidget(header);
    }

    // 投影让卡片从渐变背景上「浮」起来；颜色随主题（深色下用黑色，
    // 浅色下用冷灰，避免浅色背景上出现脏灰影）
    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 5);
    shadow->setColor(g_theme.shadow);
    card->setGraphicsEffect(shadow);

    return card;
}

QWidget *makeFormRow(const QString &label, QWidget *field, const QString &hint, QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QVBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    if (!label.isEmpty()) {
        auto *caption = new QLabel(label, row);
        caption->setObjectName(QStringLiteral("formLabel"));
        layout->addWidget(caption);
    }
    layout->addWidget(field);

    if (!hint.isEmpty()) {
        auto *hintLabel = new QLabel(hint, row);
        hintLabel->setObjectName(QStringLiteral("hintLabel"));
        hintLabel->setWordWrap(true);
        layout->addWidget(hintLabel);
    }
    return row;
}

QWidget *makePageHeader(const QString &title, const QString &description,
                        const QString &iconName, QWidget *parent)
{
    auto *header = new QWidget(parent);
    header->setObjectName(QStringLiteral("pageHeader"));

    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(2, 2, 2, 8);
    layout->setSpacing(13);

    // 图标徽标：强调色的柔和底 + 线性图标
    const QString icon = iconName;
    if (!icon.isEmpty()) {
        auto *badge = new QLabel(header);
        badge->setObjectName(QStringLiteral("pageBadge"));
        badge->setFixedSize(42, 42);
        badge->setAlignment(Qt::AlignCenter);
        applyIconToLabel(badge, icon, g_theme.accent, 22);
        layout->addWidget(badge, 0, Qt::AlignTop);
    }

    auto *texts = new QWidget(header);
    auto *textLayout = new QVBoxLayout(texts);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);

    auto *titleLabel = new QLabel(title, texts);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    textLayout->addWidget(titleLabel);

    if (!description.isEmpty()) {
        auto *descLabel = new QLabel(description, texts);
        descLabel->setObjectName(QStringLiteral("pageDescription"));
        descLabel->setWordWrap(true);
        textLayout->addWidget(descLabel);
    }
    layout->addWidget(texts, 1);

    return header;
}

QWidget *makeDialogHeader(const QString &iconName, const QString &title,
                          const QString &description, QWidget *parent)
{
    auto *header = new QWidget(parent);
    header->setObjectName(QStringLiteral("pageHeader"));

    auto *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    if (!iconName.isEmpty()) {
        auto *badge = new QLabel(header);
        badge->setObjectName(QStringLiteral("dialogBadge"));
        badge->setFixedSize(38, 38);
        badge->setAlignment(Qt::AlignCenter);
        applyIconToLabel(badge, iconName, g_theme.accent, 20);
        layout->addWidget(badge, 0, Qt::AlignTop);
    }

    auto *texts = new QWidget(header);
    auto *textLayout = new QVBoxLayout(texts);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);

    auto *titleLabel = new QLabel(title, texts);
    titleLabel->setObjectName(QStringLiteral("dialogTitle"));
    textLayout->addWidget(titleLabel);

    if (!description.isEmpty()) {
        auto *descLabel = new QLabel(description, texts);
        descLabel->setObjectName(QStringLiteral("pageDescription"));
        descLabel->setWordWrap(true);
        textLayout->addWidget(descLabel);
    }
    layout->addWidget(texts, 1);

    return header;
}

QPushButton *makePrimaryButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName(QStringLiteral("primaryButton"));
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QPushButton *makeDangerButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName(QStringLiteral("dangerButton"));
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

void decorate(QPushButton *button, const QString &iconName)
{
    if (!button) return;

    // 图标颜色按按钮角色选：主按钮是实心强调色底，用强调底上的文字色；
    // 危险按钮是描边式（底色始终透明），用危险色本身；其余用正文色
    const QString role = button->objectName();
    QColor        color = g_theme.text;
    if (role == QLatin1String("primaryButton")) {
        color = g_theme.onAccent;
    } else if (role == QLatin1String("dangerButton")) {
        color = g_theme.danger;
    }

    applyIconToButton(button, iconName, color);
}

QLabel *makeHintLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QStringLiteral("hintLabel"));
    label->setWordWrap(true);
    return label;
}

void ignoreWheel(QWidget *widget)
{
    if (!widget) return;

    // 幂等：重复调用不再叠加过滤器。过滤器以控件为父对象，随控件一起销毁
    static const char kGuardInstalled[] = "clashqt_wheel_guard";
    if (widget->property(kGuardInstalled).toBool()) return;

    widget->installEventFilter(new WheelGuard(widget));
    widget->setProperty(kGuardInstalled, true);
}

void applyTheme(bool dark)
{
    ensureFusionStyle();

    g_theme = dark ? makeDarkTheme() : makeLightTheme();

    if (qApp) {
        qApp->setPalette(paletteFor(g_theme));

        // Fusion 不定义字号，沿用 13px 正文观感
        QFont font = QApplication::font();
        font.setPixelSize(13);
        QApplication::setFont(font);

        qApp->setStyleSheet(buildStyleSheet(g_theme));

        // 图标是按主题色渲染的位图，需要在换色后重新取用；
        // 这样设置页里热切换深浅色也能立即生效，无需重启
        refreshIcons();

        // 卡片投影是在创建时按当时的主题色设的，也要跟着换，
        // 否则深色主题下会残留浅色主题的冷灰影
        for (QWidget *widget : qApp->allWidgets()) {
            if (auto *effect = qobject_cast<QGraphicsDropShadowEffect *>(widget->graphicsEffect())) {
                effect->setColor(g_theme.shadow);
            }
        }
    }

    // 一并记录实际生效的 style 与调色板底色：调色板若被平台主题覆盖，
    // 会出现「浅色主题 + 深色原生控件」的错配，只看主题名不易发现
    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "UI style: Fusion (%1) style=%2 field=%3 accent=%4 desktop dark=%5")
            .arg(dark ? QCoreApplication::translate("clashqt::app", "dark") : QCoreApplication::translate("clashqt::app", "light"),
                 qApp ? QString::fromLatin1(qApp->style()->metaObject()->className())
                     : QStringLiteral("-"),
                 qApp ? qApp->palette().color(QPalette::Base).name() : QStringLiteral("-"),
                 g_theme.accent.name(),
                 systemPrefersDark() ? QCoreApplication::translate("clashqt::app", "yes") : QCoreApplication::translate("clashqt::app", "no")));
}

bool themeIsDark(const QString &themeName)
{
    if (themeName == QLatin1String("dark")) return true;
    if (themeName == QLatin1String("light")) return false;
    // system 及未知取值：跟随桌面配色方案
    return systemPrefersDark();
}

// 当前桌面是否深色。用 Qt 报告的配色方案（DDE / GNOME 等平台主题会设置），
// 而不是读 QApplication::palette()：后者在运行期可能已被本程序覆盖，
// 会得出「自己刚才设的浅色」这种自指结论。
bool systemPrefersDark()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    return false;
#endif
}

void applyThemeFor(const QString &themeName)
{
    applyTheme(themeIsDark(themeName));
}

} // namespace ui
} // namespace clashqt
