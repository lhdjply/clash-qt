#pragma once

// ---------------------------------------------------------------------------
//  覆写（override.yaml + override/<id>.{js,yaml,log}）
//
//  对应 clash-party 的 src/main/config/override.ts。
//
//  覆写的作用是在「订阅原始内容」与「最终内核配置」之间插入用户自定义修改：
//    * yaml 覆写：按 deepMerge 语义合并，支持 "<key>"、"key!"、"+key" 等修饰
//    * js   覆写：脚本方式任意改写配置
//
//  ---------------------------------------------------------------------------
//  当前阶段的实现边界（重要）
//  ---------------------------------------------------------------------------
//  本阶段只执行 yaml 覆写。js 覆写需要嵌入一个 JS 引擎（Go 侧用 goja，
//  本项目需要引入 QuickJS 或依赖 Qt6 Qml 的 QJSEngine），属于独立工作项。
//
//  因此遇到 js 覆写时：记录明确的告警并跳过，绝不静默忽略——静默跳过会让
//  用户以为覆写已生效，实际配置与预期不符，属于难以察觉的错误。
// ---------------------------------------------------------------------------

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

namespace clashqt {

struct OverrideItem {
    QString id;
    QString type = QStringLiteral("local");  // remote / local
    QString ext  = QStringLiteral("yaml");   // js / yaml
    QString name;
    qint64  updated = 0;
    bool    global  = false;                 // 是否对所有订阅生效
    QString url;                             // remote
    QString file;                            // local

    static OverrideItem fromJson(const QJsonObject &object);
    QJsonObject         toJson() const;
};

class OverrideConfig : public QObject {
    Q_OBJECT

public:
    explicit OverrideConfig(QObject *parent = nullptr);

    bool load();
    bool save();

    const QList<OverrideItem> &items() const { return m_items; }
    OverrideItem item(const QString &id) const;
    bool         contains(const QString &id) const { return !item(id).id.isEmpty(); }

    bool addItem(const OverrideItem &item);
    bool updateItem(const QString &id, const QJsonObject &patch);
    bool removeItem(const QString &id);

    // 覆盖文件内容读写；ext 取 "yaml" / "js" / "log"
    QString content(const QString &id, const QString &ext, bool *ok = nullptr) const;
    bool    setContent(const QString &id, const QString &ext, const QString &content);

    // 全局覆写 id 列表（按定义顺序）
    QStringList globalIds() const;

    QString generateId() const;

signals:
    void configChanged();

private:
    QList<OverrideItem> m_items;
};

} // namespace clashqt
