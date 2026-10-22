#pragma once

// ---------------------------------------------------------------------------
//  JSON 树工具
//
//  对齐参照实现的 src/main/utils/merge.ts。该 deepMerge 有一套键名修饰语法，
//  被覆写文件（override yaml）与插件配置大量使用，必须在 C++ 侧完整复刻：
//
//    "key"     普通合并；两侧都是对象时递归合并，否则整体覆盖
//    "<key>"   键名两侧的尖括号只是书写习惯，合并后会被去掉
//    "key!"    对象整体替换（不做递归合并），同样会去掉结尾的 '!'
//    "+key"    仅在 isOverride=true 时生效：把数组前置拼接到已有数组
//    "key+"    仅在 isOverride=true 时生效：把数组后置拼接到已有数组
//    其他数组  整体覆盖
// ---------------------------------------------------------------------------

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace core {

namespace jsonutil {

// 去掉键名两侧的尖括号
QString trimWrap(const QString &key);

// 就地合并 other 到 target，返回 target。语义与 merge.ts 完全一致。
QJsonObject deepMerge(QJsonObject target, const QJsonObject &other, bool isOverride = false);

// 从对象中按路径读取值（点号分隔），任一层缺失返回 Undefined
QJsonValue getPath(const QJsonObject &object, const QString &path);

// 写入值到指定路径，返回写入后的对象（缺失层级会自动创建为对象）
QJsonObject setPath(QJsonObject object, const QString &path, const QJsonValue &value);

// 便捷取值：带默认值，并把数字统一转成 double，避免 int/double 混淆
QString  stringValue(const QJsonObject &object, const QString &key, const QString &fallback = {});
int      intValue(const QJsonObject &object, const QString &key, int fallback = 0);
bool     boolValue(const QJsonObject &object, const QString &key, bool fallback = false);
QStringList stringArrayValue(const QJsonObject &object, const QString &key);

} // namespace jsonutil

} // namespace core
