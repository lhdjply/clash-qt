#pragma once

// ---------------------------------------------------------------------------
//  订阅页
//
//  对应 clash-party 的 src/renderer/src/pages/profiles.tsx。
//  展示订阅列表（含流量与到期信息），支持添加远程/本地订阅、
//  更新、切换当前订阅与删除。
// ---------------------------------------------------------------------------

#include <QWidget>

class QLabel;
class QTableWidget;

namespace clashqt {

class AppContext;

class ProfilesPage : public QWidget {
    Q_OBJECT

public:
    explicit ProfilesPage(AppContext *context, QWidget *parent = nullptr);

    void refresh();

private slots:
    void onAddSubscription();
    void onAddLocal();
    void onUpdateSelected();
    void onUpdateAll();
    void onSwitchSelected();
    void onRemoveSelected();

private:
    // 当前选中的订阅 id（未选中返回空）
    QString selectedId() const;

    AppContext   *m_context = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel       *m_emptyHint = nullptr;
};

} // namespace clashqt
