#pragma once

// ---------------------------------------------------------------------------
//  覆写页 —— 自定义规则（可视化编辑）
//
//  规则语法对多数人来说是门槛，所以这里不要求手写：一条规则被拆成
//  「类型 / 内容 / 策略」三个字段，全部通过下拉与输入框完成，保存时再拼成
//  标准规则文本落盘。交互对齐参照实现（clash-party 的规则编辑弹窗）。
//
//  落盘位置：<数据目录>/rules/<订阅 id>.yaml
//    prepend   自定义规则（顺序即优先级，插到订阅规则最前）
//    disabled  被停用的规则：文本仍留在 prepend 里（不丢内容），应用时跳过
//    append/delete  由其他入口维护，本页只原样保留、不覆盖
//
//  没有「保存」按钮：任何改动（新增 / 修改 / 删除 / 排序 / 启停）都会在
//  停手后自动落盘并重载内核。合并式防抖是必需的 —— 重载会断开现有连接，
//  连点几下不该触发多次重载。
// ---------------------------------------------------------------------------

#include <QList>
#include <QString>
#include <QWidget>

class QLabel;
class QTableWidget;
class QTimer;

namespace clashqt {

class AppContext;

class OverridePage : public QWidget {
    Q_OBJECT

public:
    explicit OverridePage(AppContext *context, QWidget *parent = nullptr);

    // 从当前订阅的规则覆写文件重新载入列表
    void refresh();

protected:
    // 规则覆写按订阅保存：用户可能刚在「订阅」页换了订阅，每次切回都重读
    void showEvent(QShowEvent *event) override;
    // 离开页面时把待落盘的改动立刻写完，避免切订阅后写到别的订阅上
    void hideEvent(QHideEvent *event) override;

private:
    struct RuleEntry {
        QString text; // 标准规则文本（保存到 prepend 的就是它）
        bool    enabled = true;
    };

    void rebuildTable();
    void updateTableHeight();
    // 打开规则编辑弹窗；row < 0 表示新增
    void editRule(int row);
    void removeRule(int row);
    void moveRule(int row, int delta);
    // 有改动时调用：防抖后统一落盘并重载，避免连续操作触发多次重载
    void scheduleSave();
    void persist();
    void updateSummary();

    // 策略候选：DIRECT / REJECT + 全部代理组 + 全部节点
    QStringList targetNames() const;
    // 规则集候选：配置里的 rule-providers 名称
    QStringList ruleSetNames() const;

    AppContext   *m_context = nullptr;
    QLabel       *m_profileLabel = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel       *m_summaryLabel = nullptr;
    QLabel       *m_statusLabel = nullptr;

    QList<RuleEntry> m_rules;
    bool m_rebuilding = false;
    QTimer *m_saveTimer = nullptr;
};

} // namespace clashqt
