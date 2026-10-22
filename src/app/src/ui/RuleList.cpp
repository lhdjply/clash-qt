#include "ui/RuleList.h"

#include "ui/UiFactory.h"

#include <QFontMetricsF>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace clashqt {
namespace {

constexpr int kRowHeight    = 72; // 行高，含卡片上下的 3px 间隙
constexpr int kCardInsetY   = 3;
constexpr int kPadLeft      = 14;
constexpr int kPadRight     = 14;
constexpr int kChipHeight   = 21;
constexpr int kSwitchWidth  = 40;
constexpr int kSwitchHeight = 22;

QFont withPixelSize(const QFont &base, int pixels, QFont::Weight weight = QFont::Normal)
{
    QFont font = base;
    font.setPixelSize(pixels);
    font.setWeight(weight);
    return font;
}

// 胶囊宽度：左右各 8px 内边距
qreal chipWidth(const QFontMetricsF &metrics, const QString &text)
{
    return metrics.horizontalAdvance(text) + 2 * 8;
}

} // namespace

RuleListModel::RuleListModel(QObject *parent) : QAbstractListModel(parent) {}

int RuleListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_rows.size());
}

QVariant RuleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) return QVariant();

    const RuleRow &row = m_rows.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return row.title;
    case Qt::ToolTipRole:
        return row.tooltip;
    default:
        return QVariant();
    }
}

void RuleListModel::setRows(QVector<RuleRow> rows)
{
    // 规则列表只在配置变化或启停操作后变化，每次整体替换即可；
    // 行数一致时补发 dataChanged，避免无谓地重建视图
    if (rows.size() == m_rows.size()) {
        m_rows = std::move(rows);
        if (!m_rows.isEmpty()) emit dataChanged(index(0), index(int(m_rows.size()) - 1));
        return;
    }

    beginResetModel();
    m_rows = std::move(rows);
    endResetModel();
}

const RuleRow &RuleListModel::rowAt(int row) const
{
    static const RuleRow kEmpty;
    if (row < 0 || row >= m_rows.size()) return kEmpty;
    return m_rows.at(row);
}

RuleRowDelegate::RuleRowDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QSize RuleRowDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(0, kRowHeight);
}

QRect RuleRowDelegate::switchRect(const QRect &itemRect)
{
    const QRectF card = QRectF(itemRect).adjusted(0, kCardInsetY, 0, -kCardInsetY);
    return QRect(int(card.right() - kPadRight - kSwitchWidth),
                 int(card.center().y() - kSwitchHeight / 2.0), kSwitchWidth, kSwitchHeight);
}

// 轨道 + 圆点的开关，开启时填强调色
void RuleRowDelegate::paintSwitch(QPainter *painter, const QRectF &track, bool on,
                                  const ui::Theme &colors)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(on ? colors.accent : colors.field);
    painter->drawRoundedRect(track, track.height() / 2.0, track.height() / 2.0);

    if (!on) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(colors.separator, 1));
        painter->drawRoundedRect(track.adjusted(0.5, 0.5, -0.5, -0.5),
                                 track.height() / 2.0, track.height() / 2.0);
    }

    const qreal knobSize = track.height() - 4;
    const qreal knobX = on ? track.right() - 2 - knobSize : track.left() + 2;
    painter->setPen(Qt::NoPen);
    painter->setBrush(on ? colors.onAccent : colors.textMuted);
    painter->drawEllipse(QRectF(knobX, track.top() + 2, knobSize, knobSize));
}

void RuleRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    const auto *model = qobject_cast<const RuleListModel *>(index.model());
    if (!model) return;

    const RuleRow    &row = model->rowAt(index.row());
    const ui::Theme  &colors = ui::theme();
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

    // 停用的规则整卡淡出，一眼能和生效中的区分开
    if (row.disabled) painter->setOpacity(0.5);

    const QFont metaFont  = withPixelSize(option.font, 11);
    const QFont titleFont = withPixelSize(option.font, 13, QFont::DemiBold);

    // 最右侧是启停开关，命中统计排在它左侧
    const QRectF toggle(switchRect(option.rect));

    const QFontMetricsF metaMetrics(metaFont);
    const QString       hitText = tr("%1 hits").arg(row.hits);
    const QString       percent =
        QStringLiteral("%1%").arg(QString::number(row.share * 100.0, 'f', 1));

    const qreal hitWidth     = metaMetrics.horizontalAdvance(hitText);
    const qreal percentWidth = metaMetrics.horizontalAdvance(percent) + 16;
    const qreal panelWidth   = qMax(hitWidth, percentWidth);

    const qreal panelRight = toggle.left() - 16;
    const qreal panelLeft  = panelRight - panelWidth;
    const qreal panelTop   = card.center().y() - 21;

    painter->setFont(metaFont);
    painter->setPen(colors.textMuted);
    painter->drawText(QRectF(panelLeft, panelTop, panelWidth, 18),
                      Qt::AlignRight | Qt::AlignVCenter, hitText);

    const QRectF badge(panelRight - percentWidth, panelTop + 22, percentWidth, kChipHeight);
    painter->setPen(Qt::NoPen);
    painter->setBrush(colors.accentSoft);
    painter->drawRoundedRect(badge, kChipHeight / 2.0, kChipHeight / 2.0);
    painter->setPen(colors.accent);
    painter->drawText(badge, Qt::AlignCenter, percent);

    // 左侧：标题
    const qreal textLeft   = card.left() + kPadLeft;
    const qreal titleRight = panelLeft - 12;
    const qreal titleWidth = qMax<qreal>(16, titleRight - textLeft);
    const QFontMetricsF titleMetrics(titleFont);

    painter->setFont(titleFont);
    painter->setPen(colors.text);
    painter->drawText(QRectF(textLeft, card.top() + 8, titleWidth, 20),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      titleMetrics.elidedText(row.title, Qt::ElideRight, titleWidth));

    // 第二行：规则类型与出站策略
    const QFont chipFont = withPixelSize(option.font, 11);
    const QFontMetricsF chipMetrics(chipFont);
    const qreal chipY = card.bottom() - 8 - kChipHeight;

    qreal x = textLeft;
    const qreal chipRight = titleRight;

    auto drawChip = [&](const QString &text, qreal budget) {
        if (text.isEmpty() || chipRight - x < 24) return;

        qreal width = chipWidth(chipMetrics, text);
        QString label = text;
        if (width > budget) {
            width = budget;
            label = chipMetrics.elidedText(text, Qt::ElideRight, qMax<qreal>(8, width - 16));
        }

        const QRectF chip(x, chipY, width, kChipHeight);
        painter->setPen(QPen(colors.separator, 1));
        painter->setBrush(colors.field);
        painter->drawRoundedRect(chip.adjusted(0.5, 0.5, -0.5, -0.5),
                                 kChipHeight / 2.0, kChipHeight / 2.0);

        painter->setFont(chipFont);
        painter->setPen(colors.text);
        painter->drawText(chip.adjusted(8, 0, -8, 0), Qt::AlignLeft | Qt::AlignVCenter, label);

        x += width + 6;
    };

    const qreal typeWidth  = chipWidth(chipMetrics, row.type);
    const qreal proxyBudget = chipRight - x - typeWidth - 6 - 6;
    drawChip(row.type, typeWidth);
    drawChip(row.proxy, proxyBudget);

    // 开关不跟着整卡淡出：停用的规则也要能一眼看到并拨回来
    painter->setOpacity(1.0);
    paintSwitch(painter, toggle, !row.disabled, colors);

    painter->restore();
}

bool RuleRowDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton
            && switchRect(option.rect).contains(mouse->pos())) {
            if (toggled) toggled(index.row());
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

} // namespace clashqt
