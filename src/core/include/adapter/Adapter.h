#pragma once

// ---------------------------------------------------------------------------
//  出站适配器接口
//
//  对应参照实现 adapter/adapter.go 的 Adapter 接口。所有出站节点、代理组、
//  DIRECT / REJECT 都实现该接口，Tunnel 只依赖此抽象，不感知具体协议。
//
//  线程模型：dialTcp / dialUdp 会在工作线程中被调用，该线程已运行 Qt 事件循环。
//  实现内部使用「异步连接 + 局部事件循环等待」的方式完成握手，返回时套接字
//  已连接且仍归属同一线程，可直接交给 Relay 做非阻塞转发。
//
//  ---------------------------------------------------------------------------
//  关于协议覆盖范围（重要）
//  ---------------------------------------------------------------------------
//  本阶段实现 DIRECT / REJECT / HTTP / SOCKS5 四类出站，以及代理组的
//  Selector 语义。其余 40 余种协议（VMess / VLESS / Trojan / Shadowsocks /
//  Hysteria / TUIC / WireGuard / AnyTLS / …）的编解码与握手属于参照实现
//  transport 包（Go 侧约 55,000 行）的对应工作，需要逐协议按序实现；
//  这些协议在 AdapterRegistry 中会被登记为「暂不支持」，选择它们时返回
//  明确错误，而不会静默走直连（静默降级会造成流量泄漏）。
// ---------------------------------------------------------------------------

#include "config/ProxyConfig.h"
#include "constant/Constants.h"
#include "rules/Rule.h"
#include "tunnel/OutboundStream.h"

#include <QHostAddress>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include <QUdpSocket>

#include <atomic>
#include <memory>
#include <optional>

#include <QMutex>
#include <QMutexLocker>

namespace core {

class Adapter;
class UdpChannel;

// 一次拨号请求的完整上下文
struct DialContext {
    // 目标：优先使用 host（域名），否则使用 dstIp
    QString host;
    QString dstIp;
    int     dstPort = 0;
    QString network; // "tcp" / "udp"
    Metadata metadata;
};

// 拨号结果：成功时 stream 非空且可用，error 为空。
// stream 是抽象出站流：DIRECT/HTTP/SOCKS5 为裸 TCP 流，
// WebSocket 类传输则为帧化解码后的流，中继层无需感知差异。
struct DialResult {
    std::shared_ptr<OutboundStream> stream;
    std::shared_ptr<QUdpSocket>     udp;
    // UDP 数据报通道（DIRECT 的已连接 socket / VLESS 的长度前缀分帧流）。
    // network == "udp" 时以它为准；udp 字段是它的 QUdpSocket 特例，保留兼容。
    std::shared_ptr<UdpChannel>     udpChannel;
    QString                         error;
    // REJECT-DROP：不建立任何上行连接，但保持客户端连接打开并丢弃数据
    // （对应参照实现的 RejectDrop 语义，直接 reset 会让对端立刻感知）
    bool                        drop = false;
    // 握手过程中已经读到的、属于隧道有效载荷的字节。
    // HTTP CONNECT / SOCKS5 的响应之后可能紧跟目标数据，Qt 没有「回灌」接口
    // （QIODevice::unreadData 在 Qt 6.8 中不存在），因此把多余字节带出，
    // 由 Tunnel 先写入客户端，避免丢包。
    QByteArray                  extraData;

    bool ok() const { return stream != nullptr || udp != nullptr || udpChannel != nullptr; }
};

class Adapter {
public:
    virtual ~Adapter() = default;

    virtual QString     name() const = 0;
    virtual AdapterType type() const = 0;

    // 支持的传输层协议，取值 "TCP" / "UDP"，用于 /proxies 接口与能力判断
    virtual QStringList supportNetworks() const { return {QStringLiteral("TCP")}; }
    virtual bool        supportUdp() const { return supportNetworks().contains(QStringLiteral("UDP")); }
    virtual bool        supportTcp() const { return supportNetworks().contains(QStringLiteral("TCP")); }

    // 是否需要由本机先把目标域名解析成 IP 再连接。
    // DIRECT 必须为 true（否则无法使用内核自己的 hosts / DNS 配置，
    // 也无法应用 nameserver-policy）；代理类出站通常为 false，
    // 让代理服务器远端解析以避免本地 DNS 泄漏。
    virtual bool needsLocalResolution() const { return false; }

    // 拨号。timeoutMs 为总体超时；实现必须保证在超时前返回。
    virtual DialResult dial(const DialContext &context, int timeoutMs) = 0;

    // 健康状态与延迟历史，用于 /proxies 接口。
    // 单节点测速跑在独立线程里（见 Hub::testProxyDelayAsync），会与界面读取
    // 并发：alive 用原子量，history 加锁，否则 QList 在两个线程里读写会崩。
    virtual bool alive() const { return m_alive.load(std::memory_order_relaxed); }
    virtual void setAlive(bool alive) { m_alive.store(alive, std::memory_order_relaxed); }

    struct DelayRecord {
        QString time;
        int     delay = 0;
    };
    virtual QList<DelayRecord> history() const;
    virtual void               recordDelay(int delayMs);
    virtual void               clearHistory()
    {
        QMutexLocker locker(&m_historyMutex);
        m_history.clear();
        m_alive.store(false, std::memory_order_relaxed);
    }

    // /proxies 接口中该节点的 JSON 表示
    virtual QJsonObject status() const;

    // 关闭所有由此适配器建立的资源（连接池、监听端口等）
    virtual void closeAll() {}

protected:
    std::atomic<bool>        m_alive{false};
    QList<DelayRecord>       m_history;
    mutable QMutex           m_historyMutex;
};

using AdapterPtr = std::shared_ptr<Adapter>;

// ---------------------------------------------------------------------------
//  代理组
// ---------------------------------------------------------------------------

class ProxyGroup : public Adapter {
public:
    ProxyGroup(ProxyGroupConfig config, std::vector<AdapterPtr> members);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return AdapterType::Compatible; }
    GroupType   groupType() const { return m_config.type; }

    QStringList supportNetworks() const override;

    DialResult dial(const DialContext &context, int timeoutMs) override;

    // 当前选中的成员名；没有可用成员时返回空。
    // 选择一旦确定（配置默认值或用户点选）就不会再自行改变：
    // 本类不按延迟重选，也不在拨号失败时回退到其他成员。
    QString now() const;
    // 用户是否在界面上显式选择过成员。只有显式选择才需要被持久化：
    // 构造时按配置取的首个成员只是默认值，不该被当成「用户的选择」写回配置。
    bool    hasUserSelection() const { return m_userSelected; }
    // 用户显式选定的成员名；未显式选择时返回空
    QString userSelectedMember() const { return m_userSelected ? m_fixed : QString(); }
    // 手动选择成员（Selector 语义）；成员不存在时返回 false
    bool select(const QString &memberName);

    const ProxyGroupConfig   &config() const { return m_config; }
    const std::vector<AdapterPtr> &members() const { return m_members; }

    AdapterPtr current() const;

    QJsonObject status() const override;

private:
    // 配置里的首个可用成员。仅在从未做出任何选择、或原选择已随订阅消失时
    // 作为兜底，不会因为测速结果或拨号失败而被改写。
    AdapterPtr defaultMember() const;

    ProxyGroupConfig         m_config;
    std::vector<AdapterPtr>  m_members;
    QString                  m_fixed; // 当前选中的成员名
    // 该选择是否来自用户的显式操作（界面点选）。用于决定是否写回配置，
    // 与选择本身是否会被自动改写无关——自动改写逻辑已全部移除。
    bool                     m_userSelected = false;
};

} // namespace core
