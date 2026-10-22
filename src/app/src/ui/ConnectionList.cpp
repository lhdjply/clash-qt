#include "ui/ConnectionList.h"

#include "ui/Icons.h"
#include "ui/UiFactory.h"

#include <QFontMetricsF>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

#include <utility>

namespace clashqt {
namespace {

constexpr int kRowHeight  = 64; // 行高，含卡片上下的 3px 间隙
constexpr int kCardInsetY = 3;
constexpr int kPadLeft    = 14;
constexpr int kPadRight   = 12;
constexpr int kIconBox    = 38;
constexpr int kCloseSize  = 22;
constexpr int kChipHeight = 20;

QFont withPixelSize(const QFont &base, int pixels, QFont::Weight weight = QFont::Normal)
{
    QFont font = base;
    font.setPixelSize(pixels);
    font.setWeight(weight);
    return font;
}

// 胶囊标签的宽度：左右各 8px 内边距，带圆点时再加圆点与间隔
qreal chipWidth(const QFontMetricsF &metrics, const QString &text, bool withDot)
{
    qreal width = metrics.horizontalAdvance(text) + 2 * 8;
    if (withDot) width += 2 * 3 + 5;
    return width;
}

} // namespace

// ---------------------------------------------------------------------------
//  模型
// ---------------------------------------------------------------------------

ConnectionListModel::ConnectionListModel(QObject *parent) : QAbstractListModel(parent) {}

int ConnectionListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_rows.size());
}

QVariant ConnectionListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) return QVariant();

    const ConnectionRow &row = m_rows.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return row.host;
    case Qt::ToolTipRole: {
        const QString target = row.port.isEmpty()
                                   ? row.host
                                   : QStringLiteral("%1:%2").arg(row.host, row.port);
        QStringList lines{target};
        if (!row.rule.isEmpty()) lines << tr("Rule: %1").arg(row.rule);
        if (!row.chain.isEmpty()) lines << tr("Outbound: %1").arg(row.chain);
        lines << tr("Up %1 · Down %2 · %3").arg(row.upload, row.download, row.duration);
        return lines.join(QLatin1Char('\n'));
    }
    default:
        return QVariant();
    }
}

void ConnectionListModel::setRows(QVector<ConnectionRow> rows)
{
    // 行数与顺序都没变时只发 dataChanged：每秒一次 reset 会让选中丢失、
    // 滚动条回弹，看着一直在闪
    bool sameShape = rows.size() == m_rows.size();
    if (sameShape) {
        for (int i = 0; i < rows.size(); ++i) {
            if (rows.at(i).id != m_rows.at(i).id) {
                sameShape = false;
                break;
            }
        }
    }

    if (!sameShape) {
        beginResetModel();
        m_rows = std::move(rows);
        endResetModel();
        return;
    }

    m_rows = std::move(rows);
    if (!m_rows.isEmpty()) emit dataChanged(index(0), index(int(m_rows.size()) - 1));
}

const ConnectionRow &ConnectionListModel::rowAt(int row) const
{
    static const ConnectionRow kEmpty;
    if (row < 0 || row >= m_rows.size()) return kEmpty;
    return m_rows.at(row);
}

QString ConnectionListModel::idAt(int row) const
{
    if (row < 0 || row >= m_rows.size()) return QString();
    return m_rows.at(row).id;
}

int ConnectionListModel::indexOfId(const QString &id) const
{
    if (id.isEmpty()) return -1;
    for (int i = 0; i < m_rows.size(); ++i) {
        if (m_rows.at(i).id == id) return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
//  行绘制
// ---------------------------------------------------------------------------

ConnectionRowDelegate::ConnectionRowDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QSize ConnectionRowDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(0, kRowHeight);
}

QRect ConnectionRowDelegate::closeButtonRect(const QRect &itemRect)
{
    const QRectF card = QRectF(itemRect).adjusted(0, kCardInsetY, 0, -kCardInsetY);
    return QRect(int(card.right() - kPadRight - kCloseSize),
                 int(card.center().y() - kCloseSize / 2.0), kCloseSize, kCloseSize);
}

void ConnectionRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    const auto *model = qobject_cast<const ConnectionListModel *>(index.model());
    if (!model) return;

    const ConnectionRow &row = model->rowAt(index.row());
    const ui::Theme &colors = ui::theme();
    const bool selected = (option.state & QStyle::State_Selected) != 0;
    const bool hovered  = (option.state & QStyle::State_MouseOver) != 0;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setClipRect(option.rect);

    const QRectF card = QRectF(option.rect).adjusted(0, kCardInsetY, 0, -kCardInsetY);

    QPainterPath shape;
    shape.addRoundedRect(card.adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);

    painter->setPen(Qt::NoPen);
    painter->fillPath(shape, colors.card);
    if (selected)     painter->fillPath(shape, colors.accentSoft);
    else if (hovered) painter->fillPath(shape, colors.hover);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(selected ? colors.accent : colors.separator, 1));
    painter->drawPath(shape);

    // 左侧：应用图标底板。取不到真实图标，统一画成一个方块
    const QRectF box(card.left() + kPadLeft, card.center().y() - kIconBox / 2.0,
                     kIconBox, kIconBox);
    painter->setPen(Qt::NoPen);
    painter->setBrush(colors.altRow);
    painter->drawRoundedRect(box, 9, 9);

    const QPixmap badge =
        ui::icons::pixmap(QStringLiteral("connections"), colors.accent, 20);
    painter->drawPixmap(QRectF(box.center().x() - 10, box.center().y() - 10, 20, 20),
                        badge, QRectF(badge.rect()));

    const QRectF closeRect(closeButtonRect(option.rect));
    const qreal textLeft  = box.right() + 12;
    const qreal textRight = closeRect.left() - 8;

    const QFont metaFont  = withPixelSize(option.font, 11);
    const QFont titleFont = withPixelSize(option.font, 13, QFont::DemiBold);

    const QFontMetricsF metaMetrics(metaFont);
    const qreal whenWidth = row.duration.isEmpty() ? 0 : metaMetrics.horizontalAdvance(row.duration);

    if (!row.duration.isEmpty()) {
        painter->setFont(metaFont);
        painter->setPen(colors.textMuted);
        painter->drawText(QRectF(textRight - whenWidth, card.top() + 6, whenWidth, 20),
                          Qt::AlignRight | Qt::AlignVCenter, row.duration);
    }

    // 标题：「进程 → 目标」。主机在末尾，从中间省略才不会只剩一截域名
    const QString titleText = row.process.isEmpty()
                                  ? row.host
                                  : QStringLiteral("%1 → %2").arg(row.process, row.host);
    const qreal titleRight = row.duration.isEmpty() ? textRight : textRight - whenWidth - 10;
    const QFontMetricsF titleMetrics(titleFont);
    const qreal titleWidth = qMax<qreal>(16, titleRight - textLeft);

    painter->setFont(titleFont);
    painter->setPen(colors.text);
    painter->drawText(QRectF(textLeft, card.top() + 6, titleWidth, 20),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      titleMetrics.elidedText(titleText, Qt::ElideMiddle, titleWidth));

    // 第二行：协议、出口节点、累计流量
    const QString proto = row.type.isEmpty()
                              ? row.network
                              : (row.network.isEmpty()
                                     ? row.type
                                     : QStringLiteral("%1(%2)").arg(row.type, row.network));
    const QString node = row.node.isEmpty() ? QStringLiteral("DIRECT") : row.node;
    const QString flow = QStringLiteral("↑ %1  ↓ %2").arg(row.upload, row.download);

    const QFont chipTextFont = withPixelSize(option.font, 11);
    const QFontMetricsF chipMetrics(chipTextFont);
    const qreal chipY = card.bottom() - 9 - kChipHeight;
    qreal x = textLeft;

    auto drawChip = [&](const QString &text, const QColor &dot, const QColor &textColor,
                        qreal maxWidth) {
        qreal width = chipWidth(chipMetrics, text, dot.isValid());
        QString label = text;
        if (width > maxWidth) {
            width = maxWidth;
            const qreal textWidth = width - 16 - (dot.isValid() ? 11 : 0);
            label = chipMetrics.elidedText(text, Qt::ElideRight, qMax<qreal>(8, textWidth));
        }

        const QRectF chip(x, chipY, width, kChipHeight);
        painter->setPen(QPen(colors.separator, 1));
        painter->setBrush(colors.field);
        painter->drawRoundedRect(chip.adjusted(0.5, 0.5, -0.5, -0.5),
                                 kChipHeight / 2.0, kChipHeight / 2.0);

        qreal textX = chip.left() + 8;
        if (dot.isValid()) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(dot);
            painter->drawEllipse(QPointF(textX + 3, chip.center().y()), 3, 3);
            textX += 11;
        }

        painter->setFont(chipTextFont);
        painter->setPen(textColor);
        painter->drawText(QRectF(textX, chip.top(), chip.right() - textX - 8, chip.height()),
                          Qt::AlignLeft | Qt::AlignVCenter, label);

        x += width + 6;
    };

    const qreal protoWidth = chipWidth(chipMetrics, proto, true);
    const qreal flowWidth  = chipWidth(chipMetrics, flow, false);
    const qreal nodeBudget = textRight - textLeft - protoWidth - flowWidth - 12;

    if (nodeBudget >= 60) {
        drawChip(proto, colors.accent, colors.text, protoWidth);
        drawChip(node, row.node.isEmpty() ? QColor() : colors.tintPurple, colors.text, nodeBudget);
        drawChip(flow, QColor(), colors.textMuted, flowWidth);
    } else {
        drawChip(proto, colors.accent, colors.text, protoWidth);
        drawChip(flow, QColor(), colors.textMuted, flowWidth);
    }

    // 右侧关闭按钮：悬停或用键盘选中时才显形
    if (hovered || selected) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(colors.hover);
        painter->drawEllipse(closeRect);
    }
    const QColor crossColor = (hovered || selected) ? colors.danger : colors.textMuted;
    const QPointF center = closeRect.center();
    const qreal arm = 5;
    painter->setPen(QPen(crossColor, 1.6, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(center + QPointF(-arm, -arm), center + QPointF(arm, arm));
    painter->drawLine(center + QPointF(arm, -arm), center + QPointF(-arm, arm));

    painter->restore();
}

bool ConnectionRowDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton
            && closeButtonRect(option.rect).contains(mouse->pos())) {
            const auto *rows = qobject_cast<const ConnectionListModel *>(model);
            if (rows && closeRequested) closeRequested(rows->idAt(index.row()));
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

} // namespace clashqt
