#pragma once

// ---------------------------------------------------------------------------
//  规则列表的模型与行绘制
//
//  一条规则画成一张卡片：标题是规则的值，下面是规则类型与出站策略两枚
//  胶囊，右侧是本次运行的命中次数与占比，最右侧一枚开关控制该规则是否
//  参与匹配。停用的规则整卡淡出。
// ---------------------------------------------------------------------------

#include <QAbstractListModel>
#include <QString>
#include <QStyledItemDelegate>
#include <QVector>

#include <functional>

namespace clashqt {

namespace ui {
struct Theme;
}

struct RuleRow {
    QString ruleText;  // 规则描述文本，启停接口按它定位
    QString title;     // 卡片标题：规则值，取不到时用类型
    QString type;      // 类型显示名，如 DomainSuffix
    QString proxy;     // 出站策略名
    QString tooltip;
    qint64  hits = 0;  // 本次运行命中次数
    double  share = 0; // 占全部命中次数的比例
    bool    disabled = false;
};

class RuleListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit RuleListModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setRows(QVector<RuleRow> rows);
    const RuleRow &rowAt(int row) const;

private:
    QVector<RuleRow> m_rows;
};

class RuleRowDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit RuleRowDelegate(QObject *parent = nullptr);

    void  paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    bool  editorEvent(QEvent *event, QAbstractItemModel *model,
                      const QStyleOptionViewItem &option, const QModelIndex &index) override;

    // 拨了右侧开关：把行号交回页面去切换启用状态
    std::function<void(int row)> toggled;

private:
    static QRect switchRect(const QRect &itemRect);
    static void  paintSwitch(QPainter *painter, const QRectF &track, bool on,
                             const ui::Theme &colors);
};

} // namespace clashqt
