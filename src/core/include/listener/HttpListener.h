#pragma once

// ---------------------------------------------------------------------------
//  HTTP 入站监听
//
//  对应参照实现 listener/http。支持两种形态：
//    * CONNECT host:port      建立隧道（HTTPS / 任意 TCP）
//    * 明文 HTTP 请求          以代理方式转发（绝对 URI 会改写为 origin-form）
//
//  Phase 1 说明：为简化连接生命周期，转发的明文请求统一按 Connection: close
//  处理（不使用 keep-alive 复用）。这对功能无影响，只是每条请求多一次
//  建连开销；keep-alive 复用属于后续阶段的优化项。
// ---------------------------------------------------------------------------

#include "listener/Listener.h"

namespace core {

class HttpListener : public Listener {
    Q_OBJECT

public:
    static constexpr const char *kType = "HTTP";

    explicit HttpListener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent = nullptr);

    // 供 MIXED 监听复用：直接处理一条已完成协议嗅探的连接
    void processConnection(QTcpSocket *clientSocket) { handleConnection(clientSocket); }

protected:
    void handleConnection(QTcpSocket *clientSocket) override;

private:
    // 解析请求头；失败返回 false 并填充 error
    struct ParsedRequest {
        QString     method;
        QString     target;
        QString     version;
        QList<QPair<QString, QString>> headers;
        bool        isConnect = false;
    };

    // 读取并拆分请求首部与已到达的请求体（两者必须分开，
    // 否则重建请求时会丢掉请求体或把请求体当成首部解析）
    bool readRequestHead(QTcpSocket *client, QByteArray *rawHead, QByteArray *body,
                         QString *error) const;
    bool parseRequest(const QByteArray &rawHead, ParsedRequest *request, QString *error) const;

    // 明文请求：把绝对 URI 改写为 origin-form 并剥离逐跳首部
    QByteArray rebuildRequest(const ParsedRequest &request, const QString &originForm) const;

    static QString headerValue(const ParsedRequest &request, const QString &name);
};

} // namespace core
