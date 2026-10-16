#pragma once

// ---------------------------------------------------------------------------
//  Hysteria2 出站适配器
//
//  对应参照实现 adapter/outbound/hysteria2.go（底层为 sing-quic/hysteria2）。
//  协议与传输细节见 common/Hysteria2Client.h。
//
//  支持的配置字段：
//    server / port / ports（端口跳跃区间，建连时随机取一个）
//    password（或 auth）
//    sni（TLS SNI，缺省用 server）
//    skip-cert-verify / fingerprint（证书 SHA-256 指纹固定）
//    up / down（带宽声明；本实现固定发送 CC-RX=0，交由服务端默认拥塞控制）
//
//  明确不支持（给出清晰失败原因，不静默降级）：
//    obfs（Salamander 混淆）、realm、UDP 中继（OpenSSL 3.2 无 QUIC DATAGRAM）
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

namespace core {

class Hysteria2Adapter : public Adapter {
public:
    explicit Hysteria2Adapter(ProxyConfig config);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return AdapterType::Hysteria2; }
    // 仅 TCP：QUIC DATAGRAM 在 OpenSSL 3.2 中尚未实现，无法承载 UDP 中继
    QStringList supportNetworks() const override { return {QStringLiteral("TCP")}; }

    DialResult dial(const DialContext &context, int timeoutMs) override;

    QJsonObject status() const override;

private:
    ProxyConfig m_config;
    QString     m_password;
    QString     m_serverName;   // TLS SNI
    QString     m_portRange;    // ports 字段（"60000-63000"）
    QByteArray  m_fingerprint;  // 证书 SHA-256 指纹（32 字节）
    bool        m_skipVerify = false;
    QString     m_unsupported;  // 配置中使用了尚未支持的选项时的原因
};

} // namespace core
