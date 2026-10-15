#pragma once

// ---------------------------------------------------------------------------
//  MIXED 入站监听
//
//  对应参照实现 listener/mixed：在同一端口上同时接受 HTTP 与 SOCKS5。
//  识别方式为首字节嗅探——SOCKS5 的版本字节固定为 0x05，其余按 HTTP 处理。
//  这是 Clash 生态的既定约定，也是各客户端与面板的默认连接方式。
// ---------------------------------------------------------------------------

#include "listener/HttpListener.h"
#include "listener/Socks5Listener.h"

#include <memory>

namespace core {

class MixedListener : public Listener {
    Q_OBJECT

public:
    static constexpr const char *kType = "MIXED";

    MixedListener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent = nullptr);

    // 与 mixed 端口共用的认证凭据（沿用主配置 authentication 段）
    void setCredentials(const QStringList &credentials);

protected:
    void handleConnection(QTcpSocket *clientSocket) override;

private:
    // 协议处理器：不自行监听，仅复用其握手逻辑
    std::shared_ptr<HttpListener>   m_httpHandler;
    std::shared_ptr<Socks5Listener> m_socksHandler;
};

} // namespace core
