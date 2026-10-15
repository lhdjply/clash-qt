#pragma once

// ---------------------------------------------------------------------------
//  出站节点与代理组的解析结果
//
//  对应参照实现 adapter/parser.go（ParseProxy / ParseProxyGroup）。
//
//  解析只做「通用字段提取 + 类型判定」：协议私有参数（cipher / uuid / ws-opts
//  / reality-opts / plugin-opts …）不在这里逐项建模，而是保留在 extra 中，
//  由具体协议适配器在建立连接时读取。这样新增协议无需改动解析层，
//  也与参照实现通过 mapstructure 把剩余字段交给各协议 opts 的做法一致。
// ---------------------------------------------------------------------------

#include "constant/Constants.h"

#include <QJsonObject>
#include <QString>
#include <QStringList>

#include <optional>

namespace core {

struct ProxyConfig {
    QString     name;
    AdapterType type = AdapterType::Unknown;
    // YAML 中给出的原始类型文本，便于报错时回显用户配置
    QString rawType;

    QString server;
    int     port = 0;

    bool udp = false;
    bool tfo = false;
    bool xudp = false;
    bool uot = false;
    bool mptcp = false;
    bool smux = false;

    QString interfaceName;
    int     routingMark = 0;
    QString providerName;
    QString dialerProxy;

    // 协议私有参数与未识别字段的原样保留
    QJsonObject extra;

    bool isValid() const { return type != AdapterType::Unknown && !name.isEmpty(); }

    // 从单个 proxy 映射解析；类型无法识别时返回 nullopt
    static std::optional<ProxyConfig> parse(const QJsonObject &object);

    // 反向序列化（用于写回配置 / 内置节点编辑）
    QJsonObject toJson() const;
};

struct ProxyGroupConfig {
    QString     name;
    GroupType   type = GroupType::Selector;
    QString     rawType;
    QStringList proxies; // 成员名列表（可以是节点名、子组名或 provider 内节点）
    QStringList use;     // 引用 proxy-provider 的名称列表

    QString url;
    int     interval  = 0;
    int     tolerance = 0;
    bool    lazy      = false;
    bool    hidden    = false;
    QString icon;

    QJsonObject extra;

    bool isValid() const { return type != GroupType::Unknown && !name.isEmpty(); }

    static std::optional<ProxyGroupConfig> parse(const QJsonObject &object);
    QJsonObject                            toJson() const;
};

} // namespace core
