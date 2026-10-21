#pragma once

// 平台差异收敛层：socket 类型 / 关闭 / 非阻塞 / 轮询 / 错误串 / 句柄复制。
// 除头文件与类型别名必须按平台引入外，函数差异一律写在函数内部。
//
// 注意：Windows 上 winsock2.h 必须早于 windows.h，使用方须在**任何**
// windows.h 之前包含本头（本头内部已保证顺序）。

#include <QtGlobal>

#ifdef Q_OS_WIN

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>

#else

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// sockaddr_in / sockaddr_in6 / htons / htonl / ntohs / ntohl / IPPROTO_* 都在
// netinet/in.h；inet_pton / inet_ntop 在 arpa/inet.h。sys/socket.h 只给
// sockaddr / socklen_t / AF_INET / SOCK_DGRAM，缺这两个头会在使用具体地址
// 结构的文件里编译失败（Hysteria2Client 用了 sockaddr_in6 与 htons）。
#include <arpa/inet.h>
#include <netinet/in.h>

#endif

#include <QString>

namespace clashqt {
namespace platform {

// socket 句柄在 Windows 上是 UINT_PTR，64 位进程里不能用 int 承载
#ifdef Q_OS_WIN
using SocketHandle = SOCKET;
inline constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
using SocketHandle = int;
inline constexpr SocketHandle kInvalidSocket = -1;
#endif

// Windows 的 Winsock 需要显式初始化：Qt 自己也会初始化，但裸 socket 可能先于
// QTcpSocket 被创建（例如 TUN 之前先建 UDP），所以这里做一次幂等初始化。
inline bool ensureWinsock()
{
#ifdef Q_OS_WIN
    static const bool ok = [] {
        WSADATA data{};
        return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }();
    return ok;
#else
    return true;
#endif
}

inline int closeSocket(SocketHandle handle)
{
#ifdef Q_OS_WIN
    return ::closesocket(handle);
#else
    return ::close(handle);
#endif
}

// 关闭 CRT 描述符（不是 socket）：TUN 链路在 Linux 上是 /dev/net/tun 的 fd，
// Windows 上走 Wintun 会话句柄，但参数校验失败等路径仍按描述符语义清理。
inline int closeDescriptor(int descriptor)
{
#ifdef Q_OS_WIN
    return ::_close(descriptor);
#else
    return ::close(descriptor);
#endif
}

// fcntl(O_NONBLOCK) 的等价物
inline bool setNonBlocking(SocketHandle handle)
{
#ifdef Q_OS_WIN
    u_long mode = 1;
    return ::ioctlsocket(handle, FIONBIO, &mode) == 0;
#else
    const int flags = ::fcntl(handle, F_GETFL, 0);
    return flags >= 0 && ::fcntl(handle, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

inline QString socketErrorText()
{
#ifdef Q_OS_WIN
    const int code = ::WSAGetLastError();
    wchar_t *buffer = nullptr;
    const DWORD length = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, DWORD(code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<wchar_t *>(&buffer), 0, nullptr);
    QString text;
    if (length > 0 && buffer) {
        text = QString::fromWCharArray(buffer, int(length)).trimmed();
    }
    if (buffer) ::LocalFree(buffer);
    if (text.isEmpty()) text = QStringLiteral("WSA error %1").arg(code);
    return QStringLiteral("%1 (%2)").arg(text).arg(code);
#else
    return QString::fromLocal8Bit(std::strerror(errno));
#endif
}

// 同一进程内复制一个 socket：POSIX 是 dup()，Winsock 要先取协议信息再重建。
// 返回 kInvalidSocket 表示失败。
inline SocketHandle duplicateSocket(SocketHandle handle)
{
#ifdef Q_OS_WIN
    WSAPROTOCOL_INFOW info{};
    if (::WSADuplicateSocketW(handle, ::GetCurrentProcessId(), &info) != 0) {
        return kInvalidSocket;
    }
    return ::WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &info, 0,
                        WSA_FLAG_OVERLAPPED);
#else
    return ::dup(handle);
#endif
}

// 让阻塞在这个 socket 上的读写立刻返回（返回 0 / 出错），但**不释放句柄**。
// 停监听时用它打断在途连接：工作线程可能正卡在 waitForReadyRead 上，
// 等到超时要几秒；而 close 会让 fd 号被其它线程立刻复用，跨线程 close
// 会误伤新连接，shutdown 没有这个问题（真正的 close 仍由持有者自己做）。
inline int shutdownSocket(SocketHandle handle)
{
#ifdef Q_OS_WIN
    return ::shutdown(handle, SD_BOTH);
#else
    return ::shutdown(handle, SHUT_RDWR);
#endif
}

// 等待 socket 可读/可写；返回 >0 表示就绪，0 表示超时，<0 表示出错
inline int pollSocket(SocketHandle handle, bool forWrite, int timeoutMs)
{
#ifdef Q_OS_WIN
    WSAPOLLFD pfd{};
    pfd.fd      = handle;
    pfd.events  = forWrite ? POLLWRNORM : POLLRDNORM;
    pfd.revents = 0;
    return ::WSAPoll(&pfd, 1, timeoutMs);
#else
    struct pollfd pfd{};
    pfd.fd      = handle;
    pfd.events  = forWrite ? POLLOUT : POLLIN;
    pfd.revents = 0;
    return ::poll(&pfd, 1, timeoutMs);
#endif
}

} // namespace platform
} // namespace clashqt
