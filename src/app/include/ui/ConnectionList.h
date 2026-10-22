#pragma once

// ---------------------------------------------------------------------------
//  连接列表的模型与行绘制
//
//  连接页每秒刷新一次，逐格改表格文本既闪又费。这里把一条连接画成一张
//  卡片：左侧应用图标，中间「进程 → 目标」与几枚胶囊标签，右侧存活时长与
//  关闭按钮。刷新时行数与顺序没变就只发 dataChanged，选中与滚动都不丢。
// ---------------------------------------------------------------------------

#include <QAbstractListModel>
#include <QString>
#include <QStyledItemDelegate>
#include <QVector>

#include <functional>

class QPainter;
class QRect;

namespace clashqt {

struct ConnectionRow {
    QString id;
    QString process;   // 进程名；取不到时用来源地址顶上
    QString host;
    QString port;
    QString type;      // HTTP / HTTPS / SOCKS5
    QString network;   // TCP / UDP
    QString node;      // 出口节点
    QString chain;     // 完整出站链，进悬浮提示
    QString rule;      // 命中规则，进悬浮提示
    QString upload;
    QString download;
    QString duration;  // 已建立多久
    QString startedAt; // 排序用的原始时间戳
};

class ConnectionListModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit ConnectionListModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setRows(QVector<ConnectionRow> rows);
    const ConnectionRow &rowAt(int row) const;
    QString idAt(int row) const;
    int indexOfId(const QString &id) const;

private:
    QVector<ConnectionRow> m_rows;
};

class ConnectionRowDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit ConnectionRowDelegate(QObject *parent = nullptr);

    void  paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    bool  editorEvent(QEvent *event, QAbstractItemModel *model,
                      const QStyleOptionViewItem &option, const QModelIndex &index) override;

    // 点了右侧的关闭按钮：交回页面去关掉这条连接
    std::function<void(const QString &id)> closeRequested;

private:
    static QRect closeButtonRect(const QRect &itemRect);
};

} // namespace clashqt
