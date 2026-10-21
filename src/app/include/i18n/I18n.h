#pragma once

// ---------------------------------------------------------------------------
//  界面语言
//
//  源码里的界面文案以英文为源语言（tr() 的原文），中文由
//  translations/clash-qt_zh_CN.ts 提供、编译成 .qm 后随资源内嵌。
//  英文不需要翻译文件，加载不到 .qm 时 Qt 会原样返回英文，界面不会残缺。
//
//  Qt 自己的文案（输入框右键菜单、QMessageBox 标准按钮、标准对话框）不在
//  上面的 .ts 里：它们由 Qt 自带的 qtbase_zh_CN.qm 提供，见 install()。
//
//  语言在启动时确定：QTranslator 只影响之后创建的部件，运行期切换没有意义
//  （界面是一次性构造出来的），所以设置页改完语言会提示重启。
// ---------------------------------------------------------------------------

#include <QCoreApplication>
#include <QString>

namespace clashqt::i18n {

// 配置值（system / zh-CN / en-US / 空）→ 实际语言代码（zh / en）
QString resolve(const QString &setting);

// 启动时调用一次：按配置安装翻译器。返回实际生效的语言代码。
QString install(QCoreApplication *app, const QString &setting);

// 当前生效的语言代码（zh / en），供界面显示与测试使用
QString current();

} // namespace clashqt::i18n
