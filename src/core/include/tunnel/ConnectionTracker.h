#pragma once

// ---------------------------------------------------------------------------
//  连接跟踪
//
//  对应参照实现 tunnel/statistic.go 与 hub/route/connections.go 的数据来源。
//  记录每条活动连接的元数据与流量，供 /connections 接口与界面展示。
//
//  线程安全：连接分散在不同工作线程中，类内部以互斥量保护状态，
//  快照接口返回拷贝而不是引用，避免调用方持有悬空引用。
// ---------------------------------------------------------------------------

#include "rules/Rule.h"

#include <QDateTime>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QMutex>
#include <QString>
#include <QStringList>

#include <memory>

namespace core {

struct ConnectionInfo {
    QString id;
    QString startTime;      // ISO8601
    QString rule;           // 命中的规则类型
    QString rulePayload;    // 命中的规则匹配值
    QStringList chains;     // 出站链（组名 -> ... -> 节点名）
    QStringList providerChains;

    Metadata metadata;

    qint64 upload   = 0;
    qint64 download = 0;
    qint64 uploadSpeed   = 0;
    qint64 downloadSpeed = 0;

    bool active = true;

    // 上一次采样时间与采样时的累计值，用于计算瞬时速度
    qint64 lastSampleMs      = 0;
    qint64 lastUpload        = 0;
    qint64 lastDownload      = 0;
};

class ConnectionTracker {
public:
    ConnectionTracker() = default;

    QString createConnection(const Metadata &metadata);
    void    closeConnection(const QString &id);
    void    closeAll();

    // 规则匹配完成后回填命中信息（规则类型、匹配值与出站链）
    void updateDispatch(const QString &id, const QString &ruleType, const QString &rulePayload,
                        const QStringList &chains);

    // 累加流量并更新瞬时速度
    void addUpload(const QString &id, qint64 bytes);
    void addDownload(const QString &id, qint64 bytes);

    // 快照（按开始时间倒序）
    QList<ConnectionInfo> snapshot() const;
    int                   activeCount() const;

    // 累计流量（进程生命周期内）
    qint64 totalUpload() const;
    qint64 totalDownload() const;

    // 关闭匹配指定出站链的连接，返回关闭数量（用于 /connections?group= 语义）
    int closeByChain(const QString &chain);

    // 重启内核等场景下刷新速度基准，避免重启后出现巨大瞬时值
    void resetSpeedBaseline();

private:
    void updateSpeedsLocked(ConnectionInfo &info, qint64 nowMs);

    mutable QMutex                        m_mutex;
    QHash<QString, ConnectionInfo>        m_connections;
    qint64                                m_totalUpload = 0;
    qint64                                m_totalDownload = 0;
    quint64                               m_sequence = 0;
};

// ConnectionInfo -> /connections 接口所需的 JSON 结构
QJsonObject connectionToJson(const ConnectionInfo &info);

} // namespace core
