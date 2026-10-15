#include "config/Factory.h"

#include "config/AppConfig.h"
#include "config/ControledKernelConfig.h"
#include "config/OverrideConfig.h"
#include "config/ProfileConfig.h"
#include "utils/AppLogger.h"
#include "utils/Paths.h"
#include "utils/SafeFile.h"

#include <QFile>
#include <QJsonArray>
#include <QSet>

#include <algorithm>

#include "common/JsonUtil.h"
#include "common/Yaml.h"

namespace clashqt {
namespace {

// 规则覆写中的「偏移插入」语法：
//   "3,DOMAIN,x,DIRECT"  -> 插到第 3 条之前
//   append 形式同理，从末尾往前数
// 非数字开头的条目按普通规则追加到对应组。
struct RuleInsertResult {
    QStringList normal;
    QStringList inserted;
};

RuleInsertResult processRulesWithOffset(const QStringList &ruleStrings,
                                        const QStringList &currentRules, bool isAppend)
{
    RuleInsertResult result;
    result.inserted = currentRules;

    for (const QString &ruleString : ruleStrings) {
        const QStringList parts = ruleString.split(QLatin1Char(','));
        bool              firstIsNumber = false;

        if (parts.size() >= 3 && !parts.first().trimmed().isEmpty()) {
            bool ok = false;
            parts.first().trimmed().toInt(&ok);
            firstIsNumber = ok;
        }

        if (!firstIsNumber) {
            result.normal << ruleString;
            continue;
        }

        const int     offset = parts.first().trimmed().toInt();
        QStringList   rest   = parts.mid(1);
        const QString rule   = rest.join(QLatin1Char(','));

        if (isAppend) {
            // QList::size() 返回 qsizetype，需显式统一类型再做比较
            const int position = std::max<qsizetype>(0, qsizetype(result.inserted.size())
                                                        - std::min<qsizetype>(offset,
                                                                            result.inserted.size()));
            result.inserted.insert(int(position), rule);
        } else {
            const int position = int(std::min<qsizetype>(offset, result.inserted.size()));
            result.inserted.insert(position, rule);
        }
    }
    return result;
}

} // namespace

Factory::Factory(AppConfig *appConfig, ControledKernelConfig *controledConfig,
                 ProfileConfig *profileConfig, OverrideConfig *overrideConfig)
    : m_appConfig(appConfig),
      m_controledConfig(controledConfig),
      m_profileConfig(profileConfig),
      m_overrideConfig(overrideConfig)
{
}

QString Factory::ruleOverridePath() const
{
    const QString id = m_profileConfig ? m_profileConfig->current() : QString();
    return paths::rulePath(id.isEmpty() ? QStringLiteral("default") : id);
}

bool Factory::applyOverrides(core::RawConfig *config, const QStringList &overrideIds,
                             QStringList *warnings)
{
    if (!m_overrideConfig) return true;

    for (const QString &overrideId : overrideIds) {
        const OverrideItem item = m_overrideConfig->item(overrideId);
        if (item.id.isEmpty()) {
            if (warnings) {
                warnings->append(QStringLiteral("引用了不存在的覆写：%1").arg(overrideId));
            }
            continue;
        }

        bool          ok = false;
        const QString content = m_overrideConfig->content(item.id, item.ext, &ok);
        if (!ok) {
            if (warnings) {
                warnings->append(QStringLiteral("覆写 %1 的内容文件缺失").arg(item.name));
            }
            continue;
        }

        if (item.ext == QLatin1String("js")) {
            // 明确告警而不是静默跳过：静默跳过会让用户以为覆写已生效
            if (warnings) {
                warnings->append(
                    QStringLiteral("覆写「%1」是 js 脚本，当前尚未支持 JS 覆写引擎，"
                                   "已跳过（配置不会应用该覆写）")
                        .arg(item.name));
            }
            continue;
        }

        QString    parseError;
        const auto patch = core::Yaml::parse(content, &parseError);
        if (!patch) {
            if (warnings) {
                warnings->append(QStringLiteral("覆写「%1」解析失败：%2")
                                     .arg(item.name, parseError));
            }
            continue;
        }
        if (!patch->isObject()) {
            if (warnings) {
                warnings->append(QStringLiteral("覆写「%1」的根节点必须是映射").arg(item.name));
            }
            continue;
        }

        // isOverride=true：启用 "+key" / "key+" 数组拼接语义
        config->applyPatch(patch->toObject(), /*isOverride=*/true);
    }
    return true;
}

void Factory::applyRuleOverride(core::RawConfig *config, const QString &profileId,
                                QStringList *warnings)
{
    const QString path = paths::rulePath(profileId.isEmpty() ? QStringLiteral("default")
                                                            : profileId);
    if (!QFile::exists(path)) return;

    bool          ok = false;
    const QString content = safefile::readText(path, &ok);
    if (!ok || content.trimmed().isEmpty()) return;

    QString    parseError;
    const auto parsed = core::Yaml::parse(content, &parseError);
    if (!parsed || !parsed->isObject()) {
        if (warnings) {
            warnings->append(QStringLiteral("规则覆写文件解析失败：%1").arg(parseError));
        }
        return;
    }

    const QJsonObject root = parsed->toObject();
    QJsonArray        rules = config->rules();

    QStringList current;
    for (const QJsonValue &value : rules) {
        if (value.isString()) current << value.toString();
    }

    const auto readList = [&root](const QString &key) {
        QStringList list;
        for (const QJsonValue &value : root.value(key).toArray()) {
            if (value.isString()) list << value.toString();
        }
        return list;
    };

    // prepend：支持偏移插入，插到列表前部
    const QStringList prependList = readList(QStringLiteral("prepend"));
    if (!prependList.isEmpty()) {
        const RuleInsertResult result = processRulesWithOffset(prependList, current, false);
        current = result.normal + result.inserted;
    }

    // append：插到列表后部
    const QStringList appendList = readList(QStringLiteral("append"));
    if (!appendList.isEmpty()) {
        const RuleInsertResult result = processRulesWithOffset(appendList, current, true);
        current = result.inserted + result.normal;
    }

    // delete：精确删除匹配的规则文本
    const QStringList deleteList = readList(QStringLiteral("delete"));
    if (!deleteList.isEmpty()) {
        const QSet<QString> deleteSet(deleteList.begin(), deleteList.end());
        QStringList         kept;
        for (const QString &rule : current) {
            if (!deleteSet.contains(rule)) kept << rule;
        }
        current = kept;
    }

    config->setRules(QJsonArray::fromStringList(current));
}

std::optional<core::RawConfig> Factory::generateInMemory(Result *result)
{
    if (result) *result = Result();

    const QString profileId = m_profileConfig ? m_profileConfig->current() : QString();
    if (profileId.isEmpty()) {
        if (result) {
            result->error = QStringLiteral("尚未选择任何订阅，请先在「订阅」页添加并选中一个配置");
        }
        return std::nullopt;
    }

    if (result) result->profileId = profileId;

    bool          ok = false;
    const QString content = m_profileConfig->profileContent(profileId, &ok);
    if (!ok || content.trimmed().isEmpty()) {
        if (result) {
            result->error = QStringLiteral("订阅「%1」的内容为空或不存在")
                                .arg(m_profileConfig->item(profileId).name);
        }
        return std::nullopt;
    }

    QString                 parseError;
    const auto              parsed = core::RawConfig::fromYaml(content, &parseError);
    if (!parsed) {
        if (result) {
            result->error = QStringLiteral("订阅内容解析失败：%1").arg(parseError);
        }
        return std::nullopt;
    }

    core::RawConfig config = *parsed;

    // ---- 覆写 -------------------------------------------------------------
    QStringList overrideIds;
    if (m_overrideConfig) {
        overrideIds = m_overrideConfig->globalIds();
    }
    const ProfileItem profileItem = m_profileConfig->item(profileId);
    for (const QString &id : profileItem.override) {
        if (!overrideIds.contains(id)) overrideIds << id;
    }

    QStringList warnings;
    applyOverrides(&config, overrideIds, &warnings);

    // ---- 规则覆写 ---------------------------------------------------------
    applyRuleOverride(&config, profileId, &warnings);

    // ---- 合并受控内核配置 -------------------------------------------------
    if (m_controledConfig) {
        const bool controlDns   = m_appConfig ? m_appConfig->controlDns() : false;
        const bool controlSniff = m_appConfig ? m_appConfig->controlSniff() : true;

        QJsonObject controled = m_controledConfig->object();

        // 关闭 DNS 覆写时，连同 hosts 一起交给订阅自己决定，
        // 否则会出现「用户关了 DNS 覆写但 hosts 仍被注入」的不一致
        if (!controlDns) {
            controled.remove(QStringLiteral("dns"));
            controled.remove(QStringLiteral("hosts"));
        }
        if (!controlSniff) {
            controled.remove(QStringLiteral("sniffer"));
        }
        if (m_appConfig && !m_appConfig->useNameserverPolicy()) {
            QJsonObject dns = controled.value(QStringLiteral("dns")).toObject();
            dns.remove(QStringLiteral("nameserver-policy"));
            if (!dns.isEmpty()) controled.insert(QStringLiteral("dns"), dns);
        }

        // 关闭 DNS 覆写且最终配置里没有启用的 DNS 时，清空 dns-hijack：
        // 否则 TUN 会把 53 端口流量劫持过来，却没有解析器处理
        config.applyPatch(controled, /*isOverride=*/false);

        if (!controlDns && !config.dns().value(QStringLiteral("enable")).toBool(false)) {
            QJsonObject tun = config.tun();
            if (!tun.isEmpty()) {
                tun.insert(QStringLiteral("dns-hijack"), QJsonArray());
                config.setTun(tun);
            }
        }
    }

    // ---- 收尾清理 ---------------------------------------------------------
    QJsonObject root = config.toJson();

    // 空的 lan-allowed-ips 会导致局域网访问异常，直接删除
    const QJsonArray lanAllowed = root.value(QStringLiteral("lan-allowed-ips")).toArray();
    if (lanAllowed.isEmpty()) root.remove(QStringLiteral("lan-allowed-ips"));

    // 未启用外部控制器时不写入相关字段，避免内核去下载 WebUI
    if (root.value(QStringLiteral("external-controller")).toString().isEmpty()) {
        root.remove(QStringLiteral("external-controller"));
        root.remove(QStringLiteral("external-ui"));
        root.remove(QStringLiteral("external-ui-url"));
        root.remove(QStringLiteral("external-controller-cors"));
    }

    core::RawConfig finalConfig(root);
    m_runtimeConfig = finalConfig;

    // 日志等级若为 silent/error，仍按 info 记录到内核工作配置：
    // 启动阶段的问题排查依赖日志，否则用户看不到任何失败原因。
    const QString logLevel = finalConfig.logLevel() == core::LogLevel::Debug
                                 ? QStringLiteral("debug")
                                 : QStringLiteral("info");
    config.setLogLevel(core::parseLogLevel(logLevel).value_or(core::LogLevel::Info));

    if (result) {
        result->success  = true;
        result->warnings = warnings;
    }
    for (const QString &warning : warnings) {
        AppLogger::instance().warn(warning);
    }
    return config;
}

Factory::Result Factory::generate()
{
    Result                    result;
    const auto                config = generateInMemory(&result);
    if (!config) {
        // generateInMemory 已填充 error；这里保证 success 为 false
        result.success = false;
        return result;
    }

    // 写出内核工作配置
    QString error;
    if (!safefile::writeAtomic(paths::workConfigPath(), config->toYaml(), &error)) {
        result.success = false;
        result.error   = QStringLiteral("配置写入失败：%1").arg(error);
        return result;
    }

    AppLogger::instance().info(
        QStringLiteral("已生成运行配置：%1（订阅 %2）").arg(paths::workConfigPath(),
                                                          result.profileId));
    result.success = true;
    return result;
}

} // namespace clashqt
