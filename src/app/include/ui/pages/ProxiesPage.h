#pragma once

// ---------------------------------------------------------------------------
//  代理页
//
//  选择代理组后，组成员按卡片排成网格：点卡片切换节点，卡片右侧是最近一次
//  测速的延迟，鼠标悬停时露出测速闪电。测速在独立线程里跑，期间卡片显示
//  「测试中」，不阻塞界面。
// ---------------------------------------------------------------------------

#include <QQueue>
#include <QSet>
#include <QStringList>
#include <QWidget>

class QComboBox;
class QListWidget;
class QListWidgetItem;
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
    void onSortChanged(int index);
    void onTestCurrent();
    void onTestAll();

private:
    // 点卡片切换节点（测速热区的点击在 eventFilter 里就被拦下了）
    void activateItem(QListWidgetItem *item);
    // 按视口宽度重算网格：一排放几张卡片
    void updateGridSize();

    // 列表排序方式。默认顺序即订阅里成员的原始排列，不做任何重排。
    enum class SortMode { Default, Name, Type, Delay };

    // 排序方式与配置字符串（default / name / type / delay）互转
    static QString  sortKey(SortMode mode);
    static SortMode sortModeFromKey(const QString &key);

    // 重新填充组下拉框；尽量保持用户当前选择
    void rebuildGroups();

    // 按 m_sortMode 重排当前列表的成员行（默认顺序直接返回）
    void sortRows();

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
    QComboBox   *m_sortCombo = nullptr;
    QListWidget *m_memberList = nullptr;
    QPushButton *m_testButton = nullptr;
    QPushButton *m_testAllButton = nullptr;

    SortMode m_sortMode = SortMode::Default;

    // 当前展示的组名与成员名，与列表行一一对应
    QString m_currentGroup;
    QStringList m_memberNames;

    // 上次构建列表时的出站模式是否为全局：用于在模式切换后把焦点从 GLOBAL
    // 挪进/挪出（见 rebuildGroups）
    bool m_globalModeShown = false;

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
