#include "config/OverrideConfig.h"

#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QDateTime>
#include <QFile>
#include <QUuid>

#include "common/JsonUtil.h"
#include "common/Yaml.h"

namespace clashqt {

// ---------------------------------------------------------------------------
//  OverrideItem
// ---------------------------------------------------------------------------

OverrideItem OverrideItem::fromJson(const QJsonObject &object)
{
    OverrideItem item;
    item.id   = core::jsonutil::stringValue(object, QStringLiteral("id"));
    item.type = core::jsonutil::stringValue(object, QStringLiteral("type"),
                                             QStringLiteral("local"));
    item.ext  = core::jsonutil::stringValue(object, QStringLiteral("ext"),
                                             QStringLiteral("yaml"));
    item.name = core::jsonutil::stringValue(object, QStringLiteral("name"));
    item.updated = qint64(core::jsonutil::intValue(object, QStringLiteral("updated")));
    item.global  = core::jsonutil::boolValue(object, QStringLiteral("global"), false);
    item.url     = core::jsonutil::stringValue(object, QStringLiteral("url"));
    item.file    = core::jsonutil::stringValue(object, QStringLiteral("file"));
    return item;
}

QJsonObject OverrideItem::toJson() const
{
    QJsonObject object;
    object.insert(QStringLiteral("id"), id);
    object.insert(QStringLiteral("type"), type);
    object.insert(QStringLiteral("ext"), ext);
    object.insert(QStringLiteral("name"), name);
    if (updated > 0) object.insert(QStringLiteral("updated"), double(updated));
    if (global) object.insert(QStringLiteral("global"), true);
    if (!url.isEmpty()) object.insert(QStringLiteral("url"), url);
    if (!file.isEmpty()) object.insert(QStringLiteral("file"), file);
    return object;
}

// ---------------------------------------------------------------------------
//  OverrideConfig
// ---------------------------------------------------------------------------

OverrideConfig::OverrideConfig(QObject *parent) : QObject(parent) {}

bool OverrideConfig::load()
{
    bool ok = false;
    const QString content = safefile::readText(paths::overrideConfigPath(), &ok);

    m_items.clear();
    if (ok && !content.trimmed().isEmpty()) {
        QString    error;
        const auto value = core::Yaml::parse(content, &error);
        if (value && value->isObject()) {
            const QJsonArray items = value->toObject().value(QStringLiteral("items")).toArray();
            for (const QJsonValue &entry : items) {
                if (!entry.isObject()) continue;
                const OverrideItem item = OverrideItem::fromJson(entry.toObject());
                if (!item.id.isEmpty()) m_items.append(item);
            }
        } else if (!error.isEmpty()) {
            AppLogger::instance().error(QStringLiteral("覆写列表解析失败：%1").arg(error));
        }
    }

    emit configChanged();
    return true;
}

bool OverrideConfig::save()
{
    QJsonArray items;
    for (const OverrideItem &item : m_items) items.append(item.toJson());

    QJsonObject root;
    root.insert(QStringLiteral("items"), items);

    QString error;
    if (!safefile::writeAtomic(paths::overrideConfigPath(), core::Yaml::stringify(root), &error)) {
        AppLogger::instance().error(QStringLiteral("覆写列表保存失败：%1").arg(error));
        return false;
    }
    emit configChanged();
    return true;
}

OverrideItem OverrideConfig::item(const QString &id) const
{
    for (const OverrideItem &entry : m_items) {
        if (entry.id == id) return entry;
    }
    return OverrideItem();
}

QString OverrideConfig::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(12);
}

bool OverrideConfig::addItem(const OverrideItem &item)
{
    if (item.id.isEmpty() || contains(item.id)) return false;
    m_items.append(item);
    save();
    return true;
}

bool OverrideConfig::updateItem(const QString &id, const QJsonObject &patch)
{
    for (OverrideItem &entry : m_items) {
        if (entry.id != id) continue;
        entry = OverrideItem::fromJson(core::jsonutil::deepMerge(entry.toJson(), patch));
        save();
        return true;
    }
    return false;
}

bool OverrideConfig::removeItem(const QString &id)
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).id != id) continue;

        const OverrideItem removed = m_items.takeAt(i);
        // 连同脚本/内容与执行日志一起清理
        QFile::remove(paths::overridePath(removed.id, removed.ext));
        QFile::remove(paths::overridePath(removed.id, QStringLiteral("log")));
        save();
        return true;
    }
    return false;
}

QString OverrideConfig::content(const QString &id, const QString &ext, bool *ok) const
{
    return safefile::readText(paths::overridePath(id, ext), ok);
}

bool OverrideConfig::setContent(const QString &id, const QString &ext, const QString &content)
{
    QString error;
    if (!safefile::writeAtomic(paths::overridePath(id, ext), content, &error)) {
        AppLogger::instance().error(
            QStringLiteral("覆写内容写入失败（%1.%2）：%3").arg(id, ext, error));
        return false;
    }
    return true;
}

QStringList OverrideConfig::globalIds() const
{
    QStringList ids;
    for (const OverrideItem &item : m_items) {
        if (item.global) ids << item.id;
    }
    return ids;
}

} // namespace clashqt
