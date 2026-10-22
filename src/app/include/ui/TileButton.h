#pragma once

// ---------------------------------------------------------------------------
//  侧栏瓦片按钮
//
//  参考图里的侧栏是「卡片网格」：每个入口是一块圆角瓦片，左上角线性图标、
//  左下角文字，右上角可以是一个计数徽标或一个药丸开关。
//
//  用自绘而不是 QCheckBox + 样式表：勾选框的画法由 Fusion 决定，样式表改不了
//  尺寸与内部结构；瓦片需要「图标 + 开关 + 文字」三段布局和圆角卡片底，
//  只能自己画。
// ---------------------------------------------------------------------------

#include <QAbstractButton>
#include <QColor>
#include <QSize>
#include <QString>

#include "ui/UiFactory.h"

class QEnterEvent;
class QPainter;

namespace clashqt {
namespace ui {

class TileButton : public QAbstractButton {
    Q_OBJECT

public:
    TileButton(const QString &iconName, const QString &text, QWidget *parent = nullptr);

    // 右上角计数徽标；count < 0 时不显示
    void setBadge(int count);
    // 右上角显示药丸开关，开关状态跟随 checked
    void setSwitchVisible(bool visible);
    // 强调色实心底（导航选中态）；开启时图标与文字取反色
    void setAccentFill(bool on);
    // 图标与徽标的识别色。存的是色相槽位而非颜色值，换深浅主题时重绘即可
    // 取到新色，不必回来重设
    void setTint(Tint tint);
    // 副标题：显示在文字右侧的次要信息（如「启用中…」），为空时不占位
    void setHint(const QString &hint);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override { return sizeHint(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    // 开关瓦片只让开关本体算作可点区域
    bool hitButton(const QPoint &pos) const override;

private:
    void paintSwitch(QPainter &painter, int top);
    void paintBadge(QPainter &painter, int top);
    // 图标 / 文字块的顶部（垂直居中，且不贴边）
    int  contentTop() const;

    QString m_iconName;
    QString m_hint;
    int     m_badge       = -1;
    bool    m_switch      = false;
    bool    m_accentFill  = false;
    bool    m_hover       = false;
    Tint    m_tint        = Tint::None;
};

} // namespace ui
} // namespace clashqt
