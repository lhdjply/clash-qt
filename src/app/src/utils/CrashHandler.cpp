#include <QtGlobal> // Q_OS_WIN 等平台宏（必须早于下面的平台分支）

#include "utils/CrashHandler.h"

#include <csignal>
#include <cstdint>
#include <cstring>

// 崩溃处理：Linux 用 sigaction + backtrace（保留 core dump），Windows 用
// SetUnhandledExceptionFilter + StackWalk64，另写一份 minidump。
// 处理器内不分配堆内存、不调用 Qt、不加锁；文本拼接工具两平台共用。

#ifdef Q_OS_WIN
#include <cstdlib>
#include <cwchar>
#include <iterator>

#include <windows.h>
#include <dbghelp.h>
#else
#include <initializer_list>

#include <execinfo.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace clashqt {
namespace crash {
namespace {

// 平台无关：把数字/字符串拼进固定缓冲

size_t appendStr(char *dst, size_t pos, size_t cap, const char *text)
{
    if (!text) return pos;
    const size_t length = std::strlen(text);
    for (size_t i = 0; i < length && pos + 1 < cap; ++i) dst[pos++] = text[i];
    return pos;
}

size_t appendDec(char *dst, size_t pos, size_t cap, unsigned long long value)
{
    char reversed[32];
    int  count = 0;
    do {
        reversed[count++] = char('0' + value % 10);
        value /= 10;
    } while (value > 0 && count < int(sizeof(reversed)));
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

// 崩溃现场的文件句柄：Windows 是 HANDLE，Linux 是 fd
#ifdef Q_OS_WIN
using CrashFile = HANDLE;
#else
using CrashFile = int;
#endif

void writeAll(CrashFile file, const char *data, size_t length)
{
    while (length > 0) {
#ifdef Q_OS_WIN
        DWORD written = 0;
        if (!::WriteFile(file, data, DWORD(length), &written, nullptr) || written == 0) return;
        data += written;
        length -= written;
#else
        const ssize_t written = ::write(file, data, length);
        if (written <= 0) return;
        data += written;
        length -= size_t(written);
#endif
    }
}

#ifdef Q_OS_WIN

// Windows 后端：SEH 过滤器 + dbghelp 栈回溯 + minidump

// 崩溃处理期间只能碰静态存储：路径、行缓冲、句柄都放这里
wchar_t       g_logDirW[512] = {0};
volatile LONG g_inHandler    = 0;

struct DbgHelpApi {
    BOOL (WINAPI *miniDumpWriteDump)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
                                     PMINIDUMP_EXCEPTION_INFORMATION,
                                     PMINIDUMP_USER_STREAM_INFORMATION,
                                     PMINIDUMP_CALLBACK_INFORMATION);
    BOOL (WINAPI *symInitialize)(HANDLE, PCSTR, BOOL);
    BOOL (WINAPI *stackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID,
                               PREAD_PROCESS_MEMORY_ROUTINE64,
                               PFUNCTION_TABLE_ACCESS_ROUTINE64,
                               PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
    DWORD64 (WINAPI *symGetModuleBase64)(HANDLE, DWORD64);
    BOOL (WINAPI *symFromAddr)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
    BOOL (WINAPI *symGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
};

DbgHelpApi g_dbg{};
bool       g_dbgReady = false;

size_t appendStr(wchar_t *dst, size_t pos, size_t cap, const wchar_t *text)
{
    if (!text) return pos;
    const size_t length = std::wcslen(text);
    for (size_t i = 0; i < length && pos + 1 < cap; ++i) dst[pos++] = text[i];
    return pos;
}

const char *exceptionName(DWORD code)
{
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:      return "ACCESS_VIOLATION";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
    case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:    return "FLT_DIVIDE_BY_ZERO";
    case EXCEPTION_ILLEGAL_INSTRUCTION:   return "ILLEGAL_INSTRUCTION";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:    return "INT_DIVIDE_BY_ZERO";
    case EXCEPTION_STACK_OVERFLOW:        return "STACK_OVERFLOW";
    case EXCEPTION_BREAKPOINT:            return "BREAKPOINT";
    default:                              return "UNKNOWN";
    }
}

// 拼出 <logDir>\crash-<tid>.<suffix>；返回写入的字符数
size_t buildCrashPath(wchar_t *out, size_t cap, DWORD tid, const wchar_t *suffix)
{
    size_t pos = 0;
    pos = appendStr(out, pos, cap, g_logDirW[0] ? g_logDirW : L".");
    pos = appendStr(out, pos, cap, L"\\crash-");

    wchar_t digits[16];
    int     count = 0;
    do {
        digits[count++] = wchar_t(L'0' + tid % 10);
        tid /= 10;
    } while (tid > 0 && count < int(sizeof(digits)));
    while (count-- > 0 && pos + 1 < cap) out[pos++] = digits[count];

    pos = appendStr(out, pos, cap, suffix);
    out[pos < cap ? pos : cap - 1] = L'\0';
    return pos;
}

void loadDbgHelp()
{
    if (g_dbgReady) return;
    HMODULE module = ::LoadLibraryW(L"dbghelp.dll");
    if (!module) return;

    g_dbg.miniDumpWriteDump =
        reinterpret_cast<decltype(g_dbg.miniDumpWriteDump)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "MiniDumpWriteDump")));
    g_dbg.symInitialize =
        reinterpret_cast<decltype(g_dbg.symInitialize)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "SymInitialize")));
    g_dbg.stackWalk64 =
        reinterpret_cast<decltype(g_dbg.stackWalk64)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "StackWalk64")));
    g_dbg.symGetModuleBase64 =
        reinterpret_cast<decltype(g_dbg.symGetModuleBase64)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "SymGetModuleBase64")));
    g_dbg.symFromAddr =
        reinterpret_cast<decltype(g_dbg.symFromAddr)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "SymFromAddr")));
    g_dbg.symGetLineFromAddr64 =
        reinterpret_cast<decltype(g_dbg.symGetLineFromAddr64)>(
            reinterpret_cast<void *>(::GetProcAddress(module, "SymGetLineFromAddr64")));

    if (g_dbg.symInitialize) {
        // fInvadeProcess = FALSE：崩溃线程里不能再遍历模块去做符号加载
        g_dbg.symInitialize(::GetCurrentProcess(), nullptr, FALSE);
    }
    g_dbgReady = true;
}

// 栈帧写成 "symbol + 0x偏移 [file:line]"：没有 pdb 时至少能定位到模块偏移，
// 有 pdb（RelWithDebInfo 构建）时能直接给到行号
void writeFrames(HANDLE file, CONTEXT *context)
{
    loadDbgHelp();
    if (!g_dbg.stackWalk64) return;

    STACKFRAME64 frame{};
    DWORD        machine = 0;
#if defined(_M_X64) || defined(__x86_64__)
    machine                = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset    = context->Rip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context->Rbp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context->Rsp;
    frame.AddrStack.Mode   = AddrModeFlat;
#elif defined(_M_ARM64) || defined(__aarch64__)
    machine                = IMAGE_FILE_MACHINE_ARM64;
    frame.AddrPC.Offset    = context->Pc;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context->Fp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context->Sp;
    frame.AddrStack.Mode   = AddrModeFlat;
#else
    machine                = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset    = context->Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context->Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context->Esp;
    frame.AddrStack.Mode   = AddrModeFlat;
#endif

    const HANDLE process = ::GetCurrentProcess();
    const HANDLE thread  = ::GetCurrentThread();

    // SYMBOL_INFO 尾部自带名字空间，按需多留 256 字节
    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + 256] = {0};
    auto *symbol = reinterpret_cast<SYMBOL_INFO *>(symbolBuffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen   = MAX_SYM_NAME;

    for (int index = 0; index < 64; ++index) {
        if (!g_dbg.stackWalk64(machine, process, thread, &frame, context, nullptr, nullptr,
                               g_dbg.symGetModuleBase64, nullptr)) {
            break;
        }
        if (frame.AddrPC.Offset == 0) break;

        const DWORD64 pc = frame.AddrPC.Offset;

        char   line[512];
        size_t n = 0;
        n = appendStr(line, n, sizeof(line), "  #");
        n = appendDec(line, n, sizeof(line), static_cast<unsigned long long>(index));
        n = appendStr(line, n, sizeof(line), " 0x");
        n = appendHex(line, n, sizeof(line), pc);

        DWORD64 displacement = 0;
        if (g_dbg.symFromAddr && g_dbg.symFromAddr(process, pc, &displacement, symbol)) {
            n = appendStr(line, n, sizeof(line), " ");
            n = appendStr(line, n, sizeof(line), symbol->Name);
            n = appendStr(line, n, sizeof(line), " + 0x");
            n = appendHex(line, n, sizeof(line), displacement);

            IMAGEHLP_LINE64 sourceInfo{};
            sourceInfo.SizeOfStruct = sizeof(sourceInfo);
            DWORD lineDisplacement  = 0;
            if (g_dbg.symGetLineFromAddr64
                && g_dbg.symGetLineFromAddr64(process, pc, &lineDisplacement, &sourceInfo)) {
                n = appendStr(line, n, sizeof(line), " [");
                n = appendStr(line, n, sizeof(line), sourceInfo.FileName);
                n = appendStr(line, n, sizeof(line), ":");
                n = appendDec(line, n, sizeof(line),
                              static_cast<unsigned long long>(sourceInfo.LineNumber));
                n = appendStr(line, n, sizeof(line), "]");
            }
        } else if (g_dbg.symGetModuleBase64) {
            const DWORD64 base = g_dbg.symGetModuleBase64(process, pc);
            if (base) {
                n = appendStr(line, n, sizeof(line), " <module+0x");
                n = appendHex(line, n, sizeof(line), pc - base);
                n = appendStr(line, n, sizeof(line), ">");
            }
        }
        n = appendStr(line, n, sizeof(line), "\n");
        writeAll(file, line, n);
    }
}

void writeMiniDump(EXCEPTION_POINTERS *info, DWORD tid)
{
    loadDbgHelp();
    if (!g_dbg.miniDumpWriteDump) return;

    wchar_t path[640];
    buildCrashPath(path, 640, tid, L".dmp");

    const HANDLE file = ::CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo{};
    exceptionInfo.ThreadId          = tid;
    exceptionInfo.ExceptionPointers = info;
    exceptionInfo.ClientPointers    = FALSE;

    g_dbg.miniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), file,
                            MiniDumpWithThreadInfo, &exceptionInfo, nullptr, nullptr);
    ::CloseHandle(file);
}

// signal 入口没有异常上下文，只写文本
void writeSignalReport(int sig)
{
    const DWORD tid = ::GetCurrentThreadId();

    wchar_t path[640];
    buildCrashPath(path, 640, tid, L".txt");

    const HANDLE file = ::CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;

    char   line[384];
    size_t n = 0;
    n = appendStr(line, n, sizeof(line), "==== crash ====\nsignal=");
    n = appendDec(line, n, sizeof(line), static_cast<unsigned long long>(sig));
    n = appendStr(line, n, sizeof(line), "\ntid=");
    n = appendDec(line, n, sizeof(line), static_cast<unsigned long long>(tid));
    n = appendStr(line, n, sizeof(line), "\nframes: (no exception context available)\n");
    n = appendStr(line, n, sizeof(line), "==== end ====\n");
    writeAll(file, line, n);
    ::CloseHandle(file);
}

LONG WINAPI unhandledFilter(EXCEPTION_POINTERS *info)
{
    if (::InterlockedExchange(&g_inHandler, 1) != 0) return EXCEPTION_CONTINUE_SEARCH;

    const DWORD tid  = ::GetCurrentThreadId();
    const DWORD code = info && info->ExceptionRecord ? info->ExceptionRecord->ExceptionCode : 0;

    wchar_t path[640];
    buildCrashPath(path, 640, tid, L".txt");

    const HANDLE file = ::CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        char   line[512];
        size_t n = 0;
        n = appendStr(line, n, sizeof(line), "==== crash ====\ncode=0x");
        n = appendHex(line, n, sizeof(line), code);
        n = appendStr(line, n, sizeof(line), " (");
        n = appendStr(line, n, sizeof(line), exceptionName(code));
        n = appendStr(line, n, sizeof(line), ")\ntid=");
        n = appendDec(line, n, sizeof(line), static_cast<unsigned long long>(tid));
        if (info && info->ExceptionRecord) {
            n = appendStr(line, n, sizeof(line), "\nfault addr=0x");
            n = appendHex(line, n, sizeof(line),
                          reinterpret_cast<uintptr_t>(info->ExceptionRecord->ExceptionAddress));
            // 访问冲突的记录项：[0] 读/写/执行，[1] 访问的地址
            if (code == EXCEPTION_ACCESS_VIOLATION && info->ExceptionRecord->NumberParameters >= 2) {
                n = appendStr(line, n, sizeof(line), "\naccess type=");
                n = appendDec(line, n, sizeof(line), info->ExceptionRecord->ExceptionInformation[0]);
                n = appendStr(line, n, sizeof(line), "\naccess addr=0x");
                n = appendHex(line, n, sizeof(line), info->ExceptionRecord->ExceptionInformation[1]);
            }
        }
        n = appendStr(line, n, sizeof(line), "\nframes:\n");
        writeAll(file, line, n);
        if (info) writeFrames(file, info->ContextRecord);
        writeAll(file, "==== end ====\n", 14);
        ::CloseHandle(file);
    }

    writeMiniDump(info, tid);

    // 交回系统：保留 WER 与系统原本的终止行为
    return EXCEPTION_CONTINUE_SEARCH;
}

void signalHandler(int sig)
{
    if (::InterlockedExchange(&g_inHandler, 1) != 0) ::TerminateProcess(::GetCurrentProcess(), 3);
    writeSignalReport(sig);
    ::TerminateProcess(::GetCurrentProcess(), DWORD(128 + sig));
}

#else // !Q_OS_WIN

// Linux 后端：sigaction + backtrace

// 信号处理器里不能分配堆内存：路径与所有行缓冲都放在静态区
char              g_logDir[512] = {0};
volatile sig_atomic_t g_inHandler = 0;

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

#endif // Q_OS_WIN

} // namespace

void install(const char *logDir)
{
#ifdef Q_OS_WIN
    if (logDir && *logDir) {
        const int length = ::MultiByteToWideChar(CP_UTF8, 0, logDir, -1, g_logDirW,
                                                 int(std::size(g_logDirW)) - 1);
        if (length <= 0) g_logDirW[0] = L'\0';
    }

    // 反复调用只生效一次（与 Linux 版一致）
    static bool installed = false;
    if (installed) return;
    installed = true;

    ::SetUnhandledExceptionFilter(unhandledFilter);

    // CRT 的 abort() / 除零会以 signal 形式出现，走文本记录
    for (int sig : {SIGABRT, SIGFPE, SIGILL, SIGSEGV}) {
        std::signal(sig, signalHandler);
    }

#ifdef _MSC_VER
    // MSVC 的 CRT 会在 abort() 前弹「abort() has been called」对话框，
    // 这个开关能把它关掉；MinGW 的 msvcrt 不导出 _set_abort_behavior，
    // 那边靠下面的 SetErrorMode 兜住（不弹 WER 框即可落盘）。
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
    ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#else
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
#endif
}

} // namespace crash
} // namespace clashqt
