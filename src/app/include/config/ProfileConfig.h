#pragma once

// ---------------------------------------------------------------------------
//  订阅（配置档案）管理
//
//  对应参照实现的 src/main/config/profile.ts。
//
//    profile.yaml           订阅列表与当前选中项
//    profiles/<id>.yaml     各订阅的原始配置内容
//
//  支持三种来源（对齐原实现的 IProfileItem.type）：
//    remote  远程订阅 URL，可定期自动更新
//    local   用户本地的配置文件
//    plugin  由插件（机场）下发的订阅，由插件子系统维护
//
//  subscription-userinfo 响应头会被解析出流量与到期信息（对应原实现的
//  extra 字段），用于在订阅卡片上显示已用/总量。
// ---------------------------------------------------------------------------

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

#include <optional>

namespace clashqt {

struct SubscriptionUserInfo {
    qint64 upload   = 0;
    qint64 download = 0;
    qint64 total    = 0;
    qint64 expire   = 0; // Unix 秒
};

struct ProfileItem {
    QString     id;
    QString     type = QStringLiteral("local"); // remote / local / plugin
    QString     name;
    QString     url;                            // remote
    QString     file;                           // local（相对 profiles 目录的文件名）
    QJsonValue  interval;                       // 更新间隔（分钟），可为数字或 "cron"
    qint64      updated = 0;                    // 最近更新时间（Unix 秒）
    QStringList override;                       // 关联的覆写 id 列表
    bool        useProxy = false;
    bool        substore = false;
    bool        autoUpdate = false;
    bool        allowFixedInterval = false;
    QString     userAgent;
    QString     ageSecretKey;
    int         updateTimeout = 0;
    QString     pluginId;

    std::optional<SubscriptionUserInfo> extra;

    static ProfileItem fromJson(const QJsonObject &object);
    QJsonObject        toJson() const;
};

class ProfileConfig : public QObject {
    Q_OBJECT

public:
    explicit ProfileConfig(QObject *parent = nullptr);

    bool load();
    bool save();

    const QList<ProfileItem> &items() const { return m_items; }
    QString                   current() const { return m_current; }
    // 切换当前订阅；返回是否成功（订阅不存在时失败）
    bool    setCurrent(const QString &id);
    ProfileItem currentItem() const;
    ProfileItem item(const QString &id) const;
    bool        contains(const QString &id) const { return !item(id).id.isEmpty(); }

    // 新增 / 更新 / 删除
    bool addItem(const ProfileItem &item, bool switchTo = false);
    bool updateItem(const QString &id, const QJsonObject &patch);
    bool removeItem(const QString &id);

    // ---- 订阅内容 ---------------------------------------------------------
    QString profileContent(const QString &id, bool *ok = nullptr) const;
    bool    setProfileContent(const QString &id, const QString &content);

    // 生成一个不与现有项冲突的 id
    QString generateId() const;

    // 记录订阅流量信息（来自响应头），并返回是否解析成功
    static std::optional<SubscriptionUserInfo> parseUserInfo(const QString &headerValue);

    // 记录最近更新时间
    void touchUpdated(const QString &id);

signals:
    void configChanged();

private:
    QList<ProfileItem> m_items;
    QString            m_current;
};

} // namespace clashqt
