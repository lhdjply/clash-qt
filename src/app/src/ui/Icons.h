#pragma once

// ---------------------------------------------------------------------------
//  矢量图标
//
//  图标以内嵌 SVG 源码保存在本模块，用 Qt Svg 渲染成 QIcon：
//   · 不引入任何图片资源文件（无需 .qrc），DPI 变化时依然锐利；
//   · 描边颜色统一写 currentColor，渲染时按主题着色，同一图标可用于
//     不同状态（导航未选中 / 选中、主按钮 / 危险按钮）而无需多份资源；
//   · 结果按「名称 + 颜色 + 尺寸」缓存，重复取用不重复渲染。
//
//  图标集是自绘的 24×24 线性图标（stroke 1.8、圆角端点），风格统一。
// ---------------------------------------------------------------------------

#include <QColor>
#include <QIcon>
#include <Qt>
#include <QString>

class QPixmap;

namespace clashqt {
namespace ui {
namespace icons {

// 取图标：name 为 resources/icons/ 下的文件名（不含扩展名）
QIcon get(const QString &name, const QColor &color, int size = 20);

// 直接取位图（供 QLabel::setPixmap 使用，省去再经 QIcon 取回的一步）
QPixmap pixmap(const QString &name, const QColor &color, int size = 20);

// 应用图标底板的状态色（一眼看出「流量以哪种方式被接管」）：
//   Normal  —— 常规（蓝紫渐变），两种接管都没开
//   Active  —— 仅系统代理已开启：红色
//   Tun     —— 仅 TUN 已开启：绿色
//   Both    —— 系统代理与 TUN 同时开启：紫色
// 三种状态共用同一份 SVG，只替换底板渐变的三个停靠点（见 Icons.cpp）。
enum class AppIconState { Normal, Active, Tun, Both };

QPixmap appPixmap(int size = 64, AppIconState state = AppIconState::Normal);

// 应用图标（渐变圆角方块 + 白色猫头），用于窗口与系统托盘
QIcon appIcon(int size = 64, AppIconState state = AppIconState::Normal);

// 导航项上记录的图标名。标题会被翻译成不同语言，不能再用标题反查图标：
// 创建导航项时把图标名写进这个 role，重绘（换主题）时读回来即可。
inline constexpr int IconNameRole = Qt::UserRole + 1;

// 标题 → 图标映射（仅用于未翻译的固定标题）。界面文案翻译后标题随语言变化，

} // namespace icons
} // namespace ui
} // namespace clashqt
