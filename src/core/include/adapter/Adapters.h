#pragma once

// ---------------------------------------------------------------------------
//  具体出站适配器实现
// ---------------------------------------------------------------------------

#include "adapter/Adapter.h"

namespace core {

// ---------------------------------------------------------------------------
//  DIRECT —— 直连目标
// ---------------------------------------------------------------------------

class DirectAdapter : public Adapter {
public:
    DirectAdapter();
    explicit DirectAdapter(QString name);

    QString     name() const override { return m_name; }
    AdapterType type() const override { return AdapterType::Direct; }
    QStringList supportNetworks() const override
    {
        return {QStringLiteral("TCP"), QStringLiteral("UDP")};
    }

    // DIRECT 直连需要真实 IP：必须走内核解析器，才能应用 hosts、
    // nameserver-policy 与 fallback 等 DNS 配置（见 Tunnel 的解析流程）。
    bool needsLocalResolution() const override { return true; }

    DialResult dial(const DialContext &context, int timeoutMs) override;

    // 绑定出口网卡（对应 interface-name）
    void setInterface(const QString &interfaceName) { m_interface = interfaceName; }

private:
    QString m_name = QStringLiteral("DIRECT");
    QString m_interface;
};

// ---------------------------------------------------------------------------
//  REJECT / REJECT-DROP
// ---------------------------------------------------------------------------

class RejectAdapter : public Adapter {
public:
    explicit RejectAdapter(QString name = QStringLiteral("REJECT"), bool drop = false)
        : m_name(std::move(name)), m_drop(drop)
    {
    }

    QString     name() const override { return m_name; }
    AdapterType type() const override
    {
        return m_drop ? AdapterType::RejectDrop : AdapterType::Reject;
    }

    DialResult dial(const DialContext &context, int timeoutMs) override;

private:
    QString m_name;
    bool    m_drop = false;
};

// ---------------------------------------------------------------------------
//  HTTP / HTTPS 代理出站（CONNECT 隧道）
// ---------------------------------------------------------------------------

class HttpProxyAdapter : public Adapter {
public:
    HttpProxyAdapter(ProxyConfig config);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return AdapterType::Http; }
    QStringList supportNetworks() const override { return {QStringLiteral("TCP")}; }

    DialResult dial(const DialContext &context, int timeoutMs) override;

    QJsonObject status() const override;

private:
    // 与代理服务器本身建立 TCP 连接，返回已连接的套接字
    std::unique_ptr<QTcpSocket> connectToServer(int timeoutMs, QString *error);

    ProxyConfig m_config;
    QString     m_username;
    QString     m_password;
    bool        m_tls = false;
};

// ---------------------------------------------------------------------------
//  SOCKS5 代理出站
// ---------------------------------------------------------------------------

class Socks5ProxyAdapter : public Adapter {
public:
    Socks5ProxyAdapter(ProxyConfig config);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return AdapterType::Socks5; }
    QStringList supportNetworks() const override
    {
        return {QStringLiteral("TCP"), QStringLiteral("UDP")};
    }

    DialResult dial(const DialContext &context, int timeoutMs) override;

    QJsonObject status() const override;

private:
    std::unique_ptr<QTcpSocket> connectToServer(int timeoutMs, QString *error);
    // 完成 SOCKS5 版本协商与可选认证，失败返回 false
    bool negotiate(QTcpSocket *socket, int timeoutMs, QString *error);
    // 发送 CONNECT 请求，成功返回 true
    bool requestConnect(QTcpSocket *socket, const DialContext &context, int timeoutMs,
                        QString *error);

    ProxyConfig m_config;
    QString     m_username;
    QString     m_password;
    // SOCKS5 应答之后可能紧跟目标数据，握手阶段先暂存，再由 dial 交给 Tunnel
    QByteArray  m_pendingData;
};

// ---------------------------------------------------------------------------
//  未实现协议的占位适配器
//
//  存在的意义：配置中出现的协议若尚未实现，必须显式失败而不是退化为直连，
//  否则用户的流量会在无感知的情况下绕过代理，这是严重的安全问题。
// ---------------------------------------------------------------------------

class UnsupportedAdapter : public Adapter {
public:
    UnsupportedAdapter(ProxyConfig config, QString reason);

    QString     name() const override { return m_config.name; }
    AdapterType type() const override { return m_config.type; }

    DialResult  dial(const DialContext &context, int timeoutMs) override;
    QJsonObject status() const override;

private:
    ProxyConfig m_config;
    QString     m_reason;
};

} // namespace core
