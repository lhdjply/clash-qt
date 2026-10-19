#pragma once

// ---------------------------------------------------------------------------
//  代理页
//
//  对应 clash-party 的 src/renderer/src/pages/proxies.tsx。
//  左侧选择代理组，右侧列出组成员；支持切换选中节点、单节点测速与整组测速。
//  单节点测速是行内的小闪电按钮（鼠标悬停时高亮），测速在独立线程里跑，
//  期间该行显示「测试中」，不阻塞界面。
// ---------------------------------------------------------------------------

#include <QQueue>
#include <QSet>
#include <QStringList>
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

protected:
    // 行内的测速按钮不是独立控件（列表行由委托绘制），这里在视口上做命中
    // 判断：点击落在按钮热区内就发起测速，悬停时把光标换成手型。
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onGroupChanged(int index);
    void onItemActivated();
    void onTestCurrent();
    void onTestAll();
    void onRestoreAuto();

private:
    // 重新填充组下拉框；尽量保持用户当前选择
    void rebuildGroups();

    // 发起单节点测速（入队，异步执行；失败时行内显示「超时」并弹出原因）
    void testSingleNode(const QString &name);
    // 入队（同一节点不会重复排队），并立刻把行内状态切成「测试中」
    void enqueueTest(const QString &name);
    // 按并发上限启动队列里的测速
    void startNextTests();
    // 一个节点测完：回填延迟、统计批量进度、继续出队
    void finishTest(const QString &name, int delayMs, const QString &error);
    // 批量测速：names 里可测的节点全部入队，完成后弹一条汇总
    void startBatch(const QStringList &names);
    // 命中测试：pos 是否落在该行的测速按钮上；命中时返回成员名
    QString testButtonAt(const QPoint &pos) const;
    // 把某行的延迟/测速中状态写回并重绘该行
    void setRowDelay(const QString &name, int delayMs);

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

    // 正在测速（含排队中）的节点：跨组切换保留，切回来仍显示「测试中」
    QSet<QString> m_testing;
    QQueue<QString> m_testQueue;
    int             m_activeTests = 0; // 已启动、尚未返回的数量

    // 最近一次测速失败（超时）的节点：列表重建后仍要显示「超时」，
    // 否则批量结束时的重选会把失败标记冲掉
    QSet<QString> m_failedTests;

    // 批量测速进度（m_batchNames 为空表示没有进行中的批量任务）
    QSet<QString> m_batchNames;
    int           m_batchTotal = 0;
    int           m_batchOk    = 0;
};

} // namespace clashqt
