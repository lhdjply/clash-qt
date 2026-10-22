#include "ui/TileButton.h"

#include "ui/Icons.h"
#include "ui/UiFactory.h"

#include <QEnterEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

namespace clashqt::ui {

namespace {

// 瓦片固定高度：三行导航 + 两张卡片刚好填满最小窗口高度（620）下的侧栏
constexpr int kTileHeight  = 62;
constexpr int kRadius      = 12;
constexpr int kPadding     = 11;
constexpr int kIconSize    = 16;
constexpr int kMinTop      = 6;
constexpr int kSwitchWidth = 34;
constexpr int kSwitchHeight = 20;

} // namespace

TileButton::TileButton(const QString &iconName, const QString &text, QWidget *parent)
    : QAbstractButton(parent), m_iconName(iconName)
{
    setText(text);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::TabFocus);
    // 纵向也可伸展：导航网格把剩余高度均分给三行，瓦片才不至于挤在顶上
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TileButton::setBadge(int count)
{
    if (m_badge == count) return;
    m_badge = count;
    update();
}

void TileButton::setSwitchVisible(bool visible)
{
    if (m_switch == visible) return;
    m_switch = visible;
    // 只有开关可点，需要跟着指针位置换光标
    setMouseTracking(visible);
    update();
}

void TileButton::setAccentFill(bool on)
{
    if (m_accentFill == on) return;
    m_accentFill = on;
    update();
}

void TileButton::setHint(const QString &hint)
{
    if (m_hint == hint) return;
    m_hint = hint;
    update();
}

QSize TileButton::sizeHint() const
{
    return QSize(0, kTileHeight);
}

void TileButton::enterEvent(QEnterEvent *event)
{
    m_hover = true;
    update();
    QAbstractButton::enterEvent(event);
}

void TileButton::leaveEvent(QEvent *event)
{
    m_hover = false;
    update();
    QAbstractButton::leaveEvent(event);
}

void TileButton::mouseMoveEvent(QMouseEvent *event)
{
    if (m_switch) {
        setCursor(hitButton(event->pos()) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
    QAbstractButton::mouseMoveEvent(event);
}

int TileButton::contentTop() const
{
    const int blockHeight = kIconSize + 8 + 17;
    const int top = (height() - blockHeight) / 2;
    // 矮瓦片（导航）里不能按左右内边距来卡顶部，否则文字会贴到底边
    return top < kMinTop ? kMinTop : top;
}

bool TileButton::hitButton(const QPoint &pos) const
{
    // 带开关的瓦片：只有开关本体可以点。瓦片其余区域（图标、文字）不响应，
    // 避免「想点开卡片看看」误把系统代理 / TUN 开起来
    if (!m_switch) return QAbstractButton::hitButton(pos);

    if (!isEnabled()) return false;
    const QRectF track(width() - kPadding - kSwitchWidth, contentTop(), kSwitchWidth,
                       kSwitchHeight);
    // 命中区比绘制的轨道略大一圈，免得 34×20 的开关太难点中
    return track.adjusted(-2, -4, 1, 3).contains(pos);
}

void TileButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const Theme &theme = ui::theme();
    const QRectF box   = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    QColor background = theme.card;
    if (m_accentFill) {
        background = theme.accent;
    } else if (m_hover && isEnabled()) {
        background = theme.hover;
    }

    // 浅色主题的 cardBorder 是白色，与侧栏底色几乎同色，瓦片会糊成一片；
    // 深色主题反过来，用半透明白描边才有轮廓
    const QColor border = m_accentFill ? theme.accent
                                       : (theme.dark ? theme.cardBorder : theme.separator);
    painter.setPen(QPen(border, 1));
    painter.setBrush(background);
    painter.drawRoundedRect(box, kRadius, kRadius);

    QColor foreground = m_accentFill ? theme.onAccent : theme.text;
    if (!isEnabled()) foreground = theme.textDisabled;

    // 图标 + 文字作为一个整体垂直居中
    const int blockTop = contentTop();

    // 计数徽标与开关都占右上角，二者不会同时出现（徽标只在导航瓦片上），
    // 纵向与图标行对齐
    if (m_badge >= 0) {
        paintBadge(painter, blockTop);
    } else if (m_switch) {
        paintSwitch(painter, blockTop);
    }

    painter.drawPixmap(QPoint(kPadding, blockTop), icons::pixmap(m_iconName, foreground, kIconSize));

    QFont label = font();
    label.setPixelSize(13);
    label.setWeight(QFont::DemiBold);
    painter.setFont(label);

    QRectF textRect(kPadding, blockTop + kIconSize + 8, width() - kPadding * 2, 17);
    if (!m_hint.isEmpty()) {
        const QString hint = fontMetrics().elidedText(m_hint, Qt::ElideRight, 46);
        const int hintWidth = painter.fontMetrics().horizontalAdvance(hint);
        textRect.setWidth(qMax(0.0, textRect.width() - hintWidth - 6));

        QFont hintFont = label;
        hintFont.setPixelSize(11);
        hintFont.setWeight(QFont::Normal);
        painter.setFont(hintFont);
        painter.setPen(m_accentFill ? theme.onAccent : theme.textMuted);
        painter.drawText(QRectF(textRect.right() + 6, textRect.top(), hintWidth, textRect.height()),
                         Qt::AlignLeft | Qt::AlignVCenter, hint);
        painter.setFont(label);
    }

    painter.setPen(foreground);
    const QString label0 = painter.fontMetrics().elidedText(text(), Qt::ElideRight, int(textRect.width()));
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, label0);
}

void TileButton::paintSwitch(QPainter &painter, int top)
{
    const Theme &theme = ui::theme();
    const QRectF track(width() - kPadding - kSwitchWidth, top, kSwitchWidth, kSwitchHeight);

    painter.setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter.setBrush(theme.separator);
    } else if (isChecked()) {
        painter.setBrush(m_accentFill ? theme.onAccent : theme.accent);
    } else {
        painter.setBrush(theme.scrollHandle);
    }
    painter.drawRoundedRect(track, kSwitchHeight / 2.0, kSwitchHeight / 2.0);

    // 滑块：始终用底色画，避免强调色轨道上再叠一层浅色
    const qreal knobSize = 14;
    const qreal knobX = isChecked() ? track.right() - knobSize - 3 : track.left() + 3;
    painter.setBrush(m_accentFill ? theme.accent : QColor(0xFF, 0xFF, 0xFF));
    painter.drawEllipse(QRectF(knobX, track.top() + 3, knobSize, knobSize));
}

void TileButton::paintBadge(QPainter &painter, int top)
{
    const Theme &theme = ui::theme();

    QFont badgeFont = font();
    badgeFont.setPixelSize(11);
    badgeFont.setWeight(QFont::Normal);
    painter.setFont(badgeFont);

    const QString text = QString::number(m_badge);
    const int textWidth = painter.fontMetrics().horizontalAdvance(text);
    const qreal pillWidth = qMin<qreal>(textWidth + 14, width() - kPadding * 2 - 22);
    const QRectF pill(width() - kPadding - pillWidth, top + 1, pillWidth, 17);

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_accentFill ? QColor(255, 255, 255, 46) : theme.accentSoft);
    painter.drawRoundedRect(pill, 8.5, 8.5);

    painter.setPen(m_accentFill ? theme.onAccent : theme.accent);
    painter.drawText(pill, Qt::AlignCenter, text);
}

} // namespace clashqt::ui
