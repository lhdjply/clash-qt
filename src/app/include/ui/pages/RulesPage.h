#pragma once

// ---------------------------------------------------------------------------
//  规则页
//
//  按匹配顺序列出生效中的规则，一条一张卡片：标题是规则的值，下面是规则
//  类型与出站策略，右侧是本次运行命中的次数与占比。双击卡片临时停用或
//  重新启用，只影响当前会话，重启后恢复。
// ---------------------------------------------------------------------------

#include <QWidget>

class QLabel;
class QListView;
class QStackedWidget;
class QTimer;

namespace clashqt {

class AppContext;
class RuleListModel;
class RuleRowDelegate;

class RulesPage : public QWidget {
    Q_OBJECT

public:
    explicit RulesPage(AppContext *context, QWidget *parent = nullptr);

    // 重建列表；页面不可见时不做无谓的刷新
    void refresh();

private:
    // 按规则描述文本设置停用状态
    void applyRuleDisabled(const QString &ruleText, bool disabled);

    AppContext      *m_context = nullptr;
    RuleListModel   *m_model = nullptr;
    RuleRowDelegate *m_delegate = nullptr;

    QListView      *m_list = nullptr;
    QStackedWidget *m_stack = nullptr;
    QLabel         *m_empty = nullptr;
    QTimer         *m_timer = nullptr;
};

} // namespace clashqt
