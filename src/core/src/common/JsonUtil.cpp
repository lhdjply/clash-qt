#include "common/JsonUtil.h"

#include <QStringList>

namespace core {
namespace jsonutil {
namespace {

bool isPlainObject(const QJsonValue &value)
{
    // 与 merge.ts 的 isObject 对齐：仅对象算对象，数组不算
    return value.isObject();
}

} // namespace

QString trimWrap(const QString &key)
{
    if (key.size() >= 2 && key.startsWith(QLatin1Char('<')) && key.endsWith(QLatin1Char('>'))) {
        return key.mid(1, key.size() - 2);
    }
    return key;
}

QJsonObject deepMerge(QJsonObject target, const QJsonObject &other, bool isOverride)
{
    for (auto it = other.constBegin(); it != other.constEnd(); ++it) {
        const QString key = it.key();
        const QJsonValue value = it.value();

        if (isPlainObject(value)) {
            if (key.endsWith(QLatin1Char('!'))) {
                // 整体替换：不做递归合并
                const QString normalized = trimWrap(key.left(key.size() - 1));
                target.insert(normalized, value);
            } else {
                const QString normalized = trimWrap(key);
                QJsonValue existing = target.value(normalized);
                if (!existing.isObject()) {
                    existing = QJsonObject();
                }
                target.insert(normalized,
                              deepMerge(existing.toObject(), value.toObject(), isOverride));
            }
        } else if (value.isArray()) {
            const QJsonArray incoming = value.toArray();
            if (isOverride && key.startsWith(QLatin1Char('+'))) {
                const QString normalized = trimWrap(key.mid(1));
                const QJsonArray existing = target.value(normalized).toArray();
                QJsonArray merged = incoming;
                for (const auto &item : existing) merged.append(item);
                target.insert(normalized, merged);
            } else if (isOverride && key.endsWith(QLatin1Char('+'))) {
                const QString normalized = trimWrap(key.left(key.size() - 1));
                QJsonArray merged = target.value(normalized).toArray();
                for (const auto &item : incoming) merged.append(item);
                target.insert(normalized, merged);
            } else {
                target.insert(trimWrap(key), incoming);
            }
        } else {
            // 标量：连键名一起原样写入（尖括号在标量键上也应被去掉，与 JS 版
            // Object.assign(target, { [key]: value }) 后的结果保持一致——
            // 注意 JS 版此处保留原键名，因此这里同样保留，只做 trimWrap 兼容 <> 写法）
            target.insert(key.startsWith(QLatin1Char('<')) ? trimWrap(key) : key, value);
        }
    }
    return target;
}

QJsonValue getPath(const QJsonObject &object, const QString &path)
{
    const QStringList parts = path.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    if (parts.isEmpty()) return QJsonValue(QJsonValue::Undefined);

    QJsonValue current(object);
    for (const QString &part : parts) {
        if (!current.isObject()) return QJsonValue(QJsonValue::Undefined);
        const QJsonObject objectLevel = current.toObject();
        if (!objectLevel.contains(part)) return QJsonValue(QJsonValue::Undefined);
        current = objectLevel.value(part);
    }
    return current;
}

QJsonObject setPath(QJsonObject object, const QString &path, const QJsonValue &value)
{
    const QStringList parts = path.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    if (parts.isEmpty()) return object;

    if (parts.size() == 1) {
        object.insert(parts.first(), value);
        return object;
    }

    // 递归构建中间层，缺失或类型不符的中间节点重建为空对象
    const QString head = parts.first();
    QStringList tail = parts.mid(1);
    QJsonObject child = object.value(head).toObject();
    object.insert(head, setPath(child, tail.join(QLatin1Char('.')), value));
    return object;
}

QString stringValue(const QJsonObject &object, const QString &key, const QString &fallback)
{
    const QJsonValue value = object.value(key);
    if (value.isString()) return value.toString();
    // 协议侧常见把端口、uuid 写成数字，统一按字符串返回
    if (value.isDouble()) {
        const double number = value.toDouble();
        if (number == static_cast<long long>(number)) {
            return QString::number(static_cast<long long>(number));
        }
        return QString::number(number);
    }
    return fallback;
}

int intValue(const QJsonObject &object, const QString &key, int fallback)
{
    const QJsonValue value = object.value(key);
    if (value.isDouble()) return static_cast<int>(value.toDouble());
    if (value.isString()) {
        bool ok = false;
        const int parsed = value.toString().toInt(&ok, 10);
        if (ok) return parsed;
    }
    return fallback;
}

bool boolValue(const QJsonObject &object, const QString &key, bool fallback)
{
    const QJsonValue value = object.value(key);
    if (value.isBool()) return value.toBool();
    if (value.isString()) {
        const QString text = value.toString().trimmed().toLower();
        if (text == QLatin1String("true") || text == QLatin1String("1")) return true;
        if (text == QLatin1String("false") || text == QLatin1String("0")) return false;
    }
    if (value.isDouble()) return value.toDouble() != 0.0;
    return fallback;
}

QStringList stringArrayValue(const QJsonObject &object, const QString &key)
{
    QStringList result;
    const QJsonValue value = object.value(key);
    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        result.reserve(array.size());
        for (const auto &item : array) {
            if (item.isString()) {
                result << item.toString();
            } else if (item.isDouble()) {
                const double number = item.toDouble();
                if (number == static_cast<long long>(number)) {
                    result << QString::number(static_cast<long long>(number));
                } else {
                    result << QString::number(number);
                }
            }
        }
    } else if (value.isString()) {
        result << value.toString();
    }
    return result;
}

} // namespace jsonutil
} // namespace core
