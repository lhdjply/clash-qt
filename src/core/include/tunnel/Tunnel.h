#pragma once

// ---------------------------------------------------------------------------
//  Tunnel —— 流量分发与中继
//
//  对应参照实现 tunnel/tunnel.go。核心流程：
//
//    入站连接 -> 构造 Metadata -> 规则匹配 -> 选出站适配器
//             -> 拨号 -> 双向中继 -> 流量统计 -> 连接结束
//
//  线程模型：
//    每条入站连接由独立工作线程处理（见 listener/），该线程运行一个
//    QEventLoop。handleStream() 在同一线程内同步完成「匹配 → 拨号 → 建立
//    中继」，中继由事件循环驱动，直至任一端关闭。因此 Tunnel 本身无需加锁；
//    跨线程共享的状态（快照、连接跟踪）分别通过 atomic shared_ptr 与
//    内部互斥量保护。
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"
#include "adapter/AdapterRegistry.h"
#include "component/dns/Resolver.h"
#include "rules/Rule.h"
#include "tunnel/ConnectionTracker.h"

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

#include <atomic>
#include <memory>
#include <vector>

class QTcpSocket;

namespace core {

// 配置快照：配置热重载时整体替换，正在进行的连接继续使用旧快照，
// 避免运行中配置被改写导致的竞态。
struct TunnelSnapshot {
    std::shared_ptr<AdapterRegistry> registry;
    std::shared_ptr<Resolver>        resolver;
    std::shared_ptr<GeoLookup>       geo;
    std::vector<RulePtr>             rules;
    OutboundMode                     mode = OutboundMode::Rule;
    // 出站拨号超时
    int dialTimeoutMs = 8000;
    // 是否启用域名嗅探（本阶段仅记录开关，嗅探器在后续阶段实现）
    bool snifferEnabled = false;
};

// 规则匹配结果
struct DispatchOutcome {
    bool        matched = false;
    QString     policyName;
    QString     ruleType;
    QString     rulePayload;
    AdapterPtr  adapter;
    // 出站链（由内到外），用于 /connections 展示
    QStringList chains;
    QString     error;
};

class Tunnel : public QObject {
    Q_OBJECT

public:
    explicit Tunnel(QObject *parent = nullptr);
    ~Tunnel() override;

    // 原子替换配置快照
    void setSnapshot(std::shared_ptr<TunnelSnapshot> snapshot);
    std::shared_ptr<TunnelSnapshot> snapshot() const;

    std::shared_ptr<ConnectionTracker> tracker() const { return m_tracker; }

    // 规则匹配：返回命中的策略名与适配器
    DispatchOutcome dispatch(const Metadata &metadata) const;

    // 处理一条已解析出目标的 TCP 流（由各入站监听器调用）。
    // initialPayload 是客户端已发送、需先转发出站的字节（如明文 HTTP 请求）。
    void handleStream(QTcpSocket *client,
                      const Metadata &metadata,
                      const QByteArray &initialPayload = QByteArray());

    // 累计流量（字节）。Hub 定时采样以计算瞬时速度。
    qint64 uploadedBytes() const { return m_uploaded.load(std::memory_order_relaxed); }
    qint64 downloadedBytes() const { return m_downloaded.load(std::memory_order_relaxed); }

    // TUN 数据面直接累加。TUN 的连接由协议栈自己拨号（FlowWorker），不经过
    // handleStream，字节数也就不会走 StreamRelay 的钩子；必须由数据面显式
    // 汇总到这里，否则界面上的速度恒为 0。
    // 只碰原子计数，可在任意线程调用（TUN 数据面在自己的线程里）。
    void addTraffic(qint64 upload, qint64 download);

    // 关闭指定出站链上的连接
    int closeConnectionsByChain(const QString &chain);

signals:
    // 内核运行日志，由 Hub 汇总输出
    void coreLog(const QString &level, const QString &message);

private:
    // 规则匹配；返回命中的规则指针（未命中返回 nullptr）
    RulePtr matchRule(const TunnelSnapshot &snapshot, const Metadata &metadata) const;

    // 组链展开：把「组 -> 成员」展开成完整出站链
    QStringList expandChains(const AdapterPtr &adapter, int depth = 0) const;

    std::shared_ptr<TunnelSnapshot>    m_snapshot;
    std::shared_ptr<ConnectionTracker> m_tracker;

    std::atomic<qint64> m_uploaded{0};
    std::atomic<qint64> m_downloaded{0};
};

} // namespace core
