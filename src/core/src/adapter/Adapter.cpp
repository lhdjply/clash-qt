#include "adapter/Adapters.h"

#include "base/Common.h"
#include "common/JsonUtil.h"
#include "common/SocketUtil.h"
#include "tunnel/UdpChannel.h"

#include <QDateTime>
#include <QEventLoop>
#include <QJsonArray>
#include <QTcpSocket>
#include <QTimer>
#include <QUdpSocket>

#include <algorithm>
#include <limits>

namespace core {

// ===========================================================================
//  Adapter 基类
// ===========================================================================

void Adapter::recordDelay(int delayMs)
{
    DelayRecord record;
    record.time  = QDateTime::currentDateTime().toString(Qt::ISODate);
    record.delay = delayMs;

    QMutexLocker locker(&m_historyMutex);
    m_history.prepend(record);
    // 只保留最近 10 次，与参照实现的历史记录长度一致
    while (m_history.size() > 10) m_history.removeLast();

    m_alive.store(delayMs > 0, std::memory_order_relaxed);
}

QList<Adapter::DelayRecord> Adapter::history() const
{
    QMutexLocker locker(&m_historyMutex);
    return m_history;
}

QJsonObject Adapter::status() const
{
    QJsonObject object;
    object.insert(QStringLiteral("name"), name());
    object.insert(QStringLiteral("type"), toString(type()));
    object.insert(QStringLiteral("alive"), alive());
    object.insert(QStringLiteral("udp"), supportUdp());
    object.insert(QStringLiteral("xudp"), false);
    object.insert(QStringLiteral("tfo"), false);

    QJsonArray history;
    for (const DelayRecord &record : this->history()) {
        QJsonObject item;
        item.insert(QStringLiteral("time"), record.time);
        item.insert(QStringLiteral("delay"), record.delay);
        history.append(item);
    }
    object.insert(QStringLiteral("history"), history);
    return object;
}

// ===========================================================================
//  内部辅助：带超时的异步连接（伪同步）
// ===========================================================================
namespace {

// 在已运行事件循环的线程中发起连接并等待完成。
// 返回空指针表示连接失败，error 会带上具体原因。
// 返回 unique_ptr：调用方决定最终所有权（一般转移给 TcpStream）
std::unique_ptr<QTcpSocket> connectTcp(const QString &host, quint16 port, int timeoutMs,
                                       QString *error)
{
    auto socket = std::make_unique<QTcpSocket>();

    QEventLoop loop;
    QTimer     timer;
    timer.setSingleShot(true);

    QObject::connect(socket.get(), &QTcpSocket::connected, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    socket->connectToHost(host, port);
    timer.start(std::max(1000, timeoutMs));
    loop.exec();

    if (socket->state() != QAbstractSocket::ConnectedState) {
        if (error) {
            // 一定带上错误码：Qt 对很多失败只会给出 "Unknown error"，
            // 有了 SocketError 才能区分「连接被拒 / 主机不可达 / 超时」
            *error = QStringLiteral("连接 %1:%2 失败：%3（error=%4）")
                         .arg(host)
                         .arg(port)
                         .arg(socket->errorString())
                         .arg(int(socket->error()));
        }
        socket->abort();
        return nullptr;
    }
    return socket;
}

// 带溢出缓冲的读取器。
// 握手协议（HTTP CONNECT / SOCKS5）的应答长度由应答内容决定，而 socket 一次
// 可读的数据可能跨过应答边界进入有效载荷。Qt 6 的 QIODevice 没有「回灌」接口，
// 因此本类把多读的字节留在缓冲区里，由调用方在握手结束后取出交给 Tunnel。
class SocketBuffer {
public:
    explicit SocketBuffer(QTcpSocket *socket) : m_socket(socket) {}

    // 保证缓冲区至少有 count 字节；返回是否达成
    bool ensure(int count, int timeoutMs)
    {
        if (m_buffer.size() >= count) return true;
        return sockutil::readAtLeast(m_socket, &m_buffer, count, timeoutMs);
    }

    // 保证缓冲区出现分隔符或超过上限；用于解析变长的 HTTP 响应头
    bool ensureUntil(const QByteArray &delimiter, int timeoutMs, int maxBytes)
    {
        return sockutil::readUntil(m_socket, &m_buffer, delimiter, timeoutMs, maxBytes);
    }

    QByteArray take(int count)
    {
        // QByteArray::size() 返回 qsizetype，需显式收敛后再比较
        const int        n    = int(std::min<qsizetype>(count, m_buffer.size()));
        const QByteArray head = m_buffer.left(n);
        m_buffer.remove(0, n);
        return head;
    }

    // 取出并清空剩余字节（握手结束后属于有效载荷的部分）
    QByteArray takeRest()
    {
        const QByteArray rest = m_buffer;
        m_buffer.clear();
        return rest;
    }

    const QByteArray &buffer() const { return m_buffer; }

private:
    QTcpSocket *m_socket = nullptr;
    QByteArray  m_buffer;
};

// 目标地址字符串：优先域名，其次 IP
QString targetAuthority(const DialContext &context)
{
    const QString host = context.host.isEmpty() ? context.dstIp : context.host;
    return QStringLiteral("%1:%2").arg(host).arg(context.dstPort);
}

} // namespace

// ===========================================================================
//  DIRECT
// ===========================================================================

DirectAdapter::DirectAdapter() = default;

DirectAdapter::DirectAdapter(QString name) : m_name(std::move(name)) {}

DialResult DirectAdapter::dial(const DialContext &context, int timeoutMs)
{
    DialResult result;

    // 优先使用 dstIp：Tunnel 在 needsLocalResolution() 为真的出站上已经用
    // 内核解析器（hosts / nameserver-policy / fallback）解析出真实地址。
    // 若这里改回用域名交给 Qt 解析，会绕过内核 DNS 配置，导致 hosts 映射
    // 失效，甚至把 .local 之类的名字交给系统的 mDNS 解析。
    const QString host = context.dstIp.isEmpty() ? context.host : context.dstIp;
    if (host.isEmpty() || context.dstPort <= 0) {
        result.error = QStringLiteral("DIRECT 目标地址无效");
        return result;
    }

    if (context.network.compare(QLatin1String("udp"), Qt::CaseInsensitive) == 0) {
        // UDP 直连：创建已连接的 UDP 套接字，交给 UdpWorker 收发数据报
        auto socket = std::make_shared<QUdpSocket>();
        const QHostAddress address(host);
        if (address.isNull()) {
            result.error = QStringLiteral("DIRECT UDP 需要字面 IP 目标");
            return result;
        }
        // 绑定到任意本地端口，connectToHost 之后 socket 即处于已连接状态
        if (!socket->bind(QHostAddress(QHostAddress::AnyIPv4), 0)) {
            result.error = QStringLiteral("UDP 绑定失败：%1").arg(socket->errorString());
            return result;
        }
        socket->connectToHost(address, quint16(context.dstPort));
        result.udp        = socket;
        result.udpChannel = std::make_shared<SocketUdpChannel>(socket);
        m_alive = true;
        return result;
    }

    // TCP：连接目标。host 可能是域名，交给 Qt 解析（等价于参照实现的直连解析策略）
    auto socket = connectTcp(host, quint16(context.dstPort), timeoutMs, &result.error);
    if (!socket) return result;
    result.stream = std::make_shared<TcpStream>(socket.release(), nullptr);
    m_alive = true;
    return result;
}

// ===========================================================================
//  REJECT / REJECT-DROP
// ===========================================================================

DialResult RejectAdapter::dial(const DialContext &context, int timeoutMs)
{
    Q_UNUSED(context);
    Q_UNUSED(timeoutMs);

    DialResult result;
    if (m_drop) {
        // 不做任何上行连接，但仍保持客户端连接打开并丢弃数据
        result.drop = true;
        return result;
    }
    result.error = QStringLiteral("连接被规则拒绝（REJECT）");
    m_alive = false;
    return result;
}

// ===========================================================================
//  HTTP 代理
// ===========================================================================

HttpProxyAdapter::HttpProxyAdapter(ProxyConfig config) : m_config(std::move(config))
{
    m_username = jsonutil::stringValue(m_config.extra, QStringLiteral("username"));
    m_password = jsonutil::stringValue(m_config.extra, QStringLiteral("password"));
    // type: https 表示到代理服务器本身使用 TLS，本阶段按明文处理并记录告警
    m_tls = m_config.rawType.compare(QLatin1String("https"), Qt::CaseInsensitive) == 0;
    if (m_tls) {
        Logger::instance().warn(
            LogCategory::Adapter,
            QStringLiteral("HTTP 代理 %1 声明为 https，当前以明文 CONNECT 方式连接")
                .arg(m_config.name));
    }
}

std::unique_ptr<QTcpSocket> HttpProxyAdapter::connectToServer(int timeoutMs, QString *error)
{
    if (m_config.server.isEmpty() || m_config.port <= 0) {
        if (error) *error = QStringLiteral("HTTP 代理 %1 缺少 server/port").arg(m_config.name);
        return nullptr;
    }
    return connectTcp(m_config.server, quint16(m_config.port), timeoutMs, error);
}

DialResult HttpProxyAdapter::dial(const DialContext &context, int timeoutMs)
{
    DialResult result;

    if (context.network.compare(QLatin1String("udp"), Qt::CaseInsensitive) == 0) {
        result.error = QStringLiteral("HTTP 代理不支持 UDP 转发");
        return result;
    }

    auto socket = connectToServer(timeoutMs, &result.error);
    if (!socket) return result;

    const QString authority = targetAuthority(context);

    QByteArray request;
    request.append("CONNECT " + authority.toUtf8() + " HTTP/1.1\r\n");
    request.append("Host: " + authority.toUtf8() + "\r\n");
    request.append("Proxy-Connection: Keep-Alive\r\n");
    if (!m_username.isEmpty()) {
        const QString credentials = m_username + QLatin1Char(':') + m_password;
        request.append("Proxy-Authorization: Basic "
                       + credentials.toUtf8().toBase64() + "\r\n");
    }
    request.append("\r\n");

    if (!sockutil::writeAll(socket.get(), request, timeoutMs, &result.error)) return result;

    SocketBuffer reader(socket.get());
    // HTTP 响应头不应超过 32KB；超过即视为异常，避免被恶意对端拖死
    if (!reader.ensureUntil("\r\n\r\n", timeoutMs, 32 * 1024)) {
        result.error = QStringLiteral("HTTP 代理 %1 无响应或响应头超长").arg(m_config.name);
        return result;
    }

    const QByteArray response   = reader.buffer();
    const int        headerEnd  = response.indexOf("\r\n\r\n");
    const int        statusEnd  = response.indexOf("\r\n");
    const QByteArray statusLine = response.left(statusEnd > 0 ? statusEnd : headerEnd);

    // 解析 "HTTP/1.1 200 Connection established"
    const QList<QByteArray> parts = statusLine.split(' ');
    bool      ok         = false;
    const int statusCode = parts.size() >= 2 ? parts.at(1).trimmed().toInt(&ok) : 0;
    if (!ok || statusCode != 200) {
        result.error = QStringLiteral("HTTP 代理返回 %1").arg(QString::fromLatin1(statusLine));
        return result;
    }

    // 丢掉已消费的响应头，剩余字节属于目标数据流
    reader.take(headerEnd + 4);
    result.extraData = reader.takeRest();

    // connectTcp 返回 unique_ptr 语义的裸指针，此处转移给 TcpStream 管理
    result.stream = std::make_shared<TcpStream>(socket.release(), nullptr);
    m_alive = true;
    return result;
}

QJsonObject HttpProxyAdapter::status() const
{
    QJsonObject object = Adapter::status();
    object.insert(QStringLiteral("server"), m_config.server);
    object.insert(QStringLiteral("port"), m_config.port);
    return object;
}

// ===========================================================================
//  SOCKS5 代理
// ===========================================================================

Socks5ProxyAdapter::Socks5ProxyAdapter(ProxyConfig config) : m_config(std::move(config))
{
    m_username = jsonutil::stringValue(m_config.extra, QStringLiteral("username"));
    m_password = jsonutil::stringValue(m_config.extra, QStringLiteral("password"));
}

std::unique_ptr<QTcpSocket> Socks5ProxyAdapter::connectToServer(int timeoutMs, QString *error)
{
    if (m_config.server.isEmpty() || m_config.port <= 0) {
        if (error) *error = QStringLiteral("SOCKS5 代理 %1 缺少 server/port").arg(m_config.name);
        return nullptr;
    }
    return connectTcp(m_config.server, quint16(m_config.port), timeoutMs, error);
}

bool Socks5ProxyAdapter::negotiate(QTcpSocket *socket, int timeoutMs, QString *error)
{
    const bool needAuth = !m_username.isEmpty() || !m_password.isEmpty();

    // 版本 + 方法列表：无认证 0x00；有认证 0x02
    QByteArray greeting;
    greeting.append(char(0x05));
    greeting.append(char(needAuth ? 2 : 1));
    greeting.append(char(0x00));
    if (needAuth) greeting.append(char(0x02));

    if (!sockutil::writeAll(socket, greeting, timeoutMs, error)) return false;

    SocketBuffer reader(socket);
    if (!reader.ensure(2, timeoutMs)) {
        if (error) *error = QStringLiteral("SOCKS5 协商无响应");
        return false;
    }

    const QByteArray handshake = reader.take(2);
    if (static_cast<unsigned char>(handshake.at(0)) != 0x05) {
        if (error) *error = QStringLiteral("SOCKS5 版本不匹配");
        return false;
    }

    const unsigned char method = static_cast<unsigned char>(handshake.at(1));
    if (method == 0xFF) {
        if (error) *error = QStringLiteral("SOCKS5 服务器拒绝了所有认证方式");
        return false;
    }

    if (method == 0x02) {
        // 用户名/密码认证（RFC 1929）
        const QByteArray user = m_username.toUtf8();
        const QByteArray pass = m_password.toUtf8();
        if (user.size() > 255 || pass.size() > 255) {
            if (error) *error = QStringLiteral("SOCKS5 用户名或密码过长");
            return false;
        }

        QByteArray auth;
        auth.append(char(0x01));
        auth.append(char(user.size()));
        auth.append(user);
        auth.append(char(pass.size()));
        auth.append(pass);

        if (!sockutil::writeAll(socket, auth, timeoutMs, error)) return false;

        if (!reader.ensure(2, timeoutMs)) {
            if (error) *error = QStringLiteral("SOCKS5 认证无响应");
            return false;
        }
        const QByteArray authReply = reader.take(2);
        if (static_cast<unsigned char>(authReply.at(1)) != 0x00) {
            if (error) *error = QStringLiteral("SOCKS5 认证失败");
            return false;
        }
    } else if (method != 0x00) {
        if (error) {
            *error = QStringLiteral("SOCKS5 服务器要求不支持的认证方式 0x%1")
                         .arg(QString::number(method, 16));
        }
        return false;
    }
    return true;
}

bool Socks5ProxyAdapter::requestConnect(QTcpSocket *socket, const DialContext &context,
                                       int timeoutMs, QString *error)
{
    QByteArray request;
    request.append(char(0x05));
    request.append(char(0x01)); // CONNECT
    request.append(char(0x00)); // RSV

    const QString domain  = context.host;
    const QString ipText  = context.dstIp.isEmpty() ? domain : context.dstIp;
    const QHostAddress literal(ipText);

    if (!domain.isEmpty() && literal.isNull()) {
        // ATYP=0x03 域名：交给代理服务器远端解析，避免本地 DNS 泄漏
        const QByteArray raw = domain.toUtf8();
        if (raw.size() > 255) {
            if (error) *error = QStringLiteral("SOCKS5 目标域名过长");
            return false;
        }
        request.append(char(0x03));
        request.append(char(raw.size()));
        request.append(raw);
    } else if (literal.protocol() == QAbstractSocket::IPv4Protocol) {
        request.append(char(0x01));
        const quint32 v4 = literal.toIPv4Address();
        for (int shift = 24; shift >= 0; shift -= 8) {
            request.append(char((v4 >> shift) & 0xFF));
        }
    } else if (literal.protocol() == QAbstractSocket::IPv6Protocol) {
        request.append(char(0x04));
        const Q_IPV6ADDR v6 = literal.toIPv6Address();
        for (int i = 0; i < 16; ++i) request.append(char(v6.c[i]));
    } else {
        if (error) *error = QStringLiteral("SOCKS5 目标地址无法解析：%1").arg(ipText);
        return false;
    }

    request.append(char((context.dstPort >> 8) & 0xFF));
    request.append(char(context.dstPort & 0xFF));

    if (!sockutil::writeAll(socket, request, timeoutMs, error)) return false;

    SocketBuffer reader(socket);

    // 应答固定部分：VER(1) REP(1) RSV(1) ATYP(1)
    if (!reader.ensure(4, timeoutMs)) {
        if (error) *error = QStringLiteral("SOCKS5 CONNECT 无响应");
        return false;
    }
    const QByteArray fixed = reader.take(4);
    const unsigned char reply = static_cast<unsigned char>(fixed.at(1));
    if (reply != 0x00) {
        static const char *kReasons[] = {
            "成功",         "一般性失败",   "连接不被允许",   "网络不可达",
            "主机不可达",   "连接被拒绝",   "TTL 过期",       "不支持的命令",
            "不支持的地址类型"};
        const QString reason = reply <= 8 ? QString::fromUtf8(kReasons[reply])
                                          : QStringLiteral("未知错误 %1").arg(reply);
        if (error) {
            *error = QStringLiteral("SOCKS5 连接被拒：%1（%2）").arg(reason, targetAuthority(context));
        }
        return false;
    }

    // BND.ADDR 长度由 ATYP 决定，BND.PORT 固定 2 字节；必须读净，
    // 否则残留字节会被误当作目标数据交给客户端。
    const unsigned char atyp = static_cast<unsigned char>(fixed.at(3));
    int addressLength = 0;
    switch (atyp) {
    case 0x01: addressLength = 4; break;
    case 0x04: addressLength = 16; break;
    case 0x03: {
        if (!reader.ensure(1, timeoutMs)) {
            if (error) *error = QStringLiteral("SOCKS5 应答被截断");
            return false;
        }
        addressLength = static_cast<unsigned char>(reader.take(1).at(0));
        break;
    }
    default:
        if (error) *error = QStringLiteral("SOCKS5 应答包含未知地址类型 %1").arg(int(atyp));
        return false;
    }

    if (!reader.ensure(addressLength + 2, timeoutMs)) {
        if (error) *error = QStringLiteral("SOCKS5 应答被截断");
        return false;
    }
    reader.take(addressLength + 2);

    // 应答之后可能已带上目标数据，必须完整带走
    m_pendingData = reader.takeRest();
    return true;
}

DialResult Socks5ProxyAdapter::dial(const DialContext &context, int timeoutMs)
{
    DialResult result;
    m_pendingData.clear();

    if (context.network.compare(QLatin1String("udp"), Qt::CaseInsensitive) == 0) {
        // UDP ASSOCIATE 的完整转发链路属于后续阶段的工作，这里明确失败，
        // 避免 UDP 流量在无感知的情况下走直连。
        result.error = QStringLiteral("SOCKS5 UDP ASSOCIATE 转发尚未实现（Phase 2）");
        return result;
    }

    auto socket = connectToServer(timeoutMs, &result.error);
    if (!socket) return result;

    if (!negotiate(socket.get(), timeoutMs, &result.error)) return result;
    if (!requestConnect(socket.get(), context, timeoutMs, &result.error)) return result;

    result.stream    = std::make_shared<TcpStream>(socket.release(), nullptr);
    result.extraData = m_pendingData;
    m_alive          = true;
    return result;
}

QJsonObject Socks5ProxyAdapter::status() const
{
    QJsonObject object = Adapter::status();
    object.insert(QStringLiteral("server"), m_config.server);
    object.insert(QStringLiteral("port"), m_config.port);
    return object;
}

// ===========================================================================
//  未实现协议占位
// ===========================================================================

UnsupportedAdapter::UnsupportedAdapter(ProxyConfig config, QString reason)
    : m_config(std::move(config)), m_reason(std::move(reason))
{
}

DialResult UnsupportedAdapter::dial(const DialContext &context, int timeoutMs)
{
    Q_UNUSED(context);
    Q_UNUSED(timeoutMs);

    DialResult result;
    result.error = QStringLiteral("节点 %1（%2）暂不支持：%3")
                       .arg(m_config.name, m_config.rawType, m_reason);
    return result;
}

QJsonObject UnsupportedAdapter::status() const
{
    QJsonObject object = Adapter::status();
    object.insert(QStringLiteral("unsupported"), true);
    object.insert(QStringLiteral("reason"), m_reason);
    return object;
}

// ===========================================================================
//  ProxyGroup
// ===========================================================================

ProxyGroup::ProxyGroup(ProxyGroupConfig config, std::vector<AdapterPtr> members)
    : m_config(std::move(config)), m_members(std::move(members))
{
    // 优先采用配置中显式指定的首个成员作为初始展示选择。
    // 注意：这只是默认值，不代表用户做过选择（m_userSelected 保持 false），
    // 因此拨号失败时的回退仍然可以记住更合适的成员。
    if (!m_members.empty()) {
        m_fixed = m_config.proxies.value(0);
        if (current() == nullptr) m_fixed.clear();
    }
}

QStringList ProxyGroup::supportNetworks() const
{
    QStringList networks;
    for (const AdapterPtr &member : m_members) {
        if (!member) continue;
        for (const QString &network : member->supportNetworks()) {
            if (!networks.contains(network)) networks << network;
        }
    }
    if (networks.isEmpty()) networks << QStringLiteral("TCP");
    return networks;
}

AdapterPtr ProxyGroup::current() const
{
    if (!m_fixed.isEmpty()) {
        for (const AdapterPtr &member : m_members) {
            if (member && member->name() == m_fixed) return member;
        }
    }
    return nullptr;
}

QString ProxyGroup::now() const
{
    const AdapterPtr selected = current();
    if (selected) return selected->name();

    const AdapterPtr automatic = autoPick();
    return automatic ? automatic->name() : QString();
}

bool ProxyGroup::select(const QString &memberName)
{
    for (const AdapterPtr &member : m_members) {
        if (member && member->name() == memberName) {
            m_fixed        = memberName;
            m_userSelected = true; // 用户显式选择：此后不再被自动回退改写
            Logger::instance().info(
                LogCategory::Adapter,
                QStringLiteral("组 %1 切换到 %2").arg(m_config.name, memberName));
            return true;
        }
    }
    return false;
}

void ProxyGroup::unfix()
{
    m_fixed.clear();
    m_userSelected = false; // 清除固定后回到自动选择
}

AdapterPtr ProxyGroup::autoPick() const
{
    if (m_members.empty()) return nullptr;

    switch (m_config.type) {
    case GroupType::URLTest:
    case GroupType::Fallback: {
        // 选择最近一次测速延迟最低且存活的成员；无测速数据时退回首个成员
        AdapterPtr best;
        int        bestDelay = std::numeric_limits<int>::max();
        for (const AdapterPtr &member : m_members) {
            if (!member || !member->alive()) continue;
            const QList<Adapter::DelayRecord> history = member->history();
            const int delay = history.isEmpty() ? 0 : history.first().delay;
            if (delay > 0 && delay < bestDelay) {
                bestDelay = delay;
                best      = member;
            }
        }
        if (best) return best;
        break;
    }

    case GroupType::LoadBalance:
    case GroupType::Relay:
    case GroupType::Selector:
    default:
        break;
    }
    return m_members.front();
}

void ProxyGroup::selectByDelay()
{
    if (m_config.type != GroupType::URLTest && m_config.type != GroupType::Fallback) return;

    // 用户已手动固定成员时，测速只更新延迟数据，不改动选择（「固定优先」语义）。
    // 否则点一次测速就会把用户的选择冲掉；想回到自动选择用界面上的「恢复自动」。
    if (m_userSelected) return;

    const AdapterPtr picked = autoPick();
    if (picked) m_fixed = picked->name();
}

DialResult ProxyGroup::dial(const DialContext &context, int timeoutMs)
{
    AdapterPtr target = current();
    if (!target) target = autoPick();

    if (!target) {
        DialResult result;
        result.error = QStringLiteral("代理组 %1 没有可用成员").arg(m_config.name);
        return result;
    }

    DialResult result = target->dial(context, timeoutMs);
    m_alive = result.ok();

    // URLTest / Fallback：当前成员失败时按顺序回退到下一个成员
    if (!result.ok() && !result.drop
        && (m_config.type == GroupType::Fallback || m_config.type == GroupType::URLTest)) {
        for (const AdapterPtr &member : m_members) {
            if (!member || member->name() == target->name()) continue;
            result = member->dial(context, timeoutMs);
            if (result.ok()) {
                // 用户手动选定的节点不能因为一次拨号失败就被静默替换掉
                // （典型表现：选好节点后一开系统代理，选择自己变回列表第一个）。
                // 因此仅在「自动」状态下记住回退结果，用户固定时只对本次连接生效。
                if (!m_userSelected) {
                    m_fixed = member->name();
                    Logger::instance().info(
                        LogCategory::Adapter,
                        QStringLiteral("组 %1 回退到 %2").arg(m_config.name, member->name()));
                } else {
                    Logger::instance().info(
                        LogCategory::Adapter,
                        QStringLiteral("组 %1 本次连接回退到 %2（手动选择仍为 %3）")
                            .arg(m_config.name, member->name(), m_fixed));
                }
                return result;
            }
        }
    }
    return result;
}

QJsonObject ProxyGroup::status() const
{
    QJsonObject object;
    object.insert(QStringLiteral("name"), m_config.name);
    object.insert(QStringLiteral("type"), toString(m_config.type));
    object.insert(QStringLiteral("alive"), alive());
    object.insert(QStringLiteral("udp"), supportUdp());
    object.insert(QStringLiteral("xudp"), false);
    object.insert(QStringLiteral("tfo"), false);
    object.insert(QStringLiteral("hidden"), m_config.hidden);
    object.insert(QStringLiteral("now"), now());

    if (!m_fixed.isEmpty()) object.insert(QStringLiteral("fixed"), m_fixed);
    if (!m_config.url.isEmpty()) object.insert(QStringLiteral("testUrl"), m_config.url);
    if (!m_config.icon.isEmpty()) object.insert(QStringLiteral("icon"), m_config.icon);

    QJsonArray all;
    for (const AdapterPtr &member : m_members) {
        if (member) all.append(member->name());
    }
    object.insert(QStringLiteral("all"), all);

    QJsonArray history;
    for (const DelayRecord &record : this->history()) {
        QJsonObject item;
        item.insert(QStringLiteral("time"), record.time);
        item.insert(QStringLiteral("delay"), record.delay);
        history.append(item);
    }
    object.insert(QStringLiteral("history"), history);
    return object;
}

} // namespace core
