#pragma once

// ---------------------------------------------------------------------------
//  连接页
//
//  展示内核当前的活动连接：一条一张卡片，显示进程与目标、协议、出口节点、
//  累计流量和存活时长。支持单条关闭与全部关闭。
// ---------------------------------------------------------------------------

#include <QString>
#include <QWidget>

class QCheckBox;
class QLabel;
class QListView;
class QStackedWidget;
class QTimer;

namespace clashqt {

class AppContext;
class ConnectionListModel;
class ConnectionRowDelegate;

class ConnectionsPage : public QWidget {
    Q_OBJECT

public:
    explicit ConnectionsPage(AppContext *context, QWidget *parent = nullptr);

    void refresh();

private slots:
    void onAutoRefreshToggled(bool enabled);
    void onCloseSelected();
    void onCloseAll();

private:
    // 「3 分钟前」这类相对时长；时间戳解析不了时返回空
    QString elapsedText(const QString &start) const;

    AppContext            *m_context = nullptr;
    ConnectionListModel   *m_model = nullptr;
    ConnectionRowDelegate *m_delegate = nullptr;

    QListView      *m_list = nullptr;
    QStackedWidget *m_stack = nullptr;
    QLabel         *m_empty = nullptr;
    QLabel         *m_summary = nullptr;
    QCheckBox      *m_autoRefresh = nullptr;
    QTimer         *m_refreshTimer = nullptr;
};

} // namespace clashqt
