#include "utils/CrashHandler.h"

#include <csignal>
#include <cstdint>
#include <cstring>
#include <initializer_list>

#include <execinfo.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace clashqt {
namespace crash {
namespace {

// 信号处理器里不能分配堆内存：路径与所有行缓冲都放在静态区
char              g_logDir[512] = {0};
volatile sig_atomic_t g_inHandler = 0;

size_t appendStr(char *dst, size_t pos, size_t cap, const char *text)
{
    if (!text) return pos;
    const size_t length = std::strlen(text);
    if (pos + length >= cap) return pos;
    std::memcpy(dst + pos, text, length);
    return pos + length;
}

size_t appendDec(char *dst, size_t pos, size_t cap, unsigned long long value)
{
    char reversed[32];
    int  count = 0;
    if (value == 0) {
        if (pos + 1 < cap) dst[pos++] = '0';
        return pos;
    }
    while (value > 0 && count < int(sizeof(reversed))) {
        reversed[count++] = char('0' + value % 10);
        value /= 10;
    }
    while (count-- > 0 && pos + 1 < cap) dst[pos++] = reversed[count];
    return pos;
}

size_t appendHex(char *dst, size_t pos, size_t cap, unsigned long long value)
{
    static const char digits[] = "0123456789abcdef";
    char              reversed[32];
    int               count = 0;
    do {
        reversed[count++] = digits[value & 0xF];
        value >>= 4;
    } while (value > 0 && count < int(sizeof(reversed)));
    while (count-- > 0 && pos + 1 < cap) dst[pos++] = reversed[count];
    return pos;
}

const char *signalName(int sig)
{
    switch (sig) {
    case SIGSEGV: return "SIGSEGV";
    case SIGABRT: return "SIGABRT";
    case SIGBUS: return "SIGBUS";
    case SIGFPE: return "SIGFPE";
    case SIGILL: return "SIGILL";
    default: return "UNKNOWN";
    }
}

void writeAll(int fd, const char *data, size_t length)
{
    while (length > 0) {
        const ssize_t written = ::write(fd, data, length);
        if (written <= 0) return;
        data += written;
        length -= size_t(written);
    }
}

void handler(int sig, siginfo_t *info, void *)
{
    // 处理器内再次崩溃（或信号重入）：直接退出，避免死循环
    if (g_inHandler) _exit(128 + sig);
    g_inHandler = 1;

    const unsigned long long tid = static_cast<unsigned long long>(::syscall(SYS_gettid));

    char path[640];
    size_t pos = 0;
    pos = appendStr(path, pos, sizeof(path), g_logDir[0] ? g_logDir : ".");
    pos = appendStr(path, pos, sizeof(path), "/crash-");
    pos = appendDec(path, pos, sizeof(path), tid);
    path[pos < sizeof(path) - 1 ? pos++ : pos] = '\0';

    const int fd = ::open(path, O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0644);
    if (fd >= 0) {
        char   line[320];
        size_t n = 0;
        n = appendStr(line, n, sizeof(line), "==== crash ====\nsignal=");
        n = appendDec(line, n, sizeof(line), static_cast<unsigned long long>(sig));
        n = appendStr(line, n, sizeof(line), " (");
        n = appendStr(line, n, sizeof(line), signalName(sig));
        n = appendStr(line, n, sizeof(line), ")\ntid=");
        n = appendDec(line, n, sizeof(line), tid);
        if (info) {
            n = appendStr(line, n, sizeof(line), "\nfault addr=0x");
            n = appendHex(line, n, sizeof(line),
                          static_cast<unsigned long long>(
                              reinterpret_cast<uintptr_t>(info->si_addr)));
        }
        n = appendStr(line, n, sizeof(line), "\nframes:\n");
        writeAll(fd, line, n);

        void      *frames[64];
        const int count = ::backtrace(frames, 64);
        // 输出形如 "clash-qt(+0x1a2b3) [0x55...]"：带调试信息的构建里
        // 可以直接 addr2line -e build/bin/clash-qt -f -C 0x1a2b3 定位到行号
        ::backtrace_symbols_fd(frames, count, fd);

        writeAll(fd, "==== end ====\n", 14);
        ::close(fd);
    }

    // 交回默认处理：保留系统原本的 core dump 行为
    ::signal(sig, SIG_DFL);
    ::raise(sig);
    _exit(128 + sig);
}

} // namespace

void install(const char *logDir)
{
    if (logDir && *logDir) {
        std::strncpy(g_logDir, logDir, sizeof(g_logDir) - 1);
        g_logDir[sizeof(g_logDir) - 1] = '\0';
    }

    struct sigaction action {};
    action.sa_sigaction = handler;
    action.sa_flags     = SA_SIGINFO | SA_RESETHAND;
    sigemptyset(&action.sa_mask);

    for (int sig : {SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL}) {
        ::sigaction(sig, &action, nullptr);
    }
}

} // namespace crash
} // namespace clashqt
