#pragma once

// ---------------------------------------------------------------------------
//  界面构件工厂与主题
//
//  把重复的「卡片 / 分组标题 / 表单行 / 按钮」构造与全部配色收敛到一处，
//  使各页面只描述内容而不重复样式代码。
//
//  视觉基调（glassmorphism）：
//   · 主窗口自绘一层柔和渐变 + 角落光晕（见 MainWindow::paintEvent）；
//   · 侧栏与卡片是**半透明**面板，叠在渐变之上形成「磨砂玻璃」的透底观感；
//   · 卡片圆角 + 细描边 + 投影，标题带线性图标；
//   · 强调色统一（浅 / 深两套），主按钮实心强调色、危险按钮描边式。
//
//  说明：这里的「毛玻璃」是应用内的半透明合成（不依赖桌面合成器），因此
//  任何桌面环境下外观一致；真正的「窗口背后虚化」需要合成器配合，不在本
//  实现范围内。
//
//  例外说明：控件仍由 Fusion 风格绘制（输入框 / 下拉框 / 勾选框 / 数字框
//  一律不覆盖），样式表只负责 Fusion 表达不了的部分（卡片、按钮、导航、
//  表格、滚动条、提示）。滚动条刻意偏离 Fusion 的带箭头样式，改为细条。
// ---------------------------------------------------------------------------

#include <QColor>
#include <QGroupBox>
#include <QString>

class QLabel;
class QPushButton;
class QWidget;

namespace clashqt {
namespace ui {

// 一套主题的全部颜色。浅色 / 深色各一份，其余代码只引用本结构，
// 不再散落颜色字面量。
struct Theme {
    bool dark = false;

    // 主窗口背景（自绘渐变）与角落光晕
    QColor bgTop;
    QColor bgBottom;
    QColor glow;

    QColor sider;      // 侧栏底色（半透明）
    QColor card;       // 卡片底色（半透明）
    QColor cardBorder;
    QColor separator;

    QColor text;
    QColor textMuted;

    QColor accent;     // 主强调色
    QColor accentHover;
    QColor accentSoft; // 强调色的浅底（选中 / 悬停）
    QColor danger;
    QColor dangerHover;
    QColor onAccent;   // 强调底色上的文字 / 图标色

    QColor hover;      // 通用悬停底色
    QColor field;      // 按钮与输入类底色
    QColor scrollHandle;
    QColor scrollHandleHover;
    QColor tooltipBg;
    QColor shadow;

    // 交给 Fusion 绘制的原生控件（下拉框 / 勾选框 / 输入框 / 数字框）所用颜色。
    // 这些控件不在样式表里覆盖，只能靠调色板着色，因此必须显式给出，
    // 不能从桌面主题继承（见 paletteFor 的说明）。
    QColor base;           // 输入类控件底色
    QColor button;         // 按钮类控件底色
    QColor altRow;         // 列表斑马纹底色（半透明）
    QColor textDisabled;   // 禁用态文字
};

// 当前生效的主题
const Theme &theme();
QColor       accent();
QColor       textColor();
QColor       mutedColor();

// 统一风格的卡片容器。title 为空时不生成表头（用于纯表格卡片）。
// 表头图标按标题自动匹配（见 Icons::forCardTitle）。
QGroupBox *makeCard(const QString &title, QWidget *parent = nullptr);

// 一行「标签 + 控件」
QWidget *makeFormRow(const QString &label, QWidget *field, const QString &hint = QString(),
                     QWidget *parent = nullptr);

// 页面标题区（图标徽标 + 大标题 + 说明）。图标按标题自动匹配。
QWidget *makePageHeader(const QString &title, const QString &description,
                        QWidget *parent = nullptr);

// 主操作按钮（强调色实心）与危险按钮（描边式）
QPushButton *makePrimaryButton(const QString &text, QWidget *parent = nullptr);
QPushButton *makeDangerButton(const QString &text, QWidget *parent = nullptr);

// 给按钮补图标；颜色按按钮角色自动选择（主 / 危险按钮用强调底色上的色，
// 其余用正文色），因此调用方只需给出图标名
void decorate(QPushButton *button, const QString &iconName);

// 提示标签（次要文字）
QLabel *makeHintLabel(const QString &text, QWidget *parent = nullptr);

// 让控件忽略鼠标滚轮事件。QAbstractSpinBox / QComboBox 默认「滚轮即改值」，
// 在可滚动页面上鼠标划过就会误改端口、参数；忽略后事件会冒泡给页面滚动区，
// 于是「划过不改值，滚轮照常滚页面」。
void ignoreWheel(QWidget *widget);

// 应用全局主题。dark 为 true 时使用深色配色。
void applyTheme(bool dark);

// 桌面当前是否处于深色模式。取 Qt 报告的配色方案（Qt 6.5+），
// 不使用 QApplication::palette()：那可能已经是本程序自己设过的调色板。
bool systemPrefersDark();

// 主题名（system / light / dark）→ 是否深色。未知取值按「跟随系统」处理。
bool themeIsDark(const QString &themeName);

// 按主题名应用主题，语义与设置页的「跟随系统 / 浅色 / 深色」一致
void applyThemeFor(const QString &themeName);

} // namespace ui
} // namespace clashqt
