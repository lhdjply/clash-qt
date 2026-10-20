#pragma once

// ---------------------------------------------------------------------------
//  连接页
//
//  对应 clash-party 的 src/renderer/src/pages/connections.tsx。
//  展示内核当前的活动连接（目标、规则、出站链、流量、速度），
//  支持关闭选中连接与全部关闭。
// ---------------------------------------------------------------------------

#include <QWidget>

class QCheckBox;
class QLabel;
class QTableWidget;
class QTimer;

namespace clashqt {

class AppContext;

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
    AppContext   *m_context = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel       *m_summary = nullptr;
    QCheckBox    *m_autoRefresh = nullptr;
    QTimer       *m_refreshTimer = nullptr;

    // 连接 id 与表格行的对应关系
    // （连接 id 由内核生成，界面上以 UserRole 携带）
    QStringList m_connectionIds;
};

} // namespace clashqt
