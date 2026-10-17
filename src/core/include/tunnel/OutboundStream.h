#pragma once

// ---------------------------------------------------------------------------
//  出站流抽象
//
//  为什么需要这一层：
//  DIRECT / HTTP / SOCKS5 的出站是「裸字节流」，QTcpSocket 的
//  readyRead / read / write 语义可以直接用于中继；但 WebSocket 传输的数据
//  有帧结构（RFC 6455），直接把 socket 暴露给中继会把帧头当业务数据转发。
//
//  因此把「可读、可写、对端关闭」这组最小语义抽象出来，由两种实现承载：
//    TcpStream       —— 包装 QTcpSocket（含 QSslSocket），语义一一对应
//    WebSocketStream —— 包装 socket + WS 分帧，内部完成帧解析、ping/pong
//
//  中继（Tunnel 的 StreamRelay）只依赖本抽象，后续新增 grpc / httpupgrade
//  等传输时只需再提供一个实现。
// ---------------------------------------------------------------------------

#include <QByteArray>
#include <QObject>
#include <QString>

class QAbstractSocket;

namespace core {

class OutboundStream : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;
    ~OutboundStream() override = default;

    // 取出当前已到达的全部业务数据（不等待）
    virtual QByteArray readAll() = 0;

    // 写入业务数据；返回写入字节数，<0 表示失败
    virtual qint64 write(const QByteArray &data) = 0;

    // 关闭出站方向并释放底层连接
    virtual void closeStream() = 0;

    virtual QString errorText() const = 0;

    // 底层 socket 是否仍然连接
    virtual bool isOpen() const = 0;

    // VLESS 应答头（VER + ADDONS_LEN + ADDONS）延迟到首次读取时再消费。
    // 服务端只有在首次写出业务数据时才会发应答头，拨号阶段同步等它会死锁；
    // 各具体流实现按需覆盖（Vision / WS / 裸 TLS）。
    virtual void setLazyVlessResponseHeader(bool enabled) { Q_UNUSED(enabled); }

signals:
    // 有新的业务数据可读
    void readyToRead();
    // 对端关闭或连接出错，流不再可用
    void streamClosed();
};

// ---------------------------------------------------------------------------
//  裸 TCP / TLS 流
// ---------------------------------------------------------------------------

class TcpStream : public OutboundStream {
    Q_OBJECT

public:
    // 不取得 socket 所有权（socket 生命周期由调用方/智能指针管理）
    explicit TcpStream(QAbstractSocket *socket, QObject *parent = nullptr);

    QByteArray readAll() override;
    qint64     write(const QByteArray &data) override;
    void       closeStream() override;
    QString    errorText() const override;
    bool       isOpen() const override;

private:
    QAbstractSocket *m_socket = nullptr;
};

} // namespace core
