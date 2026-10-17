#pragma once

// ---------------------------------------------------------------------------
//  DNS 服务端（应答侧）
//
//  Resolver 负责「向外查」，DnsServer 负责「对内答」——把客户端发来的 DNS
//  查询变成应答。TUN 模式下应用发出的 53 端口报文会被劫持到这里（直接调
//  handleQuery，不经过 socket）；若配置了 dns.listen，也会同时开一个 UDP/TCP
//  监听，方便非 TUN 场景与排查。
//
//  应答策略（对应 enhanced-mode）：
//    * fake-ip：命中 fake-ip-filter 之外的域名直接分配一个假 IP 返回，
//      不发网络请求；真实解析推迟到「真有连接发往这个假 IP」时，
//      由 TUN 中继用域名去匹配规则并拨号（域名规则因此照常生效）。
//    * normal：直接走 Resolver 的真实解析（hosts / nameserver / DoH）。
//
//  AAAA 的处理：本实现的假 IP 池只有 IPv4（fake-ip-range 默认
//  198.18.0.1/16），且 dns.ipv6 默认关闭，因此 AAAA 查询返回
//  「无应答记录（NOERROR, 0 answers）」—— 客户端据此回落到 A 查询，
//  这是 fake-ip 模式下的常规做法，也避免应用绕过假 IP 直连真实 IPv6。
//
//  暂不支持：PTR / MX 等非 A/AAAA 类型（返回空应答），以及假 IP 的 PTR 反查。
// ---------------------------------------------------------------------------

#include "component/dns/Resolver.h"

#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include <memory>

class QTcpServer;
class QUdpSocket;

namespace core {

class DnsServer : public QObject {
    Q_OBJECT

public:
    explicit DnsServer(std::shared_ptr<Resolver> resolver, QObject *parent = nullptr);
    ~DnsServer() override;

    // 依据配置树里的 dns 段应用设置（enable / listen / enhanced-mode / ipv6 /
    // fake-ip-*）。配置变化后调用会重开监听。
    void applyDnsConfig(const QJsonObject &dnsConfig);

    bool isEnabled() const { return m_enabled; }
    // 是否有 socket 在监听（dns.listen 为空时不开监听，仅服务 TUN 劫持）
    bool isListening() const { return m_udp != nullptr; }
    QString listenSummary() const;

    // 打开 dns.listen 指定的监听；失败只记日志、不抛出（TUN 劫持不依赖它）
    bool startListening(QString *error = nullptr);
    void stopListening();

    // dns-hijack 是否要求劫持「任意地址的 53 端口」（TUN 用）
    bool hijackAny53() const { return m_hijackAny; }

    // 对一条原始 DNS 查询报文给出应答。返回空表示丢弃（非法报文 / 非查询 /
    // 未启用）。TUN 的 UDP 处理会直接调它。
    QByteArray handleQuery(const QByteArray &payload, const QHostAddress &client);

signals:
    void logMessage(const QString &level, const QString &message);

private slots:
    void onUdpReadyRead();

private:
    // 单条问题 → 应答记录
    QList<dns::Answer> answerFor(const dns::Question &question, const QHostAddress &client);

    std::shared_ptr<Resolver> m_resolver;

    QUdpSocket  *m_udp = nullptr;
    QTcpServer  *m_tcp = nullptr;

    bool         m_enabled     = false;
    bool         m_fakeIpMode  = false;
    bool         m_ipv6        = false;
    bool         m_hijackAny   = false;
    int          m_timeoutMs   = 5000;
    QHostAddress m_listenAddr   = QHostAddress(QHostAddress::LocalHost);
    quint16      m_listenPort   = 0; // 0 表示不监听
};

} // namespace core
