#include "common/Yaml.h"

#include "base/Common.h"

#include <yaml-cpp/yaml.h>

#include <QJsonDocument>

#include <cmath>
#include <sstream>

namespace core {
namespace {

// ---------------------------------------------------------------------------
//  YAML::Node -> QJsonValue
// ---------------------------------------------------------------------------

QJsonValue nodeToJson(const YAML::Node &node)
{
    switch (node.Type()) {
    case YAML::NodeType::Null:
    case YAML::NodeType::Undefined:
        return QJsonValue(QJsonValue::Null);

    case YAML::NodeType::Scalar: {
        const std::string raw = node.Scalar();
        const QString text = QString::fromStdString(raw);

        // 布尔字面量：YAML 1.1 允许 true/false/yes/no/on/off，统一归一为布尔。
        // 这里只接受 YAML 规范保证会被解析为 bool 的写法，避免把 "no" 这类
        // 可能是用户真实字符串值的标量误判——yaml-cpp 已提供 Tag() 供判断。
        if (node.Tag() == "!" || node.Tag() == "!!bool") {
            const auto parsed = strutil::parseBool(text);
            if (parsed) return QJsonValue(*parsed);
        }
        if (node.Tag() == "!!int" || node.Tag() == "!!float") {
            bool ok = false;
            const double number = text.toDouble(&ok);
            if (ok) return QJsonValue(number);
        }
        if (node.Tag() == "!!null") {
            return QJsonValue(QJsonValue::Null);
        }
        return QJsonValue(text);
    }

    case YAML::NodeType::Sequence: {
        QJsonArray array;
        for (const auto &item : node) {
            array.append(nodeToJson(item));
        }
        return array;
    }

    case YAML::NodeType::Map: {
        QJsonObject object;
        for (const auto &entry : node) {
            const QString key = QString::fromStdString(entry.first.Scalar());
            object.insert(key, nodeToJson(entry.second));
        }
        return object;
    }
    }
    return QJsonValue(QJsonValue::Null);
}

// ---------------------------------------------------------------------------
//  QJsonValue -> YAML::Node
// ---------------------------------------------------------------------------

YAML::Node jsonToNode(const QJsonValue &value)
{
    YAML::Node node;

    switch (value.type()) {
    case QJsonValue::Null:
    case QJsonValue::Undefined:
        node = YAML::Node(YAML::NodeType::Null);
        break;

    case QJsonValue::Bool:
        node = value.toBool();
        break;

    case QJsonValue::Double: {
        const double number = value.toDouble();
        // 整数按整数发射，避免端口、interval 等字段被写成 7890.0
        if (std::isfinite(number) && number == std::floor(number)
            && std::abs(number) < 9.0e15) {
            node = static_cast<long long>(number);
        } else {
            node = number;
        }
        break;
    }

    case QJsonValue::String:
        node = value.toString().toStdString();
        break;

    case QJsonValue::Array: {
        node = YAML::Node(YAML::NodeType::Sequence);
        const QJsonArray array = value.toArray();
        for (const auto &item : array) {
            node.push_back(jsonToNode(item));
        }
        break;
    }

    case QJsonValue::Object: {
        node = YAML::Node(YAML::NodeType::Map);
        const QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            node[it.key().toStdString()] = jsonToNode(it.value());
        }
        break;
    }
    }

    return node;
}

} // namespace

std::optional<QJsonValue> Yaml::parse(const QString &text, QString *errorMessage)
{
    if (text.trimmed().isEmpty()) {
        return QJsonValue(QJsonValue::Null);
    }

    try {
        const YAML::Node root = YAML::Load(text.toStdString());
        return nodeToJson(root);
    } catch (const YAML::Exception &error) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("YAML 解析失败：%1").arg(
                QString::fromStdString(error.what()));
        }
        return std::nullopt;
    }
}

QString Yaml::stringify(const QJsonValue &value, int indent)
{
    if (value.isNull() || value.isUndefined()) return QString();

    try {
        YAML::Emitter emitter;
        emitter.SetIndent(indent <= 0 ? 2 : indent);
        // 让长字符串换行而不是折行，避免破坏 base64、正则等内容
        emitter.SetStringFormat(YAML::SingleQuoted);
        emitter << jsonToNode(value);

        QString result = QString::fromStdString(emitter.c_str());
        if (!result.endsWith(QLatin1Char('\n'))) result.append(QLatin1Char('\n'));
        return result;
    } catch (const YAML::Exception &error) {
        Logger::instance().error(
            LogCategory::Config,
            QStringLiteral("YAML 发射失败：%1").arg(QString::fromStdString(error.what())));
        return QString();
    }
}

QJsonObject Yaml::parseObject(const QString &text, QString *errorMessage)
{
    const auto parsed = parse(text, errorMessage);
    if (!parsed || !parsed->isObject()) return QJsonObject();
    return parsed->toObject();
}

} // namespace core
