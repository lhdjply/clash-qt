#pragma once

// ---------------------------------------------------------------------------
//  阻塞式套接字读取工具
//
//  背景（重要，容易踩坑）：
//    QTcpSocket 的 waitForReadyRead() 只等待「内核 socket 上的新数据」。
//    若数据已经被 socket 引擎读入内部读缓冲（例如之前调用过 peek()、
//    readAll() 或事件循环已处理过一次通知），waitForReadyRead() 会
//    在没有任何新数据时一直阻塞到超时，即便 bytesAvailable() > 0。
//
//    因此所有握手读取都必须遵循「先消费已缓冲的数据，再等待新数据」的顺序。
//    本文件把这套顺序封装为两个函数，避免在每个协议实现里重复出错。
//
//  适用场景：工作线程中的同步握手（HTTP 请求头、SOCKS5 协商等）。
// ---------------------------------------------------------------------------

#include <QByteArray>
#include <QString>

class QAbstractSocket;

namespace core {
namespace sockutil {

// 持续读取，直到 out 中出现 delimiter（delimiter 包含在结果内），
// 或达到 maxBytes 上限、超时、连接断开。
// 返回是否成功找到 delimiter；失败原因写入 error（可为空）。
bool readUntil(QAbstractSocket *socket,
               QByteArray *out,
               const QByteArray &delimiter,
               int timeoutMs,
               int maxBytes,
               QString *error = nullptr);

// 持续读取，直到 out 至少包含 count 字节。
bool readAtLeast(QAbstractSocket *socket,
                 QByteArray *out,
                 int count,
                 int timeoutMs,
                 QString *error = nullptr);

// 把当前已缓冲的数据全部取出（不等待）
QByteArray drainAvailable(QAbstractSocket *socket);

// 阻塞式写入全部数据；返回是否成功
bool writeAll(QAbstractSocket *socket, const QByteArray &payload, int timeoutMs,
              QString *error = nullptr);

} // namespace sockutil
} // namespace core
