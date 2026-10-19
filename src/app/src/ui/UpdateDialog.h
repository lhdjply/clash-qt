#pragma once

// ---------------------------------------------------------------------------
//  更新详情对话框
//
//  只负责展示：版本号、发布日期、Release 正文（Markdown），以及一个跳转发布页
//  的按钮。不做下载 —— 见 UpdateChecker 顶部关于「不自动替换自身」的说明。
//  单独成文件而不是塞进 SettingsPage.cpp：它需要被预览 / 截图验证版式。
//  不加 Q_OBJECT：只用 lambda 连接，不需要自有信号槽。
// ---------------------------------------------------------------------------

#include "updater/UpdateChecker.h"

#include <QDialog>

namespace clashqt {

class UpdateDialog : public QDialog {
public:
    explicit UpdateDialog(const ReleaseInfo &release, QWidget *parent = nullptr);
};

} // namespace clashqt
