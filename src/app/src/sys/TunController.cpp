#include "sys/TunController.h"

#include "utils/AppLogger.h"

#include <QCoreApplication>
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

namespace clashqt {
namespace {

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
} // namespace

TunController::TunController(QObject *parent) : QObject(parent) {}

TunController::~TunController() { stop(); }

QString TunController::helperPath() const
{
    return QDir(QCoreApplication::applicationDirPath()).filePath(QLatin1String(kHelperName));
}

bool TunController::helperExists() const
{
    const QFileInfo info(helperPath());
    return info.exists() && info.isExecutable();
}

bool TunController::hasPermission() const
{
    // 与参照实现同款判定：setuid 位 + 属主 root
    struct stat st {};
    if (::stat(QFile::encodeName(helperPath()).constData(), &st) != 0) return false;
    return (st.st_mode & S_ISUID) != 0 && st.st_uid == 0;
}

bool TunController::grantPermission(QString *error)
{
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
}

bool TunController::start(const TunOptions &options, QString *error)
{
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
}

void TunController::failAndCleanup(const QString &reason)
{
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
}

void TunController::onNewConnection()
{
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
}

void TunController::onReadable()
{
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
                int *slot = (i == 0) ? &m_deviceFd : (i == 1 ? &m_dnsUdpFd : &m_dnsTcpFd);
                if (*slot >= 0) ::close(*slot);
                *slot = fd;
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
}

void TunController::closeFd()
{
    for (int *slot : {&m_deviceFd, &m_dnsUdpFd, &m_dnsTcpFd}) {
        if (*slot >= 0) {
            ::close(*slot);
            *slot = -1;
        }
    }
}

void TunController::cleanupSocketFile()
{
    if (!m_socketPath.isEmpty()) QFile::remove(m_socketPath);
}

void TunController::stop()
{
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
}

} // namespace clashqt
