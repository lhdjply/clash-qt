#pragma once

// ---------------------------------------------------------------------------
//  SOCKS5 入站监听
//
//  对应参照实现 listener/socks。实现 RFC 1928 的 CONNECT 命令与 RFC 1929 的
//  用户名/密码认证；认证凭据来自主配置的 authentication 段。
//
//  UDP ASSOCIATE（CMD=0x03）与 BIND（CMD=0x02）在 Phase 1 明确返回
//  「不支持的命令」，而不是假装成功——静默失败会让客户端长时间挂起。
// ---------------------------------------------------------------------------

#include "listener/Listener.h"

#include <QStringList>

namespace core {

class Socks5Listener : public Listener {
    Q_OBJECT

public:
    static constexpr const char *kType = "SOCKS5";

    Socks5Listener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent = nullptr);

    // 设置认证凭据（"user:pass" 形式）；为空表示允许无认证
    void setCredentials(const QStringList &credentials);

    // 供 MIXED 监听复用：直接处理一条已完成协议嗅探的连接
    void processConnection(QTcpSocket *clientSocket) { handleConnection(clientSocket); }

protected:
    void handleConnection(QTcpSocket *clientSocket) override;

private:
    // 版本协商 + 认证；成功返回 true
    bool negotiate(QTcpSocket *client, QString *username, QString *error);

    // 读取请求并填充 metadata；返回 false 表示需要向客户端回错误码
    bool readRequest(QTcpSocket *client, Metadata *metadata, uchar *replyCode, QString *error);

    bool verifyCredentials(const QString &username, const QString &password) const;

    static bool writeAll(QTcpSocket *socket, const QByteArray &payload, int timeoutMs);

    QStringList m_credentials;
};

} // namespace core
