// ---------------------------------------------------------------------------
//  <app>-tun —— TUN 模式的「特权辅助程序」（名字由 CMake 派生）
//
//  为什么单独一个程序
//  ---------------------------------------------------------------------------
//  建 TUN 设备、配地址 / MTU、改路由都需要 CAP_NET_ADMIN。参照实现
//  （clash-party）在 Linux 上的做法是把**内核二进制** chown root:root +
//  chmod +sx 做成 setuid，让它以 root 建 TUN（见
//  clash-party/src/main/core/permissions.ts:grantTunPermissions）。权限检查
//  同样简单：stat 一下看 setuid 位与 uid==0。
//
//  本项目的内核是进程内静态库，没法只给「内核」加 setuid；而把整个 Qt 图形
//  程序做成 setuid root 是明确要避免的（GUI 与全部业务都以 root 跑，配置
//  文件属主也会被打乱）。因此把这个思路搬成：只把「必须 root 的那几件事」
//  放进这个极小、零依赖、可审计的程序：
//
//    * up     建 TUN、配地址 / MTU / 置 up；若给了 --auto-route 则接管默认
//             路由（并把 --route-exclude 指定的地址（通常是代理服务器）
//             钉回原网关，避免出站流量绕回 TUN 形成环路）；随后把 TUN 的
//             fd 通过 unix socket 以 SCM_RIGHTS 交给主程序（fd 自带权限，
//             主程序无需提权即可收发包），并常驻。
//             主程序断开 / 发来 down / 收到信号 → 还原路由、删接口再退出。
//    * down   兜底清理（上次异常退出遗留的接口）。
//    * restore 按状态文件还原路由（进程被 kill -9 后由主程序下次启动时调用）。
//    * status 打印状态，供主程序与排查使用。
//
//  路由接管的安全性
//  ---------------------------------------------------------------------------
//  改默认路由是「断网级」操作，因此：
//    · 动之前先把原始默认路由写进状态文件，任何失败都按它还原；
//    · 必须先给代理服务器地址加好旁路路由，再动默认路由（顺序不可颠倒）；
//    · --dry-run 只打印将要执行的 ip 命令，不执行，便于无 root 校验。
//
//  数据面（用户态协议栈）在**主程序**里 —— 本程序只做特权动作，不碰流量，
//  也不解析任何网络报文。
//
//  安全边界
//  ---------------------------------------------------------------------------
//  · 参数全部严格校验（接口名、IPv4/CIDR、MTU 范围），不使用 shell，
//    `ip` 用 execv 数组形式调用，杜绝拼接注入；
//  · setuid 后 real uid 仍是调用者，因此要求「socket 必须属于调用者本人」，
//    别的用户没法指挥这个 root 辅助程序；
//  · 路由接管（auto-route）尚未实现，参数被显式拒绝，避免在数据面就绪前
//    把整机默认路由改到一条没人转发的隧道上（那会直接断网）。
// ---------------------------------------------------------------------------

#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <poll.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace {

constexpr int  kProtocolVersion = 1;
constexpr int  kDefaultMtu      = 1500;
volatile sig_atomic_t g_stop    = 0;

void onSignal(int) { g_stop = 1; }

void logLine(const std::string &text)
{
    std::fprintf(stderr, "[" CLASHQT_TUN_HELPER_NAME "] %s\n", text.c_str());
}

// ---- 参数校验 -------------------------------------------------------------

bool validDeviceName(const std::string &name)
{
    if (name.empty() || name.size() > 15) return false; // IFNAMSIZ-1
    if (name == "." || name == "..") return false;
    for (char c : name) {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                        || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.';
        if (!ok) return false;
    }
    return true;
}

bool validIpv4Cidr(const std::string &cidr)
{
    const auto slash = cidr.find('/');
    if (slash == std::string::npos) return false;

    in_addr addr{};
    if (inet_pton(AF_INET, cidr.substr(0, slash).c_str(), &addr) != 1) return false;

    const std::string len = cidr.substr(slash + 1);
    if (len.empty() || len.size() > 2) return false;
    const int prefix = std::atoi(len.c_str());
    return prefix >= 1 && prefix <= 32;
}

bool validMtu(int mtu) { return mtu >= 68 && mtu <= 65535; }

// ---- 执行 ip 命令（execv 数组形式，不经 shell）-----------------------------

bool runIp(const std::vector<std::string> &args, std::string *error)
{
    static const char *kIpCandidates[] = {"/usr/bin/ip", "/sbin/ip", "/bin/ip"};

    const char *ipPath = nullptr;
    for (const char *candidate : kIpCandidates) {
        if (access(candidate, X_OK) == 0) {
            ipPath = candidate;
            break;
        }
    }
    if (!ipPath) {
        *error = "找不到 ip 命令（需要 iproute2）";
        return false;
    }

    std::vector<char *> argv;
    argv.push_back(const_cast<char *>(ipPath));
    for (const std::string &arg : args) argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(nullptr);

    const pid_t pid = fork();
    if (pid < 0) {
        *error = std::string("fork 失败：") + std::strerror(errno);
        return false;
    }
    if (pid == 0) {
        execv(ipPath, argv.data());
        _exit(127); // exec 失败
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        *error = std::string("等待 ip 结束失败：") + std::strerror(errno);
        return false;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        std::string joined;
        for (const std::string &arg : args) {
            joined += arg;
            joined += ' ';
        }
        *error = "ip " + joined + "失败（退出码 " + std::to_string(WEXITSTATUS(status)) + "）";
        return false;
    }
    return true;
}

// ---- TUN 设备 -------------------------------------------------------------

// 接口是否已存在且是 TUN（点对点、无 ARP）
bool looksLikeTun(const std::string &name)
{
    int pipeFds[2] = {-1, -1};
    if (pipe(pipeFds) != 0) return false;

    const pid_t pid = fork();
    if (pid == 0) {
        dup2(pipeFds[1], STDOUT_FILENO);
        close(pipeFds[0]);
        close(pipeFds[1]);
        const char *argv[] = {"/usr/bin/ip", "-o", "link", "show", name.c_str(), nullptr};
        execv(argv[0], const_cast<char **>(argv));
        const char *argv2[] = {"/sbin/ip", "-o", "link", "show", name.c_str(), nullptr};
        execv(argv2[0], const_cast<char **>(argv2));
        _exit(127);
    }
    close(pipeFds[1]);
    if (pid < 0) {
        close(pipeFds[0]);
        return false;
    }

    std::string output;
    char        buffer[512];
    ssize_t     n = 0;
    while ((n = read(pipeFds[0], buffer, sizeof(buffer))) > 0) output.append(buffer, size_t(n));
    close(pipeFds[0]);
    int status = 0;
    waitpid(pid, &status, 0);

    return output.find("POINTOPOINT") != std::string::npos;
}

// 建 TUN 并返回 fd。失败时 error 带原因。
//
// 注意 EBUSY：它表示**同名接口已存在**（不是权限问题）——通常是上次异常退出
// 留下的残留。这里先确认它确实是 TUN 再删掉重试一次；删非 TUN 同名接口太危险。
int createTunDevice(const std::string &name, std::string *error)
{
    auto tryAttach = [&name, error]() -> int {
        const int fd = ::open("/dev/net/tun", O_RDWR | O_CLOEXEC);
        if (fd < 0) {
            *error = std::string("打开 /dev/net/tun 失败：") + std::strerror(errno)
                     + "（需要内核 tun 模块）";
            return -1;
        }

        struct ifreq ifr {};
        ifr.ifr_flags = IFF_TUN | IFF_NO_PI; // 只要 IP 包，不要 4 字节包头
        std::snprintf(ifr.ifr_name, IFNAMSIZ, "%s", name.c_str());

        if (ioctl(fd, TUNSETIFF, &ifr) < 0) {
            const int saved = errno;
            ::close(fd);
            if (saved == EPERM || saved == EACCES) {
                *error = std::string("创建 TUN 接口失败：") + std::strerror(saved)
                         + "（需要 root 或 CAP_NET_ADMIN；请先在界面里完成授权）";
            } else if (saved == EBUSY) {
                *error = "同名接口已存在且仍被占用（EBUSY）";
            } else {
                *error = std::string("创建 TUN 接口失败：") + std::strerror(saved);
            }
            errno = saved;
            return -1;
        }
        return fd;
    };

    // 成功路径统一在这里补 O_NONBLOCK（主程序用事件驱动读写，必须非阻塞）
    auto prepare = [](int fd) -> int {
        if (fd >= 0) {
            const int flags = fcntl(fd, F_GETFL, 0);
            if (flags >= 0) fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
        return fd;
    };

    int fd = tryAttach();
    if (fd >= 0) return prepare(fd);

    // EBUSY：清掉残留接口后重试一次
    if (errno == EBUSY) {
        if (looksLikeTun(name)) {
            logLine("发现残留的同名 TUN 接口 " + name + "，先删除再重建");
            std::string ignored;
            runIp({"link", "del", "dev", name}, &ignored);
            fd = tryAttach();
            if (fd >= 0) {
                logLine("残留接口已回收，TUN 重建成功");
                return prepare(fd);
            }
        } else {
            *error = "同名接口 " + name + " 已存在且不是本程序的 TUN，请改名后重试";
        }
    }

    return prepare(fd); // 失败时为 -1
}

// ---- DNS 端口接管 ---------------------------------------------------------
//  为什么需要这一手：systemd-resolved 的上游 socket 绑在物理网卡上
//  （SO_BINDTODEVICE），会绕过 TUN 的默认路由 —— 结果应用拿到的是真实 IP，
//  既没有域名（域名规则失效），又带真实 AAAA（IPv6 包进 TUN 被丢弃）。
//  做法与 clash-verge 等一致：在 TUN 自己的地址上绑 53，交给主程序应答，
//  再把系统 DNS 指向这个地址。绑 53 需要特权，所以在这里做。
int bindDnsSocket(const std::string &address, bool tcp, std::string *error)
{
    in_addr addr {};
    if (inet_pton(AF_INET, address.c_str(), &addr) != 1) {
        *error = "DNS 绑定地址不合法：" + address;
        return -1;
    }

    const int type = tcp ? SOCK_STREAM : SOCK_DGRAM;
    const int fd = ::socket(AF_INET, type | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (fd < 0) {
        *error = std::string("创建 DNS socket 失败：") + std::strerror(errno);
        return -1;
    }

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in bindAddr {};
    bindAddr.sin_family      = AF_INET;
    bindAddr.sin_addr        = addr;
    bindAddr.sin_port        = htons(53);
    if (::bind(fd, reinterpret_cast<struct sockaddr *>(&bindAddr), sizeof(bindAddr)) != 0) {
        *error = std::string("绑定 ") + address + ":53 失败：" + std::strerror(errno);
        ::close(fd);
        return -1;
    }
    if (tcp && ::listen(fd, 64) != 0) {
        *error = std::string("监听 ") + address + ":53 失败：" + std::strerror(errno);
        ::close(fd);
        return -1;
    }
    return fd;
}

// ---- 改写 /etc/resolv.conf（没有 systemd-resolved / resolvectl 的场景）----
//  很多发行版（如 Deepin）由 NetworkManager 直接写 resolv.conf，DNS 指向路由器。
//  路由器在本地网段，路由表里有更精确的直连路由 —— DNS 查询会**绕过 TUN**，
//  于是应用拿到真实 IP：既没有域名（域名规则失效），又带回真实 AAAA（IPv6 被丢弃）。
//  所以要在这里把 resolv.conf 指到 TUN 地址，退出时还原。
constexpr const char *kResolvConf        = "/etc/resolv.conf";
constexpr const char *kResolvConfBackup  = "/run/" CLASHQT_TUN_HELPER_NAME "-resolv.conf.bak";

bool writeFile(const std::string &path, const std::string &content, std::string *error)
{
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd < 0) {
        *error = "写 " + path + " 失败：" + std::strerror(errno);
        return false;
    }
    ssize_t written = ::write(fd, content.data(), content.size());
    ::close(fd);
    if (written != ssize_t(content.size())) {
        *error = "写 " + path + " 不完整";
        return false;
    }
    return true;
}

bool readFile(const std::string &path, std::string *content)
{
    const int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return false;
    char    buffer[4096];
    ssize_t n = 0;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) content->append(buffer, size_t(n));
    ::close(fd);
    return true;
}

void restoreSystemDnsFile();

void pointSystemDnsFile(const std::string &address)
{
    // 自愈：若存在旧备份，说明上次异常退出没还原（此时 resolv.conf 可能还指向
    // 一个已消失的 TUN 地址，等于没有 DNS）。先还原，再重新接管。
    if (access(kResolvConfBackup, F_OK) == 0) {
        logLine("发现上次遗留的 resolv.conf 备份，先还原再接管");
        restoreSystemDnsFile();
    }
    if (access(kResolvConfBackup, F_OK) != 0) { // 只在没有旧备份时备份
        std::string original;
        if (readFile(kResolvConf, &original)) {
            std::string error;
            if (!writeFile(kResolvConfBackup, original, &error)) {
                logLine("备份 resolv.conf 失败：" + error);
                return;
            }
        }
    }
    std::string error;
    const std::string content =
        "# Generated by " CLASHQT_TUN_HELPER_NAME
        " (TUN 模式接管 DNS；关闭 TUN 时自动还原)\n"
        "nameserver " + address + "\n";
    if (!writeFile(kResolvConf, content, &error)) {
        logLine("改写 resolv.conf 失败：" + error);
        return;
    }
    logLine("系统 DNS 已指向 " + address + "（改写 /etc/resolv.conf）");
}

void restoreSystemDnsFile()
{
    if (access(kResolvConfBackup, F_OK) != 0) return;
    std::string original;
    if (readFile(kResolvConfBackup, &original)) {
        std::string error;
        if (writeFile(kResolvConf, original, &error)) {
            logLine("resolv.conf 已还原");
        } else {
            logLine("还原 resolv.conf 失败：" + error);
        }
    }
    ::unlink(kResolvConfBackup);
}

// 把系统 DNS 指向 TUN 地址（失败不影响 TUN 本身，只是拿不到域名）
void pointSystemDns(const std::string &device, const std::string &address)
{
    if (access("/usr/bin/resolvectl", X_OK) != 0 && access("/bin/resolvectl", X_OK) != 0) {
        // 没有 systemd-resolved：直接改写 resolv.conf（Deepin / NetworkManager 场景）
        pointSystemDnsFile(address);
        return;
    }
    std::string error;
    // resolvectl dns <dev> <addr> && resolvectl domain <dev> ~.
    const pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/resolvectl", "resolvectl", "dns", device.c_str(), address.c_str(),
              static_cast<char *>(nullptr));
        execl("/bin/resolvectl", "resolvectl", "dns", device.c_str(), address.c_str(),
              static_cast<char *>(nullptr));
        _exit(127);
    }
    if (pid > 0) {
        int status = 0;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            logLine("resolvectl dns 失败（系统 DNS 未指向 TUN）");
            return;
        }
    }
    const pid_t pid2 = fork();
    if (pid2 == 0) {
        execl("/usr/bin/resolvectl", "resolvectl", "domain", device.c_str(), "~.",
              static_cast<char *>(nullptr));
        execl("/bin/resolvectl", "resolvectl", "domain", device.c_str(), "~.",
              static_cast<char *>(nullptr));
        _exit(127);
    }
    if (pid2 > 0) {
        int status = 0;
        waitpid(pid2, &status, 0);
    }
    logLine("系统 DNS 已指向 " + address + "（接口 " + device + "）");
}

void revertSystemDns(const std::string &device)
{
    restoreSystemDnsFile(); // 无论走哪条路，都尝试还原 resolv.conf
    if (access("/usr/bin/resolvectl", X_OK) != 0 && access("/bin/resolvectl", X_OK) != 0) return;

    const pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/resolvectl", "resolvectl", "revert", device.c_str(),
              static_cast<char *>(nullptr));
        execl("/bin/resolvectl", "resolvectl", "revert", device.c_str(),
              static_cast<char *>(nullptr));
        _exit(127);
    }
    if (pid > 0) {
        int status = 0;
        waitpid(pid, &status, 0);
    }
}

// ---- unix socket：把 fd 交给主程序 ----------------------------------------

int connectToMainApp(const std::string &path, std::string *error)
{
    const int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (sock < 0) {
        *error = std::string("创建 socket 失败：") + std::strerror(errno);
        return -1;
    }

    struct sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    if (path.size() >= sizeof(addr.sun_path)) {
        *error = "socket 路径过长";
        ::close(sock);
        return -1;
    }
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str());

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        *error = std::string("连接主程序 socket 失败（") + path + "）："
                 + std::strerror(errno);
        ::close(sock);
        return -1;
    }
    return sock;
}

// socket 必须属于调用者本人（setuid 后 real uid 仍是用户）：
// 否则别的用户就能指挥这个 root 辅助程序
bool socketOwnedByCaller(const std::string &path, std::string *error)
{
    struct stat st {};
    if (lstat(path.c_str(), &st) < 0) {
        *error = std::string("stat socket 失败：") + std::strerror(errno);
        return false;
    }
    if (!S_ISSOCK(st.st_mode)) {
        *error = "目标不是 socket";
        return false;
    }
    if (st.st_uid != getuid()) {
        *error = "socket 不属于当前用户，拒绝执行";
        return false;
    }
    return true;
}

bool sendFdsWithHeader(int sock, const std::vector<int> &fds, const std::string &header,
                       std::string *error)
{
    struct iovec iov {};
    iov.iov_base = const_cast<char *>(header.data());
    iov.iov_len  = header.size();

    char control[CMSG_SPACE(sizeof(int) * 4)] = {};
    struct msghdr msg {};
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = control;
    msg.msg_controllen = sizeof(control);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type  = SCM_RIGHTS;
    cmsg->cmsg_len   = CMSG_LEN(sizeof(int) * fds.size());
    for (size_t i = 0; i < fds.size(); ++i) {
        std::memcpy(reinterpret_cast<char *>(CMSG_DATA(cmsg)) + i * sizeof(int), &fds[i],
                    sizeof(int));
    }

    ssize_t sent = 0;
    do {
        sent = sendmsg(sock, &msg, MSG_NOSIGNAL);
    } while (sent < 0 && errno == EINTR);

    if (sent < 0) {
        *error = std::string("发送 fd 失败：") + std::strerror(errno);
        return false;
    }
    return true;
}

// 等主程序的确认（ok / err ...）
bool waitForAck(int sock, std::string *error)
{
    struct pollfd pfd { sock, POLLIN, 0 };
    if (poll(&pfd, 1, 5000) <= 0) {
        *error = "等待主程序确认超时";
        return false;
    }
    char buf[128] = {};
    const ssize_t n = read(sock, buf, sizeof(buf) - 1);
    if (n <= 0) {
        *error = "主程序在确认前断开";
        return false;
    }
    const std::string reply(buf, size_t(n));
    if (reply.rfind("ok", 0) != 0) {
        *error = "主程序拒绝了转发： " + reply;
        return false;
    }
    return true;
}

// ---- 默认路由与状态文件 ---------------------------------------------------

struct DefaultRoute {
    std::string gateway;  // 可能为空（点对点链路）
    std::string device;
    int         metric = 0;
    // 为防环路而额外添加的私网网段（逗号分隔），还原时要精确删除它们
    std::string bypass;
    // TUN 地址：策略路由按这个源地址把「本机出站」导回物理网卡
    std::string tunAddr;
    bool        valid  = false;
};

// 解析 `ip -o route show default` 的第一行
DefaultRoute readDefaultRoute()
{
    DefaultRoute route;

    int pipeFds[2] = {-1, -1};
    if (pipe(pipeFds) != 0) return route;

    const pid_t pid = fork();
    if (pid == 0) {
        dup2(pipeFds[1], STDOUT_FILENO);
        close(pipeFds[0]);
        close(pipeFds[1]);
        const char *argv[] = {"/usr/bin/ip", "-o", "route", "show", "default", nullptr};
        execv(argv[0], const_cast<char **>(argv));
        const char *argv2[] = {"/sbin/ip", "-o", "route", "show", "default", nullptr};
        execv(argv2[0], const_cast<char **>(argv2));
        _exit(127);
    }
    close(pipeFds[1]);
    if (pid < 0) {
        close(pipeFds[0]);
        return route;
    }

    std::string output;
    char        buffer[1024];
    ssize_t     n = 0;
    while ((n = read(pipeFds[0], buffer, sizeof(buffer))) > 0) output.append(buffer, size_t(n));
    close(pipeFds[0]);
    int status = 0;
    waitpid(pid, &status, 0);

    // 形如：default via 192.168.1.1 dev eth0 proto dhcp metric 600
    std::vector<std::string> tokens;
    std::string              current;
    for (char c : output) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) tokens.push_back(current);

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "via" && i + 1 < tokens.size()) route.gateway = tokens[i + 1];
        else if (tokens[i] == "dev" && i + 1 < tokens.size()) route.device = tokens[i + 1];
        else if (tokens[i] == "metric" && i + 1 < tokens.size()) route.metric = std::atoi(tokens[i + 1].c_str());
    }
    route.valid = !route.device.empty();
    return route;
}

std::string stateFilePath(const std::string &socketPath) { return socketPath + ".route"; }

void saveRouteState(const std::string &socketPath, const DefaultRoute &route)
{
    const int fd = ::open(stateFilePath(socketPath).c_str(),
                          O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
    if (fd < 0) return;
    const std::string line = "gateway=" + route.gateway + "\ndevice=" + route.device
                             + "\nmetric=" + std::to_string(route.metric)
                             + "\nbypass=" + route.bypass
                             + "\ntunaddr=" + route.tunAddr + "\n";
    ssize_t ignored = write(fd, line.data(), line.size());
    (void)ignored;
    ::close(fd);
}

DefaultRoute loadRouteState(const std::string &socketPath)
{
    DefaultRoute route;
    const int    fd = ::open(stateFilePath(socketPath).c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return route;

    std::string content;
    char        buffer[512];
    ssize_t     n = 0;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) content.append(buffer, size_t(n));
    ::close(fd);

    size_t pos = 0;
    while (pos < content.size()) {
        const size_t end  = content.find('\n', pos);
        const std::string line = content.substr(pos, end == std::string::npos ? std::string::npos
                                                                             : end - pos);
        const size_t equals = line.find('=');
        if (equals != std::string::npos) {
            const std::string key   = line.substr(0, equals);
            const std::string value = line.substr(equals + 1);
            if (key == "gateway") route.gateway = value;
            else if (key == "device") route.device = value;
            else if (key == "metric") route.metric = std::atoi(value.c_str());
            else if (key == "bypass") route.bypass = value;
            else if (key == "tunaddr") route.tunAddr = value;
        }
        if (end == std::string::npos) break;
        pos = end + 1;
    }
    route.valid = !route.device.empty();
    return route;
}

// 打印（dry-run）或执行一条 ip 命令
bool applyRouteCommand(const std::vector<std::string> &args, bool dryRun, std::string *error)
{
    std::string printable;
    for (const std::string &arg : args) {
        printable += arg;
        printable += ' ';
    }
    if (dryRun) {
        std::fprintf(stderr, "[dry-run] ip %s\n", printable.c_str());
        return true;
    }
    logLine("ip " + printable);
    return runIp(args, error);
}

// 接管路由：**只把 fake-ip 网段指到 TUN**。
//
// 这里刻意不改系统默认路由，也不加策略路由 —— 这两条路都试过并翻车：
//   · 抢到默认路由后，本机自身出站（连代理节点 / DIRECT 直连 / DNS 上游）会被
//     带回 TUN，形成自环：8 秒超时、「Unknown error」，期间整个数据面被这条
//     拨号堵死，用户感受就是「网页时好时坏」；
//   · 再用「按源地址」的策略路由去救，会把客户端发往假 IP 的包也导去物理网卡，
//     TUN 一个 TCP 都收不到（实测：DNS 正常、零连接）。
// 只把假 IP 网段指到 TUN，既覆盖全部域名流量，又不与 NetworkManager 抢路由。
bool takeoverRoutes(const std::string &device, const std::vector<std::string> &excludes,
                    const std::string &socketPath, const std::string &fakeIpCidr,
                    const std::string &tunAddress, bool dryRun, std::string *error)
{
    (void)excludes;
    (void)socketPath;
    (void)tunAddress;

    if (fakeIpCidr.empty()) {
        *error = "缺少 fake-ip 网段，无法接管流量";
        return false;
    }
    logLine("仅接管假 IP 网段：" + fakeIpCidr + " → " + device + "（不改系统默认路由）");
    return applyRouteCommand({"route", "replace", fakeIpCidr, "dev", device}, dryRun, error);
}

// 关闭时：假 IP 网段的路由随接口删除一起消失，这里只清状态文件。
void restoreRoutes(const std::string &socketPath, bool dryRun)
{
    if (!dryRun) ::unlink(stateFilePath(socketPath).c_str());
}

std::string jsonEscape(const std::string &text)
{
    std::string out;
    for (char c : text) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}

// ---- 子命令 ---------------------------------------------------------------

int commandStatus()
{
    const bool isRoot = (geteuid() == 0);
    const bool devOk  = (access("/dev/net/tun", R_OK | W_OK) == 0);

    std::printf("{\"protocol\":%d,\"effectiveUid\":%d,\"root\":%s,\"tunDevice\":%s,"
                "\"canCreateTun\":%s}\n",
                kProtocolVersion, int(geteuid()), isRoot ? "true" : "false",
                devOk ? "true" : "false", (isRoot && devOk) ? "true" : "false");
    return 0;
}

int commandDown(const std::string &device)
{
    if (!validDeviceName(device)) {
        std::fprintf(stderr, "接口名不合法\n");
        return 2;
    }
    std::string error;
    // 不存在时会失败，直接忽略即可（兜底清理）
    runIp({"link", "del", "dev", device}, &error);
    std::printf("{\"down\":true,\"device\":\"%s\"}\n", device.c_str());
    return 0;
}

struct UpOptions {
    std::string              device;
    std::string              address;
    std::string              socketPath;
    int                      mtu      = kDefaultMtu;
    bool                     dryRun   = false;
    bool                     autoRoute = false;
    std::vector<std::string> excludes; // --route-exclude（通常是代理服务器地址）
    bool routesOnly = false;           // 只做路由接管，不建 TUN（调试 / 自检用）
    std::string fakeIpCidr = "198.18.0.0/15"; // 假 IP 网段：给它单独加一条路由
};

int commandUp(const UpOptions &opts)
{
    if (!validDeviceName(opts.device)) {
        std::fprintf(stderr, "接口名不合法：%s\n", opts.device.c_str());
        return 2;
    }
    if (!validIpv4Cidr(opts.address)) {
        std::fprintf(stderr, "地址不合法（需要 a.b.c.d/nn）：%s\n", opts.address.c_str());
        return 2;
    }
    if (!validMtu(opts.mtu)) {
        std::fprintf(stderr, "MTU 超出范围：%d\n", opts.mtu);
        return 2;
    }
    if (opts.routesOnly) {
        std::string error;
        if (!takeoverRoutes(opts.device, opts.excludes, opts.socketPath, opts.fakeIpCidr,
                                 opts.address.substr(0, opts.address.find('/')), opts.dryRun,
                                 &error)) {
            std::fprintf(stderr, "%s\n", error.c_str());
            return 9;
        }
        return 0;
    }

    std::string error;

    // 路由接管前先记住物理网卡：接管后默认路由会指向 TUN，
    // 那时再读就拿不到物理设备名了（系统 DNS 要设在物理网卡上）
    const std::string physicalDevice = readDefaultRoute().device;

    // 上次异常退出可能留下「默认路由指向已删除的 TUN」的状态文件：先还原，
    // 否则这次接管会以一条坏路由为基准，越修越乱
    if (access(stateFilePath(opts.socketPath).c_str(), F_OK) == 0) {
        logLine("发现上次遗留的路由状态，先还原再接管");
        restoreRoutes(opts.socketPath, opts.dryRun);
    }

    if (!socketOwnedByCaller(opts.socketPath, &error)) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 4;
    }

    int  tunFd         = -1;
    int  fakeFd        = -1; // dry-run 时假的另一头
    bool deviceCreated = false;
    bool routesTaken   = false;
    if (opts.dryRun) {
        int pair[2] = {-1, -1};
        if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, pair) != 0) {
            std::fprintf(stderr, "socketpair 失败：%s\n", std::strerror(errno));
            return 5;
        }
        tunFd  = pair[0];
        fakeFd = pair[1];
        logLine("dry-run：不创建真实 TUN，用 socketpair 验证 fd 传递");
    } else {
        tunFd = createTunDevice(opts.device, &error);
        if (tunFd < 0) {
            std::fprintf(stderr, "%s\n", error.c_str());
            return 5;
        }
        deviceCreated = true;

        // 配地址 / MTU / 置 up。参数已校验，execv 数组形式调用，不经 shell。
        const std::string prefix = opts.address.substr(0, opts.address.find('/'));
        const std::string len    = opts.address.substr(opts.address.find('/') + 1);
        if (!runIp({"addr", "add", prefix + "/" + len, "dev", opts.device}, &error)
            || !runIp({"link", "set", "dev", opts.device, "mtu", std::to_string(opts.mtu)},
                      &error)
            || !runIp({"link", "set", "dev", opts.device, "up"}, &error)) {
            std::fprintf(stderr, "%s\n", error.c_str());
            runIp({"link", "del", "dev", opts.device}, &error);
            ::close(tunFd);
            return 6;
        }

        // 设备就绪后才动路由：顺序不能反，否则中途失败会留下断网的机器
        if (opts.autoRoute) {
            if (!takeoverRoutes(opts.device, opts.excludes, opts.socketPath, opts.fakeIpCidr,
                                 opts.address.substr(0, opts.address.find('/')), opts.dryRun,
                                 &error)) {
                std::fprintf(stderr, "接管路由失败：%s\n", error.c_str());
                restoreRoutes(opts.socketPath, opts.dryRun);
                runIp({"link", "del", "dev", opts.device}, &error);
                ::close(tunFd);
                return 9;
            }
            routesTaken = true;
        }
    }

    // ---- 在 TUN 地址上接管 53，并把系统 DNS 指过来 ----
    int         dnsUdpFd = -1;
    int         dnsTcpFd = -1;
    std::string dnsAddress;
    if (!opts.dryRun) {
        dnsAddress = opts.address.substr(0, opts.address.find('/'));

        std::string dnsError;
        dnsUdpFd = bindDnsSocket(dnsAddress, false, &dnsError);
        if (dnsUdpFd < 0) logLine("DNS(UDP) 未接管：" + dnsError); // 不致命
        dnsTcpFd = bindDnsSocket(dnsAddress, true, &dnsError);
        if (dnsTcpFd < 0) logLine("DNS(TCP) 未接管：" + dnsError);

        if (dnsUdpFd >= 0 && !physicalDevice.empty()) {
            pointSystemDns(physicalDevice, dnsAddress);
        }
    }

    // 交给主程序
    const int sock = connectToMainApp(opts.socketPath, &error);
    if (sock < 0) {
        std::fprintf(stderr, "%s\n", error.c_str());
        if (dnsUdpFd >= 0) ::close(dnsUdpFd);
        if (dnsTcpFd >= 0) ::close(dnsTcpFd);
        if (!physicalDevice.empty()) revertSystemDns(physicalDevice);
        if (routesTaken) restoreRoutes(opts.socketPath, opts.dryRun);
        if (deviceCreated && !opts.dryRun) runIp({"link", "del", "dev", opts.device}, &error);
        ::close(tunFd);
        return 7;
    }

    std::string header = "CLASHQT-TUN/" + std::to_string(kProtocolVersion) + " role=helper mode="
                         + (opts.dryRun ? "dry-run" : "tun") + " device=" + opts.device
                         + " addr=" + opts.address + " mtu=" + std::to_string(opts.mtu)
                         + " pid=" + std::to_string(getpid());
    if (dnsUdpFd >= 0) header += " dns=1";
    header += "\n";

    std::vector<int> fds = {tunFd};
    if (dnsUdpFd >= 0) fds.push_back(dnsUdpFd);
    if (dnsTcpFd >= 0) fds.push_back(dnsTcpFd);

    if (!sendFdsWithHeader(sock, fds, header, &error) || !waitForAck(sock, &error)) {
        std::fprintf(stderr, "%s\n", error.c_str());
        if (routesTaken) restoreRoutes(opts.socketPath, opts.dryRun);
        if (deviceCreated && !opts.dryRun) runIp({"link", "del", "dev", opts.device}, &error);
        ::close(tunFd);
        ::close(sock);
        return 8;
    }

    logLine("TUN 已交给主程序：" + opts.device);

    // 常驻：主程序断开 / 发来 down / 收到信号 → 清理并退出。
    // dry-run 时额外把 fake 端收到的数据回显，用来验证 fd 真的通。
    std::signal(SIGTERM, onSignal);
    std::signal(SIGINT, onSignal);
    std::signal(SIGHUP, onSignal);

    bool running = true;
    while (running && !g_stop) {
        struct pollfd pfds[2] = {};
        pfds[0].fd     = sock;
        pfds[0].events = POLLIN | POLLHUP | POLLERR;
        int count      = 1;
        if (opts.dryRun && fakeFd >= 0) {
            pfds[1].fd     = fakeFd;
            pfds[1].events = POLLIN;
            count          = 2;
        }

        const int ready = poll(pfds, nfds_t(count), 1000);
        if (ready < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ready == 0) continue;

        if (pfds[0].revents & (POLLIN | POLLHUP | POLLERR)) {
            char buf[64] = {};
            const ssize_t n = read(sock, buf, sizeof(buf) - 1);
            if (n <= 0) {
                logLine("主程序已断开");
                running = false;
            } else if (std::string(buf, size_t(n)).rfind("down", 0) == 0) {
                logLine("收到 down");
                running = false;
            }
        }
        if (count == 2 && (pfds[1].revents & POLLIN)) {
            char buf[512];
            const ssize_t n = read(fakeFd, buf, sizeof(buf));
            if (n > 0) {
                ssize_t written = 0;
                while (written < n) {
                    const ssize_t w = write(fakeFd, buf + written, size_t(n - written));
                    if (w <= 0) break;
                    written += w;
                }
            }
        }
    }

    if (sock >= 0) ::close(sock);
    if (tunFd >= 0) ::close(tunFd);
    if (fakeFd >= 0) ::close(fakeFd);

    if (dnsUdpFd >= 0) ::close(dnsUdpFd);
    if (dnsTcpFd >= 0) ::close(dnsTcpFd);

    if (!opts.dryRun) {
        if (!physicalDevice.empty()) revertSystemDns(physicalDevice);
        if (opts.autoRoute) {
            std::string ignored;
            if (!opts.fakeIpCidr.empty()) {
                runIp({"route", "del", opts.fakeIpCidr, "dev", opts.device}, &ignored);
            }
            restoreRoutes(opts.socketPath, false);
        }
        runIp({"link", "del", "dev", opts.device}, &error);
        logLine("已删除接口 " + opts.device);
    }
    return 0;
}

void printUsage()
{
    std::fprintf(stderr,
                 "用法：\n"
                 "  " CLASHQT_TUN_HELPER_NAME " status\n"
                 "  " CLASHQT_TUN_HELPER_NAME " up --name NAME --addr a.b.c.d/nn [--mtu N] --socket PATH\n"
                 "                 [--auto-route] [--route-exclude a.b.c.d]... [--dry-run]\n"
                 "                 [--routes-only]\n"
                 "  " CLASHQT_TUN_HELPER_NAME " down --name NAME\n"
                 "  " CLASHQT_TUN_HELPER_NAME " restore --socket PATH [--dry-run]\n");
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 2) {
        printUsage();
        return 2;
    }

    const std::string command = argv[1];
    if (command == "status") return commandStatus();

    if (command == "down") {
        std::string device;
        for (int i = 2; i + 1 < argc; ++i) {
            if (std::string(argv[i]) == "--name") device = argv[i + 1];
        }
        if (device.empty()) {
            printUsage();
            return 2;
        }
        return commandDown(device);
    }

    if (command == "restore") {
        std::string socketPath;
        bool        dryRun = false;
        for (int i = 2; i < argc; ++i) {
            const std::string key = argv[i];
            if (key == "--dry-run") dryRun = true;
            else if (i + 1 < argc && key == "--socket") socketPath = argv[++i];
        }
        if (socketPath.empty()) {
            printUsage();
            return 2;
        }
        restoreRoutes(socketPath, dryRun);
        return 0;
    }

    if (command == "up") {
        UpOptions opts;
        for (int i = 2; i < argc; ++i) {
            const std::string key = argv[i];
            if (key == "--dry-run") {
                opts.dryRun = true;
            } else if (key == "--auto-route") {
                opts.autoRoute = true;
            } else if (key == "--routes-only") {
                opts.routesOnly = true;
                opts.autoRoute  = true;
            } else if (i + 1 < argc && key == "--name") {
                opts.device = argv[++i];
            } else if (i + 1 < argc && key == "--addr") {
                opts.address = argv[++i];
            } else if (i + 1 < argc && key == "--socket") {
                opts.socketPath = argv[++i];
            } else if (i + 1 < argc && key == "--mtu") {
                opts.mtu = std::atoi(argv[++i]);
            } else if (i + 1 < argc && key == "--route-exclude") {
                opts.excludes.push_back(argv[++i]);
            } else if (i + 1 < argc && key == "--fake-ip-cidr") {
                opts.fakeIpCidr = argv[++i];
            } else {
                std::fprintf(stderr, "无法识别的参数：%s\n", key.c_str());
                printUsage();
                return 2;
            }
        }
        if (opts.device.empty() || opts.address.empty() || opts.socketPath.empty()) {
            printUsage();
            return 2;
        }
        return commandUp(opts);
    }

    printUsage();
    return 2;
}
