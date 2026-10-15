#pragma once

// ---------------------------------------------------------------------------
//  YAML <-> Qt JSON 的统一数据模型
//
//  为什么以「JSON 树」作为配置的规范表示：
//    * 真实 Clash / 参照实现配置包含大量本应用不认识的字段（插件、实验性选项、
//      协议私有参数）。若为每个字段声明 C++ 成员，未知字段会在「读入→写出」
//      过程中被静默丢弃，导致用户配置损坏。
//    * 以 QJsonObject 为唯一载体，所有字段原样保留，仅对需要操纵的字段做
//      类型化访问，等价于原实现对 yaml 库的使用方式。
//
//  YAML 侧使用内置的 yaml-cpp 0.8 完成解析与发射。
//
//  已知限制（Phase 2 处理）：yaml-cpp 的映射节点使用有序容器以外的实现，
//  重新发射时同一映射内的键顺序可能与源文件不同。YAML 映射键顺序无语义
//  影响，因此不影响功能；如需逐字节还原原始排版，需要引入保序解析器。
// ---------------------------------------------------------------------------

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <optional>

namespace core {

class Yaml {
public:
    // 将 YAML 文本解析为 JSON 值。解析失败返回 nullopt，并把错误信息写入
    // errorMessage（若提供）。空文档返回空的 QJsonValue::Null。
    static std::optional<QJsonValue> parse(const QString &text, QString *errorMessage = nullptr);

    // 将 JSON 值发射为 YAML 文本
    static QString stringify(const QJsonValue &value, int indent = 2);

    // 便捷重载：解析为映射；顶层不是映射时返回空 QJsonObject
    static QJsonObject parseObject(const QString &text, QString *errorMessage = nullptr);
};

} // namespace core
