#pragma once

// ---------------------------------------------------------------------------
//  崩溃现场记录
//
//  为什么需要：TUN 数据面是多线程的（数据面线程 + 每连接一个拨号线程），
//  段错误的现场往往在「几十个线程里某一条的某一行」。用户手动跑 gdb 时
//  通常只能拿到 `| tail -60`，恰好把最重要的一段（崩溃信号 + 崩溃线程栈）
//  裁掉，于是只能靠猜。
//
//  这里装一组致命信号处理器：崩溃瞬间把信号、地址、崩溃线程的调用栈写到
//  数据目录下的 crash-<tid>，然后按默认行为重新抛出（保留系统的 core dump）。
//
//  注意：信号处理器里只能用异步信号安全的操作，所以只做 open/write/_exit
//  与 backtrace()（glibc 实现为 async-signal-safe 的实践可接受），
//  不分配内存、不加锁、不调用 Qt。
// ---------------------------------------------------------------------------

namespace clashqt {
namespace crash {

// 安装 SIGSEGV / SIGABRT / SIGBUS / SIGFPE / SIGILL 的处理器。
// logDir 为空时退回当前工作目录。重复调用只生效一次。
void install(const char *logDir = nullptr);

} // namespace crash
} // namespace clashqt
