#include <QtGlobal> // Q_OS_WIN 等平台宏（必须早于下面的平台分支）

// TUN 控制器（主程序侧）。两平台特权模型不同：
//   Linux   辅助程序建 TUN，经 unix socket + SCM_RIGHTS 把 fd 交给主进程；
//   Windows 本进程建 Wintun 适配器并配地址/路由/DNS（需管理员），
//           内核侧再按 LUID 打开同一适配器收发（没有 fd 传递这回事）。
//   报成功会让 UI 以为流量已接管，所以任何一步失败都必须如实返回原因。

#include "sys/TunController.h"

#include "utils/AppLogger.h"

#include <QCoreApplication>

#ifdef Q_OS_WIN
#include "tunnel/Wintun.h"

#include <winsock2.h>
#include <ws2ipdef.h>

#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>

#include <QProcess>
#else
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSocketNotifier>
#include <QStandardPaths>
#include <QTimer>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace clashqt {
namespace {

#ifdef Q_OS_WIN

// 是否为提升后的进程（UAC 管理员）。用 TokenElevation 而不是 IsUserAnAdmin：
// 后者只说明账号属于管理员组，进程本身可能仍是未提升的。
bool isElevated()
{
    HANDLE token = nullptr;
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token)) return false;

    TOKEN_ELEVATION elevation{};
    DWORD           size = sizeof(elevation);
    const bool      ok   = ::GetTokenInformation(token, TokenElevation, &elevation,
                                                 sizeof(elevation), &size)
                      && elevation.TokenIsElevated != 0;
    ::CloseHandle(token);
    return ok;
}

// IP Helper 的返回值是 Win32 错误码（不是 errno），单独转一次文本
QString ipErrorText(const QString &what, DWORD code)
{
    wchar_t    *buffer = nullptr;
    const DWORD length = ::FormatMessageW(
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

// "172.19.0.1" → 网络序 32 位（不用 inet_pton：省掉字符集与 API 版本问题）
bool parseIpv4(const QString &text, quint32 *networkOrder)
{
    const QStringList parts = text.trimmed().split(QLatin1Char('.'));
    if (parts.size() != 4) return false;

    quint32 value = 0;
    for (const QString &part : parts) {
        bool      ok    = false;
        const int octet = part.toInt(&ok);
        if (!ok || octet < 0 || octet > 255) return false;
        value = (value << 8) | quint32(octet);
    }
    *networkOrder = ::htonl(value);
    return true;
}

bool setAddress(quint64 luid, quint32 address, int prefix, QString *error)
{
    MIB_UNICASTIPADDRESS_ROW row;
    ::InitializeUnicastIpAddressEntry(&row);
    row.InterfaceLuid.Value                      = luid;
    row.Address.si_family                        = AF_INET;
    row.Address.Ipv4.sin_family                  = AF_INET;
    row.Address.Ipv4.sin_addr.S_un.S_addr        = address;
    row.OnLinkPrefixLength                       = UINT8(prefix);
    // TUN 上没有别的机器会占用这个地址，跳过重复地址检测，省掉 1 秒等待
    row.DadState                                 = IpDadStatePreferred;
    row.PrefixOrigin                             = IpPrefixOriginManual;
    row.SuffixOrigin                             = IpSuffixOriginManual;
    row.ValidLifetime                            = 0xFFFFFFFF;
    row.PreferredLifetime                        = 0xFFFFFFFF;

    const DWORD status = ::CreateUnicastIpAddressEntry(&row);
    if (status != NO_ERROR && status != ERROR_OBJECT_ALREADY_EXISTS) {
        if (error) *error = ipErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not set the TUN adapter address"), status);
        return false;
    }
    return true;
}

bool setInterfaceOptions(quint64 luid, int mtu, int metric, QString *error)
{
    MIB_IPINTERFACE_ROW row;
    ::InitializeIpInterfaceEntry(&row);
    row.InterfaceLuid = NET_LUID{};
    row.InterfaceLuid.Value = luid;   // 见下：MinGW 的 NET_LUID 有 Value 成员
    row.Family              = AF_INET;

    DWORD status = ::GetIpInterfaceEntry(&row);
    if (status != NO_ERROR) {
        if (error) *error = ipErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not read the TUN adapter settings"), status);
        return false;
    }

    if (mtu > 0) row.NlMtu = ULONG(mtu);
    if (metric > 0) {
        // 必须关掉自动 metric，否则 Windows 会按链路速度重算并忽略我们指定的值
        row.UseAutomaticMetric = FALSE;
        row.Metric             = ULONG(metric);
    }
    // 文档明确要求：调用 SetIpInterfaceEntry 前必须清零，否则返回 ERROR_INVALID_PARAMETER
    row.SitePrefixLength = 0;

    status = ::SetIpInterfaceEntry(&row);
    if (status != NO_ERROR) {
        if (error) *error = ipErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not set the TUN adapter MTU/metric"), status);
        return false;
    }
    return true;
}

// nextHop 为 0.0.0.0 表示 on-link（TUN 就是这种：对端不存在，包直接进会话）
bool addRoute(quint64 luid, quint32 network, int prefix, quint32 nextHop, QString *error)
{
    MIB_IPFORWARD_ROW2 row;
    ::InitializeIpForwardEntry(&row);
    row.InterfaceLuid.Value                                 = luid;
    row.DestinationPrefix.Prefix.si_family                  = AF_INET;
    row.DestinationPrefix.Prefix.Ipv4.sin_family            = AF_INET;
    row.DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr  = network;
    row.DestinationPrefix.PrefixLength                      = UINT8(prefix);
    row.NextHop.si_family                                   = AF_INET;
    row.NextHop.Ipv4.sin_family                             = AF_INET;
    row.NextHop.Ipv4.sin_addr.S_un.S_addr                   = nextHop;
    row.Metric                                              = 1;

    const DWORD status = ::CreateIpForwardEntry2(&row);
    // 上次异常退出可能留下同一条路由：已存在视为成功
    if (status != NO_ERROR && status != ERROR_OBJECT_ALREADY_EXISTS) {
        if (error) *error = ipErrorText(QCoreApplication::translate(
                                 "clashqt::app", "Could not add the TUN route"), status);
        return false;
    }
    return true;
}

void deleteRoute(quint64 luid, quint32 network, int prefix, quint32 nextHop)
{
    MIB_IPFORWARD_ROW2 row;
    ::InitializeIpForwardEntry(&row);
    row.InterfaceLuid.Value                                = luid;
    row.DestinationPrefix.Prefix.si_family                 = AF_INET;
    row.DestinationPrefix.Prefix.Ipv4.sin_family           = AF_INET;
    row.DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr = network;
    row.DestinationPrefix.PrefixLength                     = UINT8(prefix);
    // 删除要与添加时的行完全一致（含下一跳），否则匹配不到
    row.NextHop.si_family                                  = AF_INET;
    row.NextHop.Ipv4.sin_family                            = AF_INET;
    row.NextHop.Ipv4.sin_addr.S_un.S_addr                  = nextHop;
    ::DeleteIpForwardEntry2(&row);
}

bool runNetsh(const QStringList &arguments, QString *error)
{
    QProcess process;
    process.start(QStringLiteral("netsh"), arguments);
    if (!process.waitForStarted(5000)) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Could not start netsh");
        return false;
    }
    if (!process.waitForFinished(15000)) {
        process.kill();
        if (error) *error = QCoreApplication::translate("clashqt::app", "netsh timed out");
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error) {
            *error = QString::fromLocal8Bit(process.readAllStandardOutput()
                                            + process.readAllStandardError())
                         .trimmed();
        }
        return false;
    }
    return true;
}

// 让系统解析器问 TUN 地址：查询随后被内核的 53 劫持（fake-ip 因此生效）
bool setDnsServers(const QString &alias, const QString &server, QString *error)
{
    return runNetsh({QStringLiteral("interface"), QStringLiteral("ipv4"),
                     QStringLiteral("set"), QStringLiteral("dnsservers"),
                     QStringLiteral("name=") + alias, QStringLiteral("static"), server,
                     QStringLiteral("primary")},
                    error);
}

void clearDnsServers(const QString &alias)
{
    if (alias.isEmpty()) return;
    runNetsh({QStringLiteral("interface"), QStringLiteral("ipv4"),
              QStringLiteral("delete"), QStringLiteral("dnsservers"),
              QStringLiteral("name=") + alias, QStringLiteral("address=all")},
             nullptr);
}

#else // !Q_OS_WIN

constexpr char kHelperName[] = CLASHQT_TUN_HELPER_NAME;

// 辅助程序与 pkexec 的路径都按绝对路径使用：绝不让 PATH 决定提权目标
QString absolutePath(const QString &path)
{
    if (path.isEmpty()) return path;
    if (path.startsWith(QLatin1Char('/'))) return path;

    // pkexec 要求使用绝对路径（否则会拒绝执行）
    const QByteArray candidate = QFile::encodeName(QDir::current().absoluteFilePath(path));
    return QString::fromLocal8Bit(candidate);
}

#endif // Q_OS_WIN

} // namespace

TunController::TunController(QObject *parent) : QObject(parent) {}

TunController::~TunController() { stop(); }

bool TunController::isRunning() const
{
#ifdef Q_OS_WIN
    return m_running;
#else
    // 与辅助程序的 socket 建起来就算运行中（fd 由 SCM_RIGHTS 递回）
    return m_socket >= 0;
#endif
}

QString TunController::helperPath() const
{
#ifdef Q_OS_WIN
    // Windows 侧没有独立的特权辅助程序
    return QString();
#else
    return QDir(QCoreApplication::applicationDirPath()).filePath(QLatin1String(kHelperName));
#endif
}

bool TunController::helperExists() const
{
#ifdef Q_OS_WIN
    // 没有辅助程序，这一步恒为「不存在」；调用方据此走 Windows 分支
    return false;
#else
    const QFileInfo info(helperPath());
    return info.exists() && info.isExecutable();
#endif
}

bool TunController::hasPermission() const
{
#ifdef Q_OS_WIN
    return isElevated();
#else
    // 与参照实现同款判定：setuid 位 + 属主 root
    struct stat st {};
    if (::stat(QFile::encodeName(helperPath()).constData(), &st) != 0) return false;
    return (st.st_mode & S_ISUID) != 0 && st.st_uid == 0;
#endif
}

bool TunController::grantPermission(QString *error)
{
#ifdef Q_OS_WIN
    // Windows 没有「授权一次、以后免密」的机制：UAC 只能由用户重开进程完成，
    // 进程内无法提升自己，所以这里只给出可执行的指引
    const QString message = QCoreApplication::translate(
        "clashqt::app",
        "TUN mode on Windows requires administrator privileges. "
        "Close the app and start it again with \"Run as administrator\".");
    if (error) *error = message;
    AppLogger::instance().warn(message);
    return false;
#else
    if (!helperExists()) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Helper not found: %1").arg(helperPath());
        return false;
    }

    const QString helper = absolutePath(helperPath());

    // pkexec 会弹出系统密码框；两步都成功才算授权完成
    const QList<QPair<QString, QStringList>> steps = {
        {QStringLiteral("chown"), {QStringLiteral("root:root"), helper}},
        {QStringLiteral("chmod"), {QStringLiteral("+sx"), helper}},
    };

    for (const auto &step : steps) {
        QProcess process;
        process.start(QStringLiteral("pkexec"), QStringList{step.first} + step.second);
        if (!process.waitForStarted(5000)) {
            if (error) *error = QCoreApplication::translate("clashqt::app", "Could not start pkexec: %1").arg(process.errorString());
            return false;
        }
        // 等用户输密码，给足时间；超时说明用户取消了
        if (!process.waitForFinished(90 * 1000)) {
            process.kill();
            process.waitForFinished(2000);
            if (error) *error = QCoreApplication::translate("clashqt::app", "Authorization timed out (the password prompt may have been cancelled)");
            return false;
        }
        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            if (error) {
                *error = QCoreApplication::translate("clashqt::app", "%1 failed (exit code %2): %3")
                             .arg(step.first)
                             .arg(process.exitCode())
                             .arg(QString::fromLocal8Bit(process.readAllStandardError()).trimmed());
            }
            return false;
        }
    }

    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app", "TUN permissions granted (chown root:root + chmod +sx): %1").arg(helper));
    emit stateChanged();
    return true;
#endif
}

bool TunController::start(const TunOptions &options, QString *error)
{
#ifdef Q_OS_WIN
    if (isRunning() || m_pending) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "TUN is starting or already running");
        return false;
    }

    // 失败一律走同一条收尾：撤掉半成品（路由/地址/句柄），不留脏状态
    auto fail = [&](const QString &message) {
        if (error) *error = message;
        AppLogger::instance().error(message);
        stop();
        return false;
    };

    if (!isElevated()) {
        // 与 grantPermission 同一句话：告诉用户该做什么，而不是只报「权限不足」
        return fail(QCoreApplication::translate(
            "clashqt::app",
            "TUN mode on Windows requires administrator privileges. "
            "Close the app and start it again with \"Run as administrator\"."));
    }
    if (!core::wintun::available()) {
        return fail(QCoreApplication::translate(
            "clashqt::app",
            "wintun.dll was not found. Put wintun.dll next to the executable and try again."));
    }

    // ---- 地址：172.19.0.1/30 ----
    const QString cidr    = options.address;
    const int     slash   = cidr.indexOf(QLatin1Char('/'));
    const QString address = (slash > 0 ? cidr.left(slash) : cidr).trimmed();
    const int     prefix  = slash > 0 ? cidr.mid(slash + 1).toInt() : 32;

    quint32 network = 0;
    if (!parseIpv4(address, &network) || prefix <= 0 || prefix > 32) {
        return fail(QCoreApplication::translate("clashqt::app", "Invalid TUN address: %1").arg(cidr));
    }

    // ---- 适配器（同名复用：地址与路由可能还在，下面都按幂等处理）----
    QString adapterError;
    m_adapter = core::wintun::openAdapter(options.device, &adapterError);
    if (!m_adapter) {
        return fail(adapterError.isEmpty()
                        ? QCoreApplication::translate("clashqt::app", "Could not create the TUN adapter")
                        : adapterError);
    }
    m_adapterName = options.device;
    m_luid        = core::wintun::adapterLuid(m_adapter);
    if (!m_luid) return fail(QCoreApplication::translate("clashqt::app", "Could not read the TUN adapter LUID"));

    // ---- 地址 / MTU / 优先级 ----
    // 新建的适配器要等驱动把接口建出来，前几次调用可能还找不到它
    QString configError;
    bool    addressOk = false;
    for (int attempt = 0; attempt < 20; ++attempt) {
        QString attemptError;
        if (setAddress(m_luid, network, prefix, &attemptError)) {
            addressOk = true;
            break;
        }
        configError = attemptError;
        ::Sleep(100);
    }
    if (!addressOk) return fail(configError);

    // 接口优先级压到 1：接管默认路由后，系统按「接口 metric + 路由 metric」排序，
    // 不压的话物理网卡仍会先被选中，TUN 等于没接上
    if (!setInterfaceOptions(m_luid, options.mtu, 1, &configError)) return fail(configError);

    // ---- 路由：只接管 fake-ip 网段，不动系统默认路由 ----
    // 与 Linux 辅助程序一致。改默认路由就必须把每个代理服务器地址逐个旁路，
    // 漏一个就自环；只加一条 fake-ip 路由时，真实 IP 的流量天然走物理网卡。
    m_addedRoutes.clear();
    if (options.autoRoute) {
        const int     separator   = options.fakeIpCidr.indexOf(QLatin1Char('/'));
        const QString fakeNetwork = separator > 0 ? options.fakeIpCidr.left(separator)
                                                  : options.fakeIpCidr;
        const int     fakePrefix  = separator > 0 ? options.fakeIpCidr.mid(separator + 1).toInt() : 0;

        quint32 fake = 0;
        if (!parseIpv4(fakeNetwork, &fake) || fakePrefix <= 0 || fakePrefix > 32) {
            return fail(QCoreApplication::translate("clashqt::app", "Invalid fake-ip CIDR: %1")
                            .arg(options.fakeIpCidr));
        }
        if (!addRoute(m_luid, fake, fakePrefix, 0, &configError)) return fail(configError);
        m_addedRoutes.append({m_luid, fake, fakePrefix, 0});
    }

    // ---- DNS：指向 TUN 地址，查询由内核的 53 劫持应答（fake-ip 的前提）----
    // 适配器刚建出来时别名可能还没注册，与地址一样重试几次
    QString dnsError;
    bool    dnsOk = false;
    for (int attempt = 0; attempt < 20; ++attempt) {
        if (setDnsServers(m_adapterName, address, &dnsError)) {
            dnsOk = true;
            break;
        }
        ::Sleep(100);
    }
    if (!dnsOk) {
        // DNS 未指向 TUN 时域名解析绕过 TUN，接管了路由就不能带着这个洞假装成功
        const QString message = QCoreApplication::translate(
            "clashqt::app", "Could not point system DNS at the TUN address: %1").arg(dnsError);
        if (options.autoRoute) return fail(message);
        AppLogger::instance().warn(message);
    }

    m_dnsAddress = address;
    m_deviceName = m_adapterName;
    m_deviceFd   = qintptr(m_luid);   // 内核按 LUID 打开同一适配器
    m_pending    = false;
    m_running    = true;

    AppLogger::instance().info(
        QCoreApplication::translate("clashqt::app",
                                    "TUN adapter ready: %1 %2/%3, MTU %4, routes %5, Wintun %6")
            .arg(m_adapterName, address)
            .arg(prefix)
            .arg(options.mtu)
            .arg(options.autoRoute
                     ? QCoreApplication::translate("clashqt::app", "taken over")
                     : QCoreApplication::translate("clashqt::app", "not taken over"),
                 core::wintun::versionText()));

    // 排队发 ready：先让 AppContext 的 start() 栈帧返回，再把设备交给内核
    QMetaObject::invokeMethod(this, [this]() {
        if (!m_running) return;   // 期间已被关闭（用户又点了一次开关）
        emit stateChanged();
        emit ready();
    }, Qt::QueuedConnection);
    return true;
#else
    // 真的还在等（辅助程序活着、socket 也还在）才算忙。若状态残留
    // （进程已退出 / socket 已关），就地自愈，避免"一直无法打勾"。
    const bool reallyPending =
        m_pending && m_process && m_process->state() != QProcess::NotRunning;
    if (m_pending && !reallyPending) {
        AppLogger::instance().warn(
            QCoreApplication::translate("clashqt::app", "Found a stale TUN-enabled state; cleaned it up and retried"));
        stop();
    }
    if (isRunning() || reallyPending) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "TUN is starting or already running");
        return false;
    }
    if (!helperExists()) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Helper not found: %1").arg(helperPath());
        return false;
    }
    if (!options.dryRun && !hasPermission()) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "TUN permission not granted yet (enter the password in the prompt that appears)");
        return false;
    }

    // ---- 建监听 socket：辅助程序会连上来，把 TUN 的 fd 交给我们 ----
    // socket 名固定（不掺 pid）：辅助程序把「原默认路由」写在 <socket>.route，
    // 进程被 kill -9 后下次启动还能按同一个路径找到它并还原路由。
    // 应用本身有单实例保护（SingleInstance），固定名不会冲突。
    const QString runtimeDir = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    m_socketPath = QDir(runtimeDir.isEmpty() ? QDir::tempPath() : runtimeDir)
                       .filePath(QStringLiteral(CLASHQT_TUN_HELPER_NAME ".sock"));
    cleanupSocketFile();

    m_listenSocket = ::socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (m_listenSocket < 0) {
        if (error) *error = QString::fromLocal8Bit(std::strerror(errno));
        return false;
    }

    struct sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    const QByteArray pathBytes = QFile::encodeName(m_socketPath);
    if (pathBytes.size() >= int(sizeof(addr.sun_path))) {
        ::close(m_listenSocket);
        m_listenSocket = -1;
        if (error) *error = QCoreApplication::translate("clashqt::app", "Socket path too long");
        return false;
    }
    std::memcpy(addr.sun_path, pathBytes.constData(), size_t(pathBytes.size()));

    // 建 socket 时收紧权限：只允许本人（辅助程序也按属主校验）
    const mode_t oldMask = ::umask(0077);
    const int bindResult = ::bind(m_listenSocket, reinterpret_cast<struct sockaddr *>(&addr),
                                  sizeof(addr));
    ::umask(oldMask);
    if (bindResult != 0 || ::listen(m_listenSocket, 1) != 0) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Failed to listen on the socket: %1")
                                .arg(QString::fromLocal8Bit(std::strerror(errno)));
        cleanupSocketFile();
        return false;
    }

    m_notifier = new QSocketNotifier(m_listenSocket, QSocketNotifier::Read, this);
    QObject::connect(m_notifier, &QSocketNotifier::activated, this,
                     &TunController::onNewConnection);

    // ---- 拉起辅助程序 ----
    m_dryRun = options.dryRun;
    m_process = new QProcess(this);
    m_process->setProgram(helperPath());
    QStringList args = {QStringLiteral("up"), QStringLiteral("--name"), options.device,
                        QStringLiteral("--addr"), options.address,
                        QStringLiteral("--mtu"), QString::number(options.mtu),
                        QStringLiteral("--socket"), m_socketPath};
    if (options.autoRoute) args << QStringLiteral("--auto-route");
    if (!options.fakeIpCidr.isEmpty()) {
        args << QStringLiteral("--fake-ip-cidr") << options.fakeIpCidr;
    }
    for (const QString &address : options.routeExcludes) {
        args << QStringLiteral("--route-exclude") << address;
    }
    if (options.dryRun) args << QStringLiteral("--dry-run");
    m_process->setArguments(args);

    m_process->start();
    if (!m_process->waitForStarted(5000)) {
        if (error) *error = QCoreApplication::translate("clashqt::app", "Could not start the helper: %1").arg(m_process->errorString());
        stop();
        return false;
    }

    // ---- 异步等 fd：绝不在这里跑 processEvents ----
    // 超时兜底：辅助程序可能卡在配地址/改路由上
    m_pending = true;
    m_timeout = new QTimer(this);
    m_timeout->setSingleShot(true);
    m_timeout->setInterval(20000);
    QObject::connect(m_timeout, &QTimer::timeout, this, [this]() {
        failAndCleanup(QCoreApplication::translate("clashqt::app", "Timed out waiting for the helper (20 s)"));
    });
    m_timeout->start();

    // 辅助程序自己先退出（权限不足 / ip 命令失败）时，把 stderr 报给界面。
    // 注意：这条信号可能在 onReadable 的 EOF 分支已经清理过之后才到达，
    // 所以**不能用 m_process**（那时已是空指针）—— 从 sender() 取，
    // 并且一旦发现本控制器已不在等待，直接忽略。
    QObject::connect(m_process, &QProcess::finished, this,
                     [this](int code, QProcess::ExitStatus status) {
                         auto *process = qobject_cast<QProcess *>(sender());
                         if (!process) return;
                         if (!m_pending) return; // 已被清理 / 已就绪：忽略迟到信号
                         const QString stderrText =
                             QString::fromLocal8Bit(process->readAllStandardError()).trimmed();
                         failAndCleanup(stderrText.isEmpty()
                                            ? QCoreApplication::translate("clashqt::app", "The helper exited unexpectedly (exit code %1)")
                                                  .arg(status == QProcess::CrashExit ? -1 : code)
                                            : stderrText);
                     });

    AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "Started the TUN helper; waiting for the device: %1")
                                   .arg(options.device));
    AppLogger::instance().debug(QCoreApplication::translate("clashqt::app", "TUN handshake details: socket=%1 autoRoute=%2 excludes=%3 cmd=%4 %5")
                                    .arg(m_socketPath)
                                    .arg(options.autoRoute ? 1 : 0)
                                    .arg(options.routeExcludes.join(QLatin1Char(',')))
                                    .arg(helperPath(), args.join(QLatin1Char(' '))));
    emit stateChanged();
    return true;
#endif
}

void TunController::failAndCleanup(const QString &reason)
{
#ifdef Q_OS_WIN
    Q_UNUSED(reason)
    stop();
#else
    if (m_cleaning) return; // 幂等：EOF / finished / 超时 可能同时来
    m_cleaning = true;

    if (m_timeout) {
        m_timeout->stop();
        m_timeout->deleteLater();
        m_timeout = nullptr;
    }
    const bool wasPending = m_pending;
    m_pending             = false;

    stop(); // 幂等：清 socket / 进程 / fd
    m_cleaning = false;
    if (wasPending || !reason.isEmpty()) emit failed(reason);
#endif
}

void TunController::onNewConnection()
{
#ifdef Q_OS_WIN
    // Windows 没有辅助程序 socket 这条链路，永不触发
#else
    // 必须用 accept4 + SOCK_CLOEXEC：普通 accept 建的 fd 不带 CLOEXEC，
    // 会被随后启动的辅助程序继承一份。那样即使本进程崩溃，辅助程序也收不到
    // EOF，于是永不退出、一直占着 TUN 接口 —— 下次启用就 EBUSY
    // （"Device or resource busy"）。
    const int socket = ::accept4(m_listenSocket, nullptr, nullptr, SOCK_CLOEXEC);
    if (socket < 0) return;

    m_socket = socket;

    // 监听 socket 已无用，收掉
    if (m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    ::close(m_listenSocket);
    m_listenSocket = -1;

    // 用原始 recvmsg 收 fd：这里不能让 Qt 先把数据读进它自己的缓冲
    m_notifier = new QSocketNotifier(m_socket, QSocketNotifier::Read, this);
    QObject::connect(m_notifier, &QSocketNotifier::activated, this, &TunController::onReadable);
#endif
}

void TunController::onReadable()
{
#ifdef Q_OS_WIN
    // 同上：Windows 侧没有 fd 传递链路
#else
    char           payload[512] = {};
    char           control[CMSG_SPACE(sizeof(int))] = {};
    struct iovec   iov {};
    struct msghdr  msg {};

    iov.iov_base = payload;
    iov.iov_len  = sizeof(payload) - 1;

    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = control;
    msg.msg_controllen = sizeof(control);

    const ssize_t received = (m_socket >= 0) ? ::recvmsg(m_socket, &msg, 0) : -1;
    if (received <= 0) {
        failAndCleanup(QCoreApplication::translate("clashqt::app", "The TUN helper disconnected"));
        return;
    }

    // 先看有没有带 fd 的控制消息
    for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS
            && m_deviceFd < 0) {
            // 一次可能收到多个 fd：TUN 设备、DNS(UDP)、DNS(TCP)
            const int count = int((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int));
            for (int i = 0; i < count; ++i) {
                int fd = -1;
                std::memcpy(&fd, reinterpret_cast<char *>(CMSG_DATA(cmsg)) + i * sizeof(int),
                            sizeof(int));
                if (fd < 0) continue;
                // 收到的 fd 默认不带 CLOEXEC：若之后再次拉起子进程会被继承，
                // 造成「应用已退出、接口仍被占着」→ 下次 EBUSY。这里显式关上。
                ::fcntl(fd, F_SETFD, FD_CLOEXEC);
                qintptr *slot = (i == 0) ? &m_deviceFd : (i == 1 ? &m_dnsUdpFd : &m_dnsTcpFd);
                if (*slot >= 0) ::close(int(*slot));
                *slot = qintptr(fd);
            }
        }
    }

    m_pendingHeader += QString::fromLatin1(payload, int(received));

    // 主机序的 header 是一行文本：解析出设备名与是否 dry-run
    const int newline = m_pendingHeader.indexOf(QLatin1Char('\n'));
    if (newline < 0) return;

    const QString header = m_pendingHeader.left(newline);
    m_pendingHeader.remove(0, newline + 1);

    for (const QString &token : header.split(QLatin1Char(' '), Qt::SkipEmptyParts)) {
        const int equals = token.indexOf(QLatin1Char('='));
        if (equals <= 0) continue;
        const QString key   = token.left(equals);
        const QString value = token.mid(equals + 1);
        if (key == QLatin1String("device")) m_deviceName = value;
        else if (key == QLatin1String("mode")) m_dryRun = (value == QLatin1String("dry-run"));
    }

    // 只有 fd 真的到手才确认，否则让辅助程序回滚（它会删掉刚建的接口）
    const char *reply = (m_deviceFd >= 0) ? "ok\n" : "err no-fd\n";
    const ssize_t written = ::write(m_socket, reply, std::strlen(reply));
    Q_UNUSED(written);

    if (m_deviceFd < 0) {
        failAndCleanup(QCoreApplication::translate("clashqt::app", "The helper did not send a TUN fd"));
        return;
    }

    if (m_timeout) {
        m_timeout->stop();
        m_timeout->deleteLater();
        m_timeout = nullptr;
    }
    m_pending = false;

    AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "TUN ready: %1 (%2)")
                                   .arg(m_deviceName, m_dryRun ? QStringLiteral("dry-run")
                                                               : QCoreApplication::translate("clashqt::app", "real device")));
    emit stateChanged();
    emit ready();
#endif
}

void TunController::closeFd()
{
#ifdef Q_OS_WIN
    // Windows 侧没有通过 fd 传递的设备
#else
    for (qintptr *slot : {&m_deviceFd, &m_dnsUdpFd, &m_dnsTcpFd}) {
        if (*slot >= 0) {
            ::close(int(*slot));
            *slot = -1;
        }
    }
#endif
}

void TunController::cleanupSocketFile()
{
#ifdef Q_OS_WIN
    // Windows 侧不建 socket 文件
#else
    if (!m_socketPath.isEmpty()) QFile::remove(m_socketPath);
#endif
}

void TunController::stop()
{
#ifdef Q_OS_WIN
    // 顺序：路由 → DNS → 适配器句柄。反过来会有一瞬间「默认路由还指着已经没有
    // 出口的适配器」，表现为短暂断网
    for (const RouteEntry &route : m_addedRoutes) {
        deleteRoute(route.luid, route.network, route.prefix, route.nextHop);
    }
    m_addedRoutes.clear();

    clearDnsServers(m_adapterName);
    if (m_adapter) core::wintun::closeAdapter(m_adapter);
    m_adapter = 0;
    m_luid    = 0;
    m_adapterName.clear();
    m_dnsAddress.clear();
    m_deviceFd = -1;

    const bool wasRunning = m_running;
    m_running = false;
    m_pending = false;
    m_deviceName.clear();

    if (wasRunning) {
        AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "TUN is closed"));
        emit stateChanged();
    }
#else
    const bool wasRunning = isRunning();

    if (m_timeout) {
        m_timeout->stop();
        m_timeout->deleteLater();
        m_timeout = nullptr;
    }
    m_pending = false;

    if (m_socket >= 0) {
        // 先客客气气通知辅助程序（它会删掉接口），再断开
        const char *command = "down\n";
        const ssize_t written = ::write(m_socket, command, std::strlen(command));
        Q_UNUSED(written);
    }

    if (m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }

    if (m_socket >= 0) {
        ::close(m_socket);
        m_socket = -1;
    }
    if (m_listenSocket >= 0) {
        ::close(m_listenSocket);
        m_listenSocket = -1;
    }

    if (m_process) {
        // 先断信号：否则 finished 会在清理过程中回调进来（正是之前段错误的成因）
        QObject::disconnect(m_process, nullptr, this, nullptr);
        if (m_process->state() != QProcess::NotRunning) {
            // 已发过 down，正常情况下几百毫秒内就退出；短等一次，不行就 terminate/kill
            if (!m_process->waitForFinished(500)) {
                m_process->terminate();
                if (!m_process->waitForFinished(800)) m_process->kill();
            }
        }
        m_process->deleteLater();
        m_process = nullptr;
    }

    closeFd();
    cleanupSocketFile();

    if (wasRunning) {
        AppLogger::instance().info(QCoreApplication::translate("clashqt::app", "TUN is closed"));
        emit stateChanged();
    }
    m_deviceName.clear();
    m_dryRun = false;
#endif
}

} // namespace clashqt
