#pragma once

// ---------------------------------------------------------------------------
//  WebSocket 传输（RFC 6455 客户端侧）
//
//  对应参照实现 transport/vmess/websocket 等传输的客户端职责。
//  提供：
//    * 握手（WebSocketClient::handshake）：
//        GET path HTTP/1.1 + Upgrade + Sec-WebSocket-Key，校验 101 与
//        Sec-WebSocket-Accept
//    * 帧读写（带掩码的客户端帧、服务端帧解析、ping/pong 自动应答、
//      分片消息聚合、关闭帧处理）
//    * WebSocketStream：把「socket + 帧协议」包装成 OutboundStream，
//      使中继层无需感知帧结构
//
//  说明：本实现只覆盖 VLESS 等协议在 WS 上承载二进制负载（opcode 0x2）
//  所需的子集，不实现子协议协商、permessage-deflate 压缩与分片发送。
// ---------------------------------------------------------------------------

#include "tunnel/OutboundStream.h"

#include <QByteArray>
#include <QString>

class QAbstractSocket;

namespace core {

namespace websocket {

// RFC 6455 帧操作码
constexpr uchar kOpContinuation = 0x0;
constexpr uchar kOpText         = 0x1;
constexpr uchar kOpBinary       = 0x2;
constexpr uchar kOpClose        = 0x8;
constexpr uchar kOpPing         = 0x9;
constexpr uchar kOpPong         = 0xA;

// 发起 WebSocket 升级握手。
//   socket     已连接（TCP 或 TLS）的套接字
//   path       请求路径（可含查询串），如 "/pq/jp1"
//   hostHeader Host 首部的值（CDN 场景通常为 servername 而非连接地址）
bool handshake(QAbstractSocket *socket, const QString &path, const QString &hostHeader,
               int timeoutMs, QString *error);

// 读取一帧业务数据（自动应答 ping、忽略 pong、聚合分片）。
// 成功时 opcode 为 0x1/0x2，payload 为帧负载。
bool readFrame(QAbstractSocket *socket, uchar *opcode, QByteArray *payload, int timeoutMs,
               QString *error);

// 发送一帧二进制数据（客户端帧自动加掩码）
bool sendBinary(QAbstractSocket *socket, const QByteArray &payload, QString *error);

// 发送关闭帧并尽快返回（不等对端确认）
void sendClose(QAbstractSocket *socket);

} // namespace websocket

// ---------------------------------------------------------------------------
//  WebSocket 形式的出站流
// ---------------------------------------------------------------------------

class WebSocketStream : public OutboundStream {
    Q_OBJECT

public:
    // 取得 socket 所有权（析构时关闭并删除）
    explicit WebSocketStream(QAbstractSocket *socket, QObject *parent = nullptr);
    ~WebSocketStream() override;

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

    // 底层 socket（用于在建立流之前完成阻塞式的首帧读取）
    QAbstractSocket *underlyingSocket() const { return m_socket; }

    // 注入调用方已经解码的业务数据（VLESS 首帧在拨号阶段同步读取后转入本流）
    void feedDecoded(const QByteArray &data)
    {
        if (data.isEmpty()) return;
        m_buffer.append(data);
        emit readyToRead();
    }

private:
    // 消费 socket 中已到达的字节：解析帧、应答 ping、聚合业务负载
    void pump();

    QAbstractSocket *m_socket = nullptr;
    QByteArray       m_buffer;   // 已解码、待中继读取的业务数据
    bool             m_closed = false;
};

} // namespace core
