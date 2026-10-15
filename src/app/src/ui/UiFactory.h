#pragma once

// ---------------------------------------------------------------------------
//  界面构件工厂
//
//  把重复的「卡片 / 分组标题 / 表单行」构造收敛到一处，
//  使各页面只描述内容而不重复样式代码。样式通过 application 级
//  stylesheet 统一控制（见 UiFactory::applyTheme）。
// ---------------------------------------------------------------------------

#include <QGroupBox>
#include <QString>

class QLabel;
class QPushButton;
class QWidget;

namespace clashqt {

namespace ui {

// 统一风格的卡片容器
QGroupBox *makeCard(const QString &title, QWidget *parent = nullptr);

// 一行「标签 + 控件」；返回创建出的行容器
QWidget *makeFormRow(const QString &label, QWidget *field, const QString &hint = QString(),
                     QWidget *parent = nullptr);

// 页面标题区（大标题 + 说明）
QWidget *makePageHeader(const QString &title, const QString &description,
                        QWidget *parent = nullptr);

// 主操作按钮（强调色）
QPushButton *makePrimaryButton(const QString &text, QWidget *parent = nullptr);
QPushButton *makeDangerButton(const QString &text, QWidget *parent = nullptr);

// 提示标签（次要文字）
QLabel *makeHintLabel(const QString &text, QWidget *parent = nullptr);

// 应用全局样式。dark 为 true 时使用深色配色。
void applyTheme(bool dark);

} // namespace ui
} // namespace clashqt
