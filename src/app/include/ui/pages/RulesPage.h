#pragma once

// ---------------------------------------------------------------------------
//  规则页
//  对应参照实现的 src/renderer/src/pages/rules.tsx。
//  列出内核当前生效的规则，支持运行期禁用/启用单条规则。
// ---------------------------------------------------------------------------

#include <QWidget>

class QTableWidget;

namespace clashqt {

class AppContext;

class RulesPage : public QWidget {
    Q_OBJECT

public:
    explicit RulesPage(AppContext *context, QWidget *parent = nullptr);

    void refresh();

private slots:
    void onToggleRule(int row, int column);

private:
    AppContext   *m_context = nullptr;
    QTableWidget *m_table = nullptr;

    // 规则描述文本（与内核 Rule::description() 一致），用于启停接口定位
    QStringList m_ruleTexts;
};

} // namespace clashqt
