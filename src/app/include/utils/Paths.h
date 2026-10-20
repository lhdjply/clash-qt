#pragma once

// ---------------------------------------------------------------------------
//  目录与路径约定
//
//  对应 clash-party 的 src/main/utils/dirs.ts。数据目录布局保持一致，
//  使两份实现的配置与订阅文件可以互相迁移：
//
//    <dataDir>/config.yaml          应用配置
//    <dataDir>/kernel.yaml          受控内核配置
//    <dataDir>/profile.yaml         订阅（配置档案）列表
//    <dataDir>/override.yaml        覆写列表
//    <dataDir>/profiles/<id>.yaml   各订阅的原始内容
//    <dataDir>/override/<id>.{js,yaml,log}
//    <dataDir>/rules/<id>.yaml      规则覆写
//    <dataDir>/work/config.yaml     生成后的内核工作配置
//    <dataDir>/logs/*.log           应用与内核日志
//
//  平台差异：Linux 使用 $XDG_CONFIG_HOME/clash-qt（默认 ~/.config/clash-qt）；
//  若应用目录存在 PORTABLE 标记文件，则改用 <exeDir>/data（便携模式）。
// ---------------------------------------------------------------------------

#include <QString>

namespace clashqt {

namespace paths {

// 应用数据目录（保证存在）
QString dataDir();
// 可执行文件所在目录
QString exeDir();
// 便携模式（exeDir/PORTABLE 存在）
bool isPortable();

QString appConfigPath();            // config.yaml
QString profileConfigPath();        // profile.yaml
QString overrideConfigPath();       // override.yaml

QString profilesDir();              // profiles/
QString profilePath(const QString &id);
QString overrideDir();              // override/
QString overridePath(const QString &id, const QString &ext);
QString rulesDir();                 // rules/
QString rulePath(const QString &id);

QString workDir();                  // work/
QString workConfigPath();           // work/config.yaml

QString logDir();                   // logs/
QString appLogPath();               // logs/clash-qt-YYYY-MM-DD.log
QString coreLogPath();              // logs/core-YYYY-MM-DD.log

// 确保目录存在（递归创建）；失败返回 false
bool ensureDir(const QString &path);
// 确保某个文件所在的父目录存在
bool ensureParentDir(const QString &filePath);

} // namespace paths

} // namespace clashqt
