#pragma once

// ---------------------------------------------------------------------------
//  代理页
//
//  对应 clash-party 的 src/renderer/src/pages/proxies.tsx。
//  左侧选择代理组，右侧列出组成员；支持切换选中节点与逐个/整组测速。
// ---------------------------------------------------------------------------

#include <QWidget>

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;

namespace clashqt {

class AppContext;

class ProxiesPage : public QWidget {
    Q_OBJECT

public:
    explicit ProxiesPage(AppContext *context, QWidget *parent = nullptr);

    // 从内核拉取最新组与节点状态并刷新界面
    void refresh();

private slots:
    void onGroupChanged(int index);
    void onItemActivated();
    void onTestCurrent();
    void onTestAll();
    void onRestoreAuto();

private:
    // 重新填充组下拉框；尽量保持用户当前选择
    void rebuildGroups();

    AppContext  *m_context = nullptr;
    QComboBox   *m_groupCombo = nullptr;
    QListWidget *m_memberList = nullptr;
    QLabel      *m_summaryLabel = nullptr;
    QPushButton *m_testButton = nullptr;
    QPushButton *m_testAllButton = nullptr;
    QPushButton *m_autoButton = nullptr;

    // 当前展示的组名与成员名，与列表行一一对应
    QString m_currentGroup;
    QStringList m_memberNames;
};

} // namespace clashqt
