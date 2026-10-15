#include "config/ProfileConfig.h"

#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QUuid>

#include "common/JsonUtil.h"
#include "common/Yaml.h"

namespace clashqt {

// ---------------------------------------------------------------------------
//  ProfileItem
// ---------------------------------------------------------------------------

ProfileItem ProfileItem::fromJson(const QJsonObject &object)
{
    ProfileItem item;
    item.id   = core::jsonutil::stringValue(object, QStringLiteral("id"));
    item.type = core::jsonutil::stringValue(object, QStringLiteral("type"),
                                             QStringLiteral("local"));
    item.name = core::jsonutil::stringValue(object, QStringLiteral("name"));
    item.url  = core::jsonutil::stringValue(object, QStringLiteral("url"));
    item.file = core::jsonutil::stringValue(object, QStringLiteral("file"));

    item.interval       = object.value(QStringLiteral("interval"));
    item.updated        = qint64(core::jsonutil::intValue(object, QStringLiteral("updated")));
    item.override       = core::jsonutil::stringArrayValue(object, QStringLiteral("override"));
    item.useProxy       = core::jsonutil::boolValue(object, QStringLiteral("useProxy"), false);
    item.substore       = core::jsonutil::boolValue(object, QStringLiteral("substore"), false);
    item.autoUpdate     = core::jsonutil::boolValue(object, QStringLiteral("autoUpdate"), false);
    item.allowFixedInterval =
        core::jsonutil::boolValue(object, QStringLiteral("allowFixedInterval"), false);
    item.userAgent   = core::jsonutil::stringValue(object, QStringLiteral("userAgent"));
    item.ageSecretKey = core::jsonutil::stringValue(object, QStringLiteral("ageSecretKey"));
    item.updateTimeout = core::jsonutil::intValue(object, QStringLiteral("updateTimeout"));
    item.pluginId      = core::jsonutil::stringValue(object, QStringLiteral("pluginId"));

    const QJsonObject extra = object.value(QStringLiteral("extra")).toObject();
    if (!extra.isEmpty()) {
        SubscriptionUserInfo info;
        info.upload   = qint64(extra.value(QStringLiteral("upload")).toDouble());
        info.download = qint64(extra.value(QStringLiteral("download")).toDouble());
        info.total    = qint64(extra.value(QStringLiteral("total")).toDouble());
        info.expire   = qint64(extra.value(QStringLiteral("expire")).toDouble());
        item.extra    = info;
    }
    return item;
}

QJsonObject ProfileItem::toJson() const
{
    QJsonObject object;
    object.insert(QStringLiteral("id"), id);
    object.insert(QStringLiteral("type"), type);
    object.insert(QStringLiteral("name"), name);

    if (!url.isEmpty()) object.insert(QStringLiteral("url"), url);
    if (!file.isEmpty()) object.insert(QStringLiteral("file"), file);
    if (!interval.isNull() && !interval.isUndefined()) {
        object.insert(QStringLiteral("interval"), interval);
    }
    if (updated > 0) object.insert(QStringLiteral("updated"), double(updated));
    if (!override.isEmpty()) {
        object.insert(QStringLiteral("override"), QJsonArray::fromStringList(override));
    }
    if (useProxy) object.insert(QStringLiteral("useProxy"), true);
    if (substore) object.insert(QStringLiteral("substore"), true);
    if (autoUpdate) object.insert(QStringLiteral("autoUpdate"), true);
    if (allowFixedInterval) object.insert(QStringLiteral("allowFixedInterval"), true);
    if (!userAgent.isEmpty()) object.insert(QStringLiteral("userAgent"), userAgent);
    if (!ageSecretKey.isEmpty()) object.insert(QStringLiteral("ageSecretKey"), ageSecretKey);
    if (updateTimeout > 0) object.insert(QStringLiteral("updateTimeout"), updateTimeout);
    if (!pluginId.isEmpty()) object.insert(QStringLiteral("pluginId"), pluginId);

    if (extra) {
        QJsonObject extraObject;
        extraObject.insert(QStringLiteral("upload"), double(extra->upload));
        extraObject.insert(QStringLiteral("download"), double(extra->download));
        extraObject.insert(QStringLiteral("total"), double(extra->total));
        extraObject.insert(QStringLiteral("expire"), double(extra->expire));
        object.insert(QStringLiteral("extra"), extraObject);
    }
    return object;
}

// ---------------------------------------------------------------------------
//  ProfileConfig
// ---------------------------------------------------------------------------

ProfileConfig::ProfileConfig(QObject *parent) : QObject(parent) {}

bool ProfileConfig::load()
{
    bool ok = false;
    const QString content = safefile::readText(paths::profileConfigPath(), &ok);

    m_items.clear();
    m_current.clear();

    if (ok && !content.trimmed().isEmpty()) {
        QString    error;
        const auto value = core::Yaml::parse(content, &error);
        if (value && value->isObject()) {
            const QJsonObject root = value->toObject();
            m_current = core::jsonutil::stringValue(root, QStringLiteral("current"));
            for (const QJsonValue &entry : root.value(QStringLiteral("items")).toArray()) {
                if (!entry.isObject()) continue;
                const ProfileItem item = ProfileItem::fromJson(entry.toObject());
                if (!item.id.isEmpty()) m_items.append(item);
            }
        } else if (!error.isEmpty()) {
            AppLogger::instance().error(QStringLiteral("订阅列表解析失败：%1").arg(error));
        }
    }

    // 当前项已被删除时回退到第一项，避免「当前订阅为空」导致内核无配置可用
    if (!m_current.isEmpty() && !contains(m_current)) m_current.clear();
    if (m_current.isEmpty() && !m_items.isEmpty()) m_current = m_items.first().id;

    emit configChanged();
    return true;
}

bool ProfileConfig::save()
{
    QJsonObject root;
    if (!m_current.isEmpty()) root.insert(QStringLiteral("current"), m_current);

    QJsonArray items;
    for (const ProfileItem &item : m_items) items.append(item.toJson());
    root.insert(QStringLiteral("items"), items);

    QString error;
    if (!safefile::writeAtomic(paths::profileConfigPath(), core::Yaml::stringify(root), &error)) {
        AppLogger::instance().error(QStringLiteral("订阅列表保存失败：%1").arg(error));
        return false;
    }
    emit configChanged();
    return true;
}

ProfileItem ProfileConfig::currentItem() const { return item(m_current); }

ProfileItem ProfileConfig::item(const QString &id) const
{
    for (const ProfileItem &entry : m_items) {
        if (entry.id == id) return entry;
    }
    return ProfileItem();
}

bool ProfileConfig::setCurrent(const QString &id)
{
    if (!contains(id)) return false;
    m_current = id;
    save();
    return true;
}

QString ProfileConfig::generateId() const
{
    // UUID 前 12 位足够避免冲突，同时保持文件名短、便于用户识别
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(12);
}

bool ProfileConfig::addItem(const ProfileItem &item, bool switchTo)
{
    if (item.id.isEmpty()) return false;
    if (contains(item.id)) return false;

    m_items.append(item);
    if (switchTo || m_current.isEmpty()) m_current = item.id;
    save();
    return true;
}

bool ProfileConfig::updateItem(const QString &id, const QJsonObject &patch)
{
    for (ProfileItem &entry : m_items) {
        if (entry.id != id) continue;

        // 以 JSON 往返实现部分字段更新，避免为每个字段写重复分支
        QJsonObject merged = core::jsonutil::deepMerge(entry.toJson(), patch);
        entry = ProfileItem::fromJson(merged);
        save();
        return true;
    }
    return false;
}

bool ProfileConfig::removeItem(const QString &id)
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).id != id) continue;

        const ProfileItem removed = m_items.takeAt(i);
        // 同时删除订阅内容文件，避免残留
        if (!removed.file.isEmpty()) {
            QFile::remove(paths::profilePath(removed.id));
        }
        if (m_current == id) {
            m_current = m_items.isEmpty() ? QString() : m_items.first().id;
        }
        save();
        return true;
    }
    return false;
}

QString ProfileConfig::profileContent(const QString &id, bool *ok) const
{
    return safefile::readText(paths::profilePath(id), ok);
}

bool ProfileConfig::setProfileContent(const QString &id, const QString &content)
{
    QString error;
    if (!safefile::writeAtomic(paths::profilePath(id), content, &error)) {
        AppLogger::instance().error(
            QStringLiteral("订阅内容写入失败（%1）：%2").arg(id, error));
        return false;
    }
    return true;
}

std::optional<SubscriptionUserInfo> ProfileConfig::parseUserInfo(const QString &headerValue)
{
    // 标准形如：upload=1234; download=5678; total=10737418240; expire=1735660800
    if (headerValue.trimmed().isEmpty()) return std::nullopt;

    SubscriptionUserInfo info;
    bool                 matched = false;

    static const QRegularExpression pattern(
        QStringLiteral("(upload|download|total|expire)\\s*=\\s*(\\d+)"));

    auto it = pattern.globalMatch(headerValue);
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        const QString key   = match.captured(1).toLower();
        const qint64  value = match.captured(2).toLongLong();
        matched = true;

        if (key == QLatin1String("upload")) {
            info.upload = value;
        } else if (key == QLatin1String("download")) {
            info.download = value;
        } else if (key == QLatin1String("total")) {
            info.total = value;
        } else if (key == QLatin1String("expire")) {
            info.expire = value;
        }
    }

    if (!matched) return std::nullopt;
    return info;
}

void ProfileConfig::touchUpdated(const QString &id)
{
    for (ProfileItem &entry : m_items) {
        if (entry.id != id) continue;
        entry.updated = QDateTime::currentSecsSinceEpoch();
        save();
        return;
    }
}

} // namespace clashqt
