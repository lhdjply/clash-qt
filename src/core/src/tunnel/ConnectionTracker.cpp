#include "tunnel/ConnectionTracker.h"

#include <QMutexLocker>

#include <algorithm>

namespace core {

namespace {

QString makeConnectionId(quint64 sequence)
{
    // 结构参考参照实现的连接 id：时间戳 + 递增序号，保证唯一且可按时间排序
    const qint64 stamp = QDateTime::currentMSecsSinceEpoch();
    return QStringLiteral("%1-%2").arg(stamp).arg(sequence, 8, 36, QLatin1Char('0'));
}

} // namespace

QString ConnectionTracker::createConnection(const Metadata &metadata)
{
    QMutexLocker locker(&m_mutex);

    ConnectionInfo info;
    info.id        = makeConnectionId(++m_sequence);
    info.startTime = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    info.metadata  = metadata;
    info.lastSampleMs = QDateTime::currentMSecsSinceEpoch();

    m_connections.insert(info.id, info);
    return info.id;
}

void ConnectionTracker::closeConnection(const QString &id)
{
    QMutexLocker locker(&m_mutex);
    const auto it = m_connections.find(id);
    if (it == m_connections.end()) return;

    m_totalUpload += it.value().upload;
    m_totalDownload += it.value().download;
    m_connections.erase(it);
}

void ConnectionTracker::updateDispatch(const QString &id, const QString &ruleType,
                                      const QString &rulePayload, const QStringList &chains)
{
    QMutexLocker locker(&m_mutex);
    const auto it = m_connections.find(id);
    if (it == m_connections.end()) return;

    it.value().rule        = ruleType;
    it.value().rulePayload = rulePayload;
    it.value().chains      = chains;
}

void ConnectionTracker::closeAll()
{
    QMutexLocker locker(&m_mutex);
    for (auto it = m_connections.constBegin(); it != m_connections.constEnd(); ++it) {
        m_totalUpload += it.value().upload;
        m_totalDownload += it.value().download;
    }
    m_connections.clear();
}

void ConnectionTracker::updateSpeedsLocked(ConnectionInfo &info, qint64 nowMs)
{
    const qint64 elapsed = nowMs - info.lastSampleMs;
    if (elapsed < 500) return; // 采样窗口太短会让速度剧烈抖动

    const double seconds = double(elapsed) / 1000.0;
    info.uploadSpeed   = qint64(std::max<qint64>(0, info.upload - info.lastUpload) / seconds);
    info.downloadSpeed = qint64(std::max<qint64>(0, info.download - info.lastDownload) / seconds);

    info.lastSampleMs = nowMs;
    info.lastUpload   = info.upload;
    info.lastDownload = info.download;
}

void ConnectionTracker::addUpload(const QString &id, qint64 bytes)
{
    if (bytes <= 0) return;
    QMutexLocker locker(&m_mutex);
    const auto it = m_connections.find(id);
    if (it == m_connections.end()) return;

    it.value().upload += bytes;
    updateSpeedsLocked(it.value(), QDateTime::currentMSecsSinceEpoch());
}

void ConnectionTracker::addDownload(const QString &id, qint64 bytes)
{
    if (bytes <= 0) return;
    QMutexLocker locker(&m_mutex);
    const auto it = m_connections.find(id);
    if (it == m_connections.end()) return;

    it.value().download += bytes;
    updateSpeedsLocked(it.value(), QDateTime::currentMSecsSinceEpoch());
}

QList<ConnectionInfo> ConnectionTracker::snapshot() const
{
    QMutexLocker locker(&m_mutex);

    QList<ConnectionInfo> result;
    result.reserve(m_connections.size());
    for (const ConnectionInfo &info : m_connections) {
        result.append(info);
    }

    // 最新建立的连接排在最前，与界面默认排序一致
    std::sort(result.begin(), result.end(),
              [](const ConnectionInfo &a, const ConnectionInfo &b) { return a.id > b.id; });
    return result;
}

int ConnectionTracker::activeCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_connections.size();
}

qint64 ConnectionTracker::totalUpload() const
{
    QMutexLocker locker(&m_mutex);
    return m_totalUpload;
}

qint64 ConnectionTracker::totalDownload() const
{
    QMutexLocker locker(&m_mutex);
    return m_totalDownload;
}

int ConnectionTracker::closeByChain(const QString &chain)
{
    QMutexLocker locker(&m_mutex);

    int closed = 0;
    for (auto it = m_connections.begin(); it != m_connections.end();) {
        if (!it.value().chains.contains(chain)) {
            ++it;
            continue;
        }
        m_totalUpload += it.value().upload;
        m_totalDownload += it.value().download;
        it = m_connections.erase(it);
        ++closed;
    }
    return closed;
}

void ConnectionTracker::resetSpeedBaseline()
{
    QMutexLocker locker(&m_mutex);
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        it.value().lastSampleMs = now;
        it.value().lastUpload   = it.value().upload;
        it.value().lastDownload = it.value().download;
        it.value().uploadSpeed   = 0;
        it.value().downloadSpeed = 0;
    }
}

QJsonObject connectionToJson(const ConnectionInfo &info)
{
    QJsonObject metadata;
    metadata.insert(QStringLiteral("network"), info.metadata.network);
    metadata.insert(QStringLiteral("type"),
                    info.metadata.inboundType.isEmpty() ? QStringLiteral("HTTP")
                                                        : info.metadata.inboundType);
    metadata.insert(QStringLiteral("sourceIP"), info.metadata.srcIp);
    metadata.insert(QStringLiteral("sourceGeoIP"), QJsonArray());
    metadata.insert(QStringLiteral("sourceIPASN"), QString());
    metadata.insert(QStringLiteral("destinationIP"), info.metadata.dstIp);
    metadata.insert(QStringLiteral("destinationGeoIP"), QJsonArray());
    metadata.insert(QStringLiteral("destinationIPASN"), QString());
    metadata.insert(QStringLiteral("sourcePort"), QString::number(info.metadata.srcPort));
    metadata.insert(QStringLiteral("destinationPort"), QString::number(info.metadata.dstPort));
    metadata.insert(QStringLiteral("inboundIP"), QString());
    metadata.insert(QStringLiteral("inboundPort"), QString::number(info.metadata.inboundPort));
    metadata.insert(QStringLiteral("inboundName"), info.metadata.inboundName);
    metadata.insert(QStringLiteral("inboundUser"), info.metadata.inboundUser);
    metadata.insert(QStringLiteral("host"), info.metadata.host);
    metadata.insert(QStringLiteral("dnsMode"), QStringLiteral("normal"));
    metadata.insert(QStringLiteral("uid"), info.metadata.uid);
    metadata.insert(QStringLiteral("process"), info.metadata.processName);
    metadata.insert(QStringLiteral("processPath"), info.metadata.processPath);
    metadata.insert(QStringLiteral("specialProxy"), QString());
    metadata.insert(QStringLiteral("specialRules"), QString());
    metadata.insert(QStringLiteral("remoteDestination"), QString());
    metadata.insert(QStringLiteral("dscp"), info.metadata.dscp);
    metadata.insert(QStringLiteral("sniffHost"),
                    info.metadata.hostFromSniffing ? info.metadata.host : QString());

    QJsonObject object;
    object.insert(QStringLiteral("id"), info.id);
    object.insert(QStringLiteral("isActive"), info.active);
    object.insert(QStringLiteral("metadata"), metadata);
    object.insert(QStringLiteral("uploadSpeed"), info.uploadSpeed);
    object.insert(QStringLiteral("downloadSpeed"), info.downloadSpeed);
    object.insert(QStringLiteral("upload"), info.upload);
    object.insert(QStringLiteral("download"), info.download);
    object.insert(QStringLiteral("start"), info.startTime);
    object.insert(QStringLiteral("chains"), QJsonArray::fromStringList(info.chains));
    object.insert(QStringLiteral("providerChains"),
                  QJsonArray::fromStringList(info.providerChains));
    object.insert(QStringLiteral("rule"), info.rule);
    object.insert(QStringLiteral("rulePayload"), info.rulePayload);
    return object;
}

} // namespace core
