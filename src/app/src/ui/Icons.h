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
#include <QString>

class QPixmap;

namespace clashqt {
namespace ui {
namespace icons {

// 取图标：name 为 resources/icons/ 下的文件名（不含扩展名）
QIcon get(const QString &name, const QColor &color, int size = 20);

// 直接取位图（供 QLabel::setPixmap 使用，省去再经 QIcon 取回的一步）
QPixmap pixmap(const QString &name, const QColor &color, int size = 20);

// 应用图标底板的状态色：
//   Normal —— 常规（蓝紫渐变）
//   Active —— 系统代理已开启，底板换成红色，托盘上一眼看出「流量正被接管」
enum class AppIconState { Normal, Active };

QPixmap appPixmap(int size = 64, AppIconState state = AppIconState::Normal);

// 应用图标（渐变圆角方块 + 白色猫头），用于窗口与系统托盘
QIcon appIcon(int size = 64, AppIconState state = AppIconState::Normal);

// 标题 → 图标映射：让既有 makeCard(标题) 调用自动获得合适的图标，
// 无需逐个改动页面代码。未收录的标题返回空图标。
QIcon forCardTitle(const QString &title, const QColor &color, int size = 18);
QIcon forNavTitle(const QString &title, const QColor &color, int size = 18);

// 标题对应的图标名（未收录返回空）。需要记录图标以便主题切换后重取时使用。
QString cardIconName(const QString &title);
QString navIconName(const QString &title);

} // namespace icons
} // namespace ui
} // namespace clashqt
