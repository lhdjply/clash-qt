#pragma once

// ---------------------------------------------------------------------------
//  VLESS 出站适配器
//
//  对应参照实现 adapter/outbound/vless.go 与 transport/vless。
//
//  协议（VLESS v0，官方文档）：
//    请求  VER(0x00) | UUID(16) | ADDONS_LEN(1) | ADDONS | CMD(1) | PORT(2)
//          | ATYP(1) | ADDR | 负载
//    应答  VER(1) | ADDONS_LEN(1) | ADDONS | 负载
//  ADDONS 为 protobuf 编码的扩展（flow 等）；非 vision 流为空。
//
//  传输层组合（本实现覆盖的范围）：
//    network 缺省        -> 裸 TCP（可叠加 TLS）
//    network: ws         -> WebSocket 承载，整个 VLESS 请求作为一帧二进制消息
//    tls: true           -> 手工 TLS 1.3（见 common/Tls13Client.h）：SNI 与证书
//          校验名取 servername（缺省用 server），skip-cert-verify 可跳过校验
//    flow: xtls-rprx-vision + reality-opts -> REALITY 握手（同一套手工 TLS 1.3）
//          + Vision 填充流控（见 Tls13Client.h / VisionStream.h）
//
//  说明：TLS 一律由本项目的 TLS 1.3 客户端实现，不使用 QSslSocket ——
//  Qt 无法把「TCP 拨号地址」与「SNI」分开，也无法定制 REALITY 的 ClientHello。
//
//  明确不支持（有清晰的失败原因，而不是静默降级）：
//    非 vision 的 flow、grpc / httpupgrade 等其它传输
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

namespace core {

class VlessAdapter : public Adapter {
public:
    explicit VlessAdapter(ProxyConfig config);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return AdapterType::Vless; }
    QStringList supportNetworks() const override { return {QStringLiteral("TCP")}; }

    DialResult dial(const DialContext &context, int timeoutMs) override;

    QJsonObject status() const override;

private:
    // 明文 TCP 拨号（ws 无 TLS / 裸 tcp 无 TLS）；TLS 形态走 RealityTlsStream
    std::unique_ptr<QTcpSocket> connectPlainTcp(int timeoutMs, QString *error);

    ProxyConfig m_config;
    QString     m_uuid;        // 用户 ID
    QString     m_serverName;  // TLS SNI / WS Host（servername 字段）
    QString     m_network;     // "" / "ws"
    QString     m_wsPath;      // ws-opts.path
    QString     m_wsHost;      // ws-opts.headers.Host（缺省为 servername）
    bool        m_tls = false;
};

} // namespace core
