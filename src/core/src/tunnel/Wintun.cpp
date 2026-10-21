#include "tunnel/Wintun.h"

#ifdef Q_OS_WIN

#include <QCoreApplication>

#include <windows.h>

#include <cstring>

namespace core {
namespace wintun {

namespace {

// wintun.h 里的类型与签名（不引入它的头文件，避免编译期依赖）
using AdapterHandle = void *;
using SessionHandle = void *;

using CreateAdapterFn = AdapterHandle(WINAPI *)(const wchar_t *, const wchar_t *, const GUID *);
using OpenAdapterFn   = AdapterHandle(WINAPI *)(const wchar_t *);
using CloseAdapterFn  = void(WINAPI *)(AdapterHandle);
using AdapterLuidFn   = BOOL(WINAPI *)(AdapterHandle, LUID *);
using DriverVersionFn = DWORD(WINAPI *)(void);
using StartSessionFn  = SessionHandle(WINAPI *)(AdapterHandle, DWORD);
using EndSessionFn    = void(WINAPI *)(SessionHandle);
using ReadWaitEventFn = HANDLE(WINAPI *)(SessionHandle);
using ReceivePacketFn = BYTE *(WINAPI *)(SessionHandle, DWORD *);
using ReleasePacketFn = void(WINAPI *)(SessionHandle, const BYTE *);
using AllocatePacketFn = BYTE *(WINAPI *)(SessionHandle, DWORD);
using SendPacketFn    = void(WINAPI *)(SessionHandle, const BYTE *);

struct Api {
    CreateAdapterFn  createAdapter  = nullptr;
    OpenAdapterFn    openAdapter    = nullptr;
    CloseAdapterFn   closeAdapter   = nullptr;
    AdapterLuidFn    adapterLuid    = nullptr;
    DriverVersionFn  driverVersion  = nullptr;
    StartSessionFn   startSession   = nullptr;
    EndSessionFn     endSession     = nullptr;
    ReadWaitEventFn  readWaitEvent  = nullptr;
    ReceivePacketFn  receivePacket  = nullptr;
    ReleasePacketFn  releasePacket  = nullptr;
    AllocatePacketFn allocatePacket = nullptr;
    SendPacketFn     sendPacket     = nullptr;

    bool complete() const
    {
        return createAdapter && openAdapter && closeAdapter && adapterLuid && startSession
               && endSession && readWaitEvent && receivePacket && releasePacket
               && allocatePacket && sendPacket;
    }
};

template <typename Fn>
Fn resolve(HMODULE module, const char *name)
{
    return reinterpret_cast<Fn>(reinterpret_cast<void *>(::GetProcAddress(module, name)));
}

// 只在首次调用时加载；之后直接返回同一份函数表
const Api &api()
{
    static const Api instance = [] {
        Api result;
        HMODULE module = ::LoadLibraryW(L"wintun.dll");
        if (!module) {
            // 有些安装器把 dll 放在程序目录但工作目录不同：再按绝对路径试一次
            const QString local =
                QCoreApplication::applicationDirPath() + QStringLiteral("/wintun.dll");
            module = ::LoadLibraryW(reinterpret_cast<const wchar_t *>(local.utf16()));
        }
        if (!module) return result;

        result.createAdapter  = resolve<CreateAdapterFn>(module, "WintunCreateAdapter");
        result.openAdapter    = resolve<OpenAdapterFn>(module, "WintunOpenAdapter");
        result.closeAdapter   = resolve<CloseAdapterFn>(module, "WintunCloseAdapter");
        result.adapterLuid    = resolve<AdapterLuidFn>(module, "WintunGetAdapterLUID");
        result.driverVersion  = resolve<DriverVersionFn>(module, "WintunGetRunningDriverVersion");
        result.startSession   = resolve<StartSessionFn>(module, "WintunStartSession");
        result.endSession     = resolve<EndSessionFn>(module, "WintunEndSession");
        result.readWaitEvent  = resolve<ReadWaitEventFn>(module, "WintunGetReadWaitEvent");
        result.receivePacket  = resolve<ReceivePacketFn>(module, "WintunReceivePacket");
        result.releasePacket  = resolve<ReleasePacketFn>(module, "WintunReleaseReceivePacket");
        result.allocatePacket = resolve<AllocatePacketFn>(module, "WintunAllocateSendPacket");
        result.sendPacket     = resolve<SendPacketFn>(module, "WintunSendPacket");

        if (!result.complete()) return Api{};
        return result;
    }();
    return instance;
}

QString lastErrorText(const QString &what)
{
    const DWORD code = ::GetLastError();
    wchar_t      *buffer = nullptr;
    const DWORD   length = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<wchar_t *>(&buffer), 0, nullptr);
    QString text;
    if (length > 0 && buffer) text = QString::fromWCharArray(buffer, int(length)).trimmed();
    if (buffer) ::LocalFree(buffer);
    if (text.isEmpty()) text = QStringLiteral("error %1").arg(code);
    return QStringLiteral("%1: %2").arg(what, text);
}

} // namespace

bool available()
{
    return api().complete();
}

QString versionText()
{
    const Api &table = api();
    if (!table.driverVersion) return QStringLiteral("unknown");
    const DWORD version = table.driverVersion();
    return QStringLiteral("%1.%2").arg(version >> 16).arg(version & 0xFFFF);
}

quintptr openAdapter(const QString &name, QString *error)
{
    const Api &table = api();
    if (!table.complete()) {
        if (error) {
            *error = QCoreApplication::translate(
                "clashqt::app",
                "wintun.dll was not found. Put wintun.dll next to the executable.");
        }
        return 0;
    }

    const auto *wideName = reinterpret_cast<const wchar_t *>(name.utf16());

    // 先复用已有适配器：卸载重装、快速重开都能省掉一次建适配器（要几秒）
    AdapterHandle adapter = table.openAdapter(wideName);
    if (!adapter) {
        adapter = table.createAdapter(wideName, L"ClashQt", nullptr);
    }
    if (!adapter) {
        if (error) *error = lastErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not create the Wintun adapter"));
        return 0;
    }
    return reinterpret_cast<quintptr>(adapter);
}

void closeAdapter(quintptr adapter)
{
    const Api &table = api();
    if (table.closeAdapter && adapter) {
        table.closeAdapter(reinterpret_cast<AdapterHandle>(adapter));
    }
}

quint64 adapterLuid(quintptr adapter)
{
    const Api &table = api();
    if (!table.adapterLuid || !adapter) return 0;

    LUID luid{};
    if (!table.adapterLuid(reinterpret_cast<AdapterHandle>(adapter), &luid)) return 0;
    // MinGW 的 LUID 没有 MSVC 那个 Value 联合体，自己拼
    return (quint64(quint32(luid.HighPart)) << 32) | quint32(luid.LowPart);
}

quintptr startSession(quintptr adapter, QString *error)
{
    const Api &table = api();
    if (!table.complete() || !adapter) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Wintun is unavailable");
        return 0;
    }

    // 4 MiB 环形缓冲：与官方示例一致，够高带宽突发
    const auto session = table.startSession(reinterpret_cast<AdapterHandle>(adapter), 0x400000);
    if (!session) {
        if (error) *error = lastErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not start the Wintun session"));
        return 0;
    }
    return reinterpret_cast<quintptr>(session);
}

void endSession(quintptr session)
{
    const Api &table = api();
    if (table.endSession && session) {
        table.endSession(reinterpret_cast<SessionHandle>(session));
    }
}

quintptr readWaitEvent(quintptr session)
{
    const Api &table = api();
    if (!table.readWaitEvent || !session) return 0;
    return reinterpret_cast<quintptr>(table.readWaitEvent(reinterpret_cast<SessionHandle>(session)));
}

int receivePacket(quintptr session, char *buffer, int capacity)
{
    const Api &table = api();
    if (!table.complete() || !session || !buffer || capacity <= 0) return -1;

    DWORD size   = 0;
    BYTE *packet = table.receivePacket(reinterpret_cast<SessionHandle>(session), &size);
    if (!packet) {
        const DWORD code = ::GetLastError();
        if (code == ERROR_NO_MORE_ITEMS) return 0;  // 暂时没有包
        return -1;                                   // 会话已结束或出错
    }

    int length = int(size);
    if (length > capacity) length = capacity;
    std::memcpy(buffer, packet, size_t(length));
    table.releasePacket(reinterpret_cast<SessionHandle>(session), packet);
    return length;
}

bool sendPacket(quintptr session, const char *data, int size)
{
    const Api &table = api();
    if (!table.complete() || !session || !data || size <= 0) return false;

    BYTE *packet = table.allocatePacket(reinterpret_cast<SessionHandle>(session), DWORD(size));
    if (!packet) return false; // 队列满：上层按丢包处理（TCP 会重传）

    std::memcpy(packet, data, size_t(size));
    table.sendPacket(reinterpret_cast<SessionHandle>(session), packet);
    return true;
}

} // namespace wintun
} // namespace core

#else // !Q_OS_WIN —— 非 Windows 上不提供 Wintun，仅保留可编译的空实现

namespace core {
namespace wintun {

bool    available() { return false; }
QString versionText() { return QString(); }
quintptr openAdapter(const QString &, QString *) { return 0; }
void     closeAdapter(quintptr) {}
quint64  adapterLuid(quintptr) { return 0; }
quintptr startSession(quintptr, QString *) { return 0; }
void     endSession(quintptr) {}
quintptr readWaitEvent(quintptr) { return 0; }
int      receivePacket(quintptr, char *, int) { return -1; }
bool     sendPacket(quintptr, const char *, int) { return false; }

} // namespace wintun
} // namespace core

#endif // Q_OS_WIN
