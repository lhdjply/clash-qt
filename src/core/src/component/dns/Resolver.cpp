#include "component/dns/Resolver.h"
#include <QCoreApplication>

#include "base/Common.h"
#include "common/JsonUtil.h"

#include <QDateTime>
#include <QEventLoop>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QTimer>
#include <QUdpSocket>
#include <QUrl>

#include <algorithm>

namespace core {

// ===========================================================================
//  DNS 报文编解码
// ===========================================================================

namespace dns {
namespace {

// 把域名编码为 DNS 标签序列："a.b.com" -> 01 'a' 01 'b' 03 'c''o''m' 00
QByteArray encodeName(const QString &name)
{
    QByteArray result;
    const QStringList labels = name.split(QLatin1Char('.'), Qt::SkipEmptyParts);
    for (const QString &label : labels) {
        const QByteArray raw = label.toUtf8();
        if (raw.size() > 63) return QByteArray(); // 标签最长 63 字节
        result.append(static_cast<char>(raw.size()));
        result.append(raw);
    }
    result.append('\0');
    return result;
}

// 解码域名，支持 RFC 1035 的压缩指针（0b11xxxxxx）
bool decodeName(const QByteArray &payload, int &offset, QString &outName, int depth = 0)
{
    if (depth > 16) return false; // 防止压缩指针成环导致无限递归

    QStringList labels;
    int         cursor = offset;
    bool        jumped = false;

    while (cursor < payload.size()) {
        const unsigned char length = static_cast<unsigned char>(payload.at(cursor));

        if (length == 0) {
            ++cursor;
            break;
        }

        if ((length & 0xC0) == 0xC0) {
            if (cursor + 1 >= payload.size()) return false;
            const int pointer = ((length & 0x3F) << 8)
                                | static_cast<unsigned char>(payload.at(cursor + 1));
            if (!jumped) offset = cursor + 2;
            jumped = true;

            int       pointerOffset = pointer;
            QString   pointed;
            if (!decodeName(payload, pointerOffset, pointed, depth + 1)) return false;
            if (!pointed.isEmpty()) labels << pointed;
            cursor += 2;
            break;
        }

        if (cursor + 1 + length > payload.size()) return false;
        labels << QString::fromUtf8(payload.mid(cursor + 1, length));
        cursor += 1 + length;
    }

    if (!jumped) offset = cursor;
    outName = labels.join(QLatin1Char('.'));
    return true;
}

quint16 readU16(const QByteArray &payload, int offset)
{
    return static_cast<quint16>((static_cast<unsigned char>(payload.at(offset)) << 8)
                                | static_cast<unsigned char>(payload.at(offset + 1)));
}

quint32 readU32(const QByteArray &payload, int offset)
{
    return (static_cast<quint32>(static_cast<unsigned char>(payload.at(offset))) << 24)
           | (static_cast<quint32>(static_cast<unsigned char>(payload.at(offset + 1))) << 16)
           | (static_cast<quint32>(static_cast<unsigned char>(payload.at(offset + 2))) << 8)
           | static_cast<quint32>(static_cast<unsigned char>(payload.at(offset + 3)));
}

void appendU16(QByteArray &out, quint16 value)
{
    out.append(static_cast<char>((value >> 8) & 0xFF));
    out.append(static_cast<char>(value & 0xFF));
}

} // namespace

quint16 randomId()
{
    return static_cast<quint16>(QRandomGenerator::global()->bounded(1, 65535));
}

QByteArray buildQuery(quint16 id, const QString &name, quint16 type)
{
    QByteArray out;

    appendU16(out, id);
    appendU16(out, 0x0100); // 标准查询 + 期望递归
    appendU16(out, 1);      // QDCOUNT
    appendU16(out, 0);      // ANCOUNT
    appendU16(out, 0);      // NSCOUNT
    appendU16(out, 0);      // ARCOUNT

    const QByteArray encodedName = encodeName(name);
    if (encodedName.isEmpty()) return QByteArray();

    out.append(encodedName);
    appendU16(out, type);
    appendU16(out, 1); // IN
    return out;
}

QByteArray buildResponse(const Message &query, const QList<Answer> &answers, int rcode)
{
    QByteArray out;
    if (!query.isValid() || query.questions.isEmpty()) return out;

    // ---- 头部：QR=1，RA=1，RD 沿用查询，rcode 由调用方决定 ----
    const quint16 questionCount = quint16(query.questions.size());
    quint16       answerCount   = 0;
    for (const Answer &answer : answers) {
        if (answer.type == 1 && answer.address.protocol() == QAbstractSocket::IPv4Protocol) {
            ++answerCount;
        } else if (answer.type == 28
                   && answer.address.protocol() == QAbstractSocket::IPv6Protocol) {
            ++answerCount;
        } else if (answer.type == 5 && !answer.text.isEmpty()) {
            ++answerCount;
        }
    }

    appendU16(out, query.id);
    appendU16(out, quint16(0x8000 | 0x0080 | (rcode & 0x000F))); // QR | RA | rcode
    appendU16(out, questionCount);
    appendU16(out, answerCount);
    appendU16(out, 0); // NSCOUNT
    appendU16(out, 0); // ARCOUNT

    // ---- 问题段：原样回带。第一条问题的名字位于偏移 12，
    //      应答段用它做指针压缩（0xC00C），省掉重复的域名 ----
    for (const Question &question : query.questions) {
        const QByteArray encoded = encodeName(question.name);
        if (encoded.isEmpty()) return QByteArray();
        out.append(encoded);
        appendU16(out, question.type);
        appendU16(out, question.cls);
    }

    // ---- 应答段 ----
    for (const Answer &answer : answers) {
        QByteArray rdata;
        if (answer.type == 1 && answer.address.protocol() == QAbstractSocket::IPv4Protocol) {
            const quint32 v4 = answer.address.toIPv4Address();
            rdata.append(char((v4 >> 24) & 0xFF)).append(char((v4 >> 16) & 0xFF));
            rdata.append(char((v4 >> 8) & 0xFF)).append(char(v4 & 0xFF));
        } else if (answer.type == 28
                   && answer.address.protocol() == QAbstractSocket::IPv6Protocol) {
            const Q_IPV6ADDR v6 = answer.address.toIPv6Address();
            rdata.append(reinterpret_cast<const char *>(v6.c), sizeof(v6.c));
        } else if (answer.type == 5 && !answer.text.isEmpty()) {
            rdata = encodeName(answer.text);
            if (rdata.isEmpty()) continue;
        } else {
            continue; // 暂不支持的类型不写入（宁缺勿错）
        }

        appendU16(out, 0xC00C); // 名字指针 → 第一条问题
        appendU16(out, answer.type);
        appendU16(out, 1); // IN
        out.append(char((answer.ttl >> 24) & 0xFF)).append(char((answer.ttl >> 16) & 0xFF));
        out.append(char((answer.ttl >> 8) & 0xFF)).append(char(answer.ttl & 0xFF));
        appendU16(out, quint16(rdata.size()));
        out.append(rdata);
    }

    return out;
}

std::optional<Message> parseMessage(const QByteArray &payload)
{
    if (payload.size() < 12) return std::nullopt;

    Message message;
    message.id         = readU16(payload, 0);
    const quint16 flags = readU16(payload, 2);
    message.isResponse = (flags & 0x8000) != 0;
    message.truncated  = (flags & 0x0200) != 0;
    message.rcode      = flags & 0x000F;

    const int qdCount = readU16(payload, 4);
    const int anCount = readU16(payload, 6);

    int offset = 12;

    for (int i = 0; i < qdCount; ++i) {
        Question question;
        if (!decodeName(payload, offset, question.name)) return std::nullopt;
        if (offset + 4 > payload.size()) return std::nullopt;
        question.type = readU16(payload, offset);
        question.cls  = readU16(payload, offset + 2);
        offset += 4;
        message.questions.append(question);
    }

    for (int i = 0; i < anCount; ++i) {
        Answer answer;
        if (!decodeName(payload, offset, answer.name)) return std::nullopt;
        if (offset + 10 > payload.size()) return std::nullopt;

        answer.type = readU16(payload, offset);
        answer.ttl  = readU32(payload, offset + 4);
        const int rdLength = readU16(payload, offset + 8);
        offset += 10;

        if (offset + rdLength > payload.size()) return std::nullopt;

        if (answer.type == 1 && rdLength == 4) {
            // A 记录
            QHostAddress address;
            quint32      v4 = 0;
            for (int b = 0; b < 4; ++b) {
                v4 = (v4 << 8) | static_cast<unsigned char>(payload.at(offset + b));
            }
            address.setAddress(v4);
            answer.address = address;
        } else if (answer.type == 28 && rdLength == 16) {
            // AAAA 记录
            Q_IPV6ADDR v6{};
            for (int b = 0; b < 16; ++b) {
                v6.c[b] = static_cast<unsigned char>(payload.at(offset + b));
            }
            QHostAddress address;
            address.setAddress(v6);
            answer.address = address;
        } else if (answer.type == 5) {
            // CNAME：RDATA 本身是（可能压缩的）域名，需要从 rdata 起点重新解码
            int    nameOffset = offset;
            QString cname;
            if (decodeName(payload, nameOffset, cname)) answer.text = cname;
        }

        offset += rdLength;
        message.answers.append(answer);
    }

    message.setValid(true);
    return message;
}

} // namespace dns

// ===========================================================================
//  FakeIpPool
// ===========================================================================

void FakeIpPool::configure(const QString &range)
{
    clear();

    const auto net = IpNet::parse(range);
    if (!net || net->family() != 4) {
        Logger::instance().warn(
            LogCategory::Dns,
            QCoreApplication::translate("clashqt::core", "Invalid or non-IPv4 fake-ip-range: %1; falling back to 198.18.0.1/16").arg(range));
        const auto fallback = IpNet::parse(QStringLiteral("198.18.0.1/16"));
        m_base = fallback->address().toIPv4Address();
        m_mask = 0xFFFF0000u;
    } else {
        m_base = net->address().toIPv4Address();
        const int prefix = net->prefixLength();
        m_mask = prefix == 0 ? 0u : (0xFFFFFFFFu << (32 - prefix));
    }

    m_next    = m_base + 1;
    m_end     = (m_base & m_mask) | (~m_mask);
    m_enabled = true;
}

QHostAddress FakeIpPool::allocate(const QString &host)
{
    if (!m_enabled || host.isEmpty()) return QHostAddress();

    const auto existing = m_forward.constFind(host);
    if (existing != m_forward.constEnd()) {
        QHostAddress address;
        address.setAddress(existing.value());
        return address;
    }

    // 跳过网络地址与广播地址；池耗尽时回绕到起始，覆盖最久未使用的映射。
    if (m_next >= m_end) m_next = m_base + 1;

    const quint32 candidate = m_next++;

    // 覆盖已被占用的地址时，清理其反向映射，保持双向映射一致
    const auto stale = m_reverse.constFind(candidate);
    if (stale != m_reverse.constEnd()) {
        m_forward.remove(stale.value());
    }

    m_forward.insert(host, candidate);
    m_reverse.insert(candidate, host);

    QHostAddress address;
    address.setAddress(candidate);
    return address;
}

std::optional<QString> FakeIpPool::reverseLookup(const QHostAddress &address) const
{
    if (!m_enabled) return std::nullopt;
    if (address.protocol() != QAbstractSocket::IPv4Protocol) return std::nullopt;

    const auto it = m_reverse.constFind(address.toIPv4Address());
    if (it == m_reverse.constEnd()) return std::nullopt;
    return it.value();
}

bool FakeIpPool::contains(const QHostAddress &address) const
{
    if (!m_enabled) return false;
    if (address.protocol() != QAbstractSocket::IPv4Protocol) return false;
    return (address.toIPv4Address() & m_mask) == (m_base & m_mask);
}

void FakeIpPool::clear()
{
    m_forward.clear();
    m_reverse.clear();
    m_enabled = false;
    m_next    = 0;
    m_end     = 0;
}

// ===========================================================================
//  Resolver
// ===========================================================================

Resolver::Resolver(QObject *parent)
    : QObject(parent)
{
}

Resolver::~Resolver() = default;

std::optional<Resolver::Server> Resolver::parseServer(const QString &spec)
{
    Server server;
    QString text = spec.trimmed();
    if (text.isEmpty()) return std::nullopt;

    if (text.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)) {
        server.httpsUrl = text;
        const QUrl url(text);
        server.host = url.host();
        if (server.host.isEmpty()) return std::nullopt;
        server.tls = true;
        return server;
    }

    if (text.startsWith(QLatin1String("tls://"), Qt::CaseInsensitive)) {
        server.tls = true;
        text.remove(0, 6);
    } else if (text.startsWith(QLatin1String("tcp://"), Qt::CaseInsensitive)) {
        server.tcp = true;
        text.remove(0, 6);
    } else if (text.startsWith(QLatin1String("udp://"), Qt::CaseInsensitive)) {
        text.remove(0, 6);
    } else if (text.startsWith(QLatin1String("dhcp://"), Qt::CaseInsensitive)) {
        // DHCP 提供的 DNS 需要从系统解析器读取，本阶段退化为系统默认
        server.host = QStringLiteral("system");
        return server;
    }

    // 处理 IPv6 字面量 "[::1]:53"
    if (text.startsWith(QLatin1Char('['))) {
        const int end = text.indexOf(QLatin1Char(']'));
        if (end < 0) return std::nullopt;
        server.host = text.mid(1, end - 1);
        const QString rest = text.mid(end + 1);
        if (rest.startsWith(QLatin1Char(':'))) {
            const auto port = strutil::parseInt(rest.mid(1));
            if (port) server.port = *port;
        }
    } else {
        const int colon = text.lastIndexOf(QLatin1Char(':'));
        if (colon > 0 && !text.contains(QLatin1Char(':'))) {
            server.host = text.left(colon);
            const auto port = strutil::parseInt(text.mid(colon + 1));
            if (port) server.port = *port;
        } else {
            server.host = text;
        }
    }

    if (server.tls) server.port = 853;
    if (server.host.isEmpty()) return std::nullopt;
    return server;
}

void Resolver::applyDnsConfig(const QJsonObject &dnsConfig)
{
    // 整段配置替换必须在锁内完成：此时可能正有拨号线程在 selectNameservers /
    // 读缓存，把 QStringList / QHash 换掉却不同步，读到的是「半新半旧的堆」。
    // 这里不含网络等待，持锁时间只有内存拷贝的量级。
    QString summary;
    {
        QMutexLocker locker(&m_mutex);

        if (dnsConfig.isEmpty()) {
            // 未提供 dns 段时保持可用：使用公共 DNS 兜底，避免解析器完全瘫痪
            m_nameservers = {QStringLiteral("223.5.5.5"), QStringLiteral("119.29.29.29")};
            m_defaultNameservers = m_nameservers;
            m_cache.clear();
            return;
        }

        m_ipv6 = jsonutil::boolValue(dnsConfig, QStringLiteral("ipv6"), false);

        const QString enhanced = jsonutil::stringValue(dnsConfig, QStringLiteral("enhanced-mode"));
        m_enhancedMode = parseDnsEnhancedMode(enhanced).value_or(DnsEnhancedMode::Normal);

        m_nameservers = jsonutil::stringArrayValue(dnsConfig, QStringLiteral("nameserver"));
        m_fallback    = jsonutil::stringArrayValue(dnsConfig, QStringLiteral("fallback"));
        m_proxyServerNameservers =
            jsonutil::stringArrayValue(dnsConfig, QStringLiteral("proxy-server-nameserver"));
        m_directNameservers = jsonutil::stringArrayValue(dnsConfig, QStringLiteral("direct-nameserver"));

        // 收集上游用到的域名（DoH 的 doh.pub / dns.alidns.com 等）：它们必须绕过
        // fake-ip，并在解析时用引导服务器，否则会解析到自己身上形成自锁。
        m_upstreamHosts.clear();
        {
            const QStringList all = m_nameservers + m_fallback + m_proxyServerNameservers
                                    + m_directNameservers
                                    + jsonutil::stringArrayValue(dnsConfig,
                                                                 QStringLiteral("default-nameserver"));
            for (const QString &spec : all) {
                QString hostPart = spec.trimmed();
                const int schemeEnd = hostPart.indexOf(QStringLiteral("://"));
                if (schemeEnd >= 0) hostPart = hostPart.mid(schemeEnd + 3);

                const QString lower = hostPart.toLower();
                if (lower.startsWith(QLatin1String("system"))
                    || lower.startsWith(QLatin1String("dhcp"))) {
                    continue;
                }
                const int slash = hostPart.indexOf(QLatin1Char('/'));
                if (slash >= 0) hostPart = hostPart.left(slash);
                // IPv6 形如 [::1]:53
                if (hostPart.startsWith(QLatin1Char('['))) {
                    const int close = hostPart.indexOf(QLatin1Char(']'));
                    if (close > 0) hostPart = hostPart.mid(1, close - 1);
                } else {
                    const int colon = hostPart.indexOf(QLatin1Char(':'));
                    if (colon > 0) hostPart = hostPart.left(colon);
                }
                if (hostPart.isEmpty()) continue;
                if (IpNet::parseIp(hostPart)) continue; // 字面 IP 不需要
                m_upstreamHosts.insert(hostPart.toLower());
            }
        }

        m_defaultNameservers =
            jsonutil::stringArrayValue(dnsConfig, QStringLiteral("default-nameserver"));
        if (m_defaultNameservers.isEmpty()) {
            // 与参照实现一致：default-nameserver 用于解析 DNS 服务器自身的域名，
            // 未配置时退化为使用 nameserver 的第一项。
            m_defaultNameservers = m_nameservers.isEmpty()
                                       ? QStringList{QStringLiteral("223.5.5.5")}
                                       : QStringList{m_nameservers.first()};
        }
        if (m_nameservers.isEmpty()) m_nameservers = m_defaultNameservers;

        m_fakeIpFilter = jsonutil::stringArrayValue(dnsConfig, QStringLiteral("fake-ip-filter"));
        m_fakeIpFilterMode = parseFilterMode(
                                 jsonutil::stringValue(dnsConfig, QStringLiteral("fake-ip-filter-mode")))
                                 .value_or(FilterMode::Blacklist);

        const QString fakeRange = jsonutil::stringValue(
            dnsConfig, QStringLiteral("fake-ip-range"), QStringLiteral("198.18.0.1/16"));

        m_fakeIp.clear();
        m_fakeIp.configure(fakeRange);
        m_fakeIp.setEnabled(m_enhancedMode == DnsEnhancedMode::FakeIp);
        if (!m_fakeIp.enabled()) m_fakeIp.clear();

        // nameserver-policy: { "example.com": ["1.1.1.1"], "+.cn": "223.5.5.5" }
        m_nameserverPolicy.clear();
        const QJsonObject policy = dnsConfig.value(QStringLiteral("nameserver-policy")).toObject();
        for (auto it = policy.constBegin(); it != policy.constEnd(); ++it) {
            QStringList servers;
            if (it.value().isArray()) {
                for (const auto &entry : it.value().toArray()) {
                    if (entry.isString()) servers << entry.toString();
                }
            } else if (it.value().isString()) {
                servers << it.value().toString();
            }
            if (!servers.isEmpty()) {
                QString key = it.key().trimmed().toLower();
                if (key.startsWith(QLatin1String("+."))) key = key.mid(2);
                m_nameserverPolicy.append({key, servers});
            }
        }

        // hosts: { "example.com": "1.2.3.4", "*.test": ["1.1.1.1", "::1"] }
        m_hosts.clear();
        const QJsonObject hosts = dnsConfig.value(QStringLiteral("hosts")).toObject();
        for (auto it = hosts.constBegin(); it != hosts.constEnd(); ++it) {
            QString value;
            if (it.value().isArray()) {
                const QJsonArray array = it.value().toArray();
                if (!array.isEmpty() && array.first().isString()) value = array.first().toString();
            } else if (it.value().isString()) {
                value = it.value().toString();
            }
            if (!value.isEmpty()) {
                m_hosts.insert(it.key().trimmed().toLower(), value);
            }
        }

        m_cache.clear();

        summary = QCoreApplication::translate("clashqt::core", "DNS configured: enhanced-mode=%1, nameserver=%2, fake-ip=%3")
                      .arg(toString(m_enhancedMode), m_nameservers.join(QLatin1Char(',')),
                           m_fakeIp.enabled() ? QStringLiteral("on") : QStringLiteral("off"));
    } // 释放锁后再发信号：槽函数可能又调回 Resolver

    if (!summary.isEmpty()) emit logMessage(QStringLiteral("info"), summary);
}

// ---- hosts / fake-ip ------------------------------------------------------

std::optional<QHostAddress> Resolver::lookupHosts(const QString &host, bool ipv6) const
{
    const QString needle = host.trimmed().toLower();
    if (needle.isEmpty()) return std::nullopt;

    QMutexLocker locker(&m_mutex);

    // 精确匹配优先
    auto it = m_hosts.constFind(needle);
    if (it != m_hosts.constEnd()) {
        const auto address = IpNet::parseIp(it.value());
        if (address && (ipv6 || address->protocol() == QAbstractSocket::IPv4Protocol)) {
            return address;
        }
    }

    // 通配匹配："*.example.com" 匹配任意子域
    for (auto wit = m_hosts.constBegin(); wit != m_hosts.constEnd(); ++wit) {
        const QString pattern = wit.key();
        if (!pattern.startsWith(QLatin1String("*."))) continue;
        const QString suffix = pattern.mid(2);
        if (!needle.endsWith(suffix) || needle.size() <= suffix.size()) continue;
        if (needle.at(needle.size() - suffix.size() - 1) != QLatin1Char('.')) continue;
        const auto address = IpNet::parseIp(wit.value());
        if (address && (ipv6 || address->protocol() == QAbstractSocket::IPv4Protocol)) {
            return address;
        }
    }
    return std::nullopt;
}

void Resolver::setServerHosts(const QStringList &hosts)
{
    QMutexLocker locker(&m_mutex);
    m_serverHosts.clear();
    for (const QString &host : hosts) {
        const QString needle = host.trimmed().toLower();
        if (!needle.isEmpty()) m_serverHosts.insert(needle);
    }
}

QSet<QString> Resolver::upstreamHosts() const
{
    QMutexLocker locker(&m_mutex);
    return m_upstreamHosts;
}

bool Resolver::shouldSkipFakeIp(const QString &host) const
{
    QMutexLocker locker(&m_mutex);
    return shouldSkipFakeIpLocked(host);
}

bool Resolver::shouldSkipFakeIpLocked(const QString &host) const
{
    const QString needle0 = host.trimmed().toLower();
    // DNS 上游自身的域名：必须给真实地址，否则解析器会连回 TUN 自锁
    if (m_upstreamHosts.contains(needle0)) return true;
    // 代理节点的服务器域名：同样必须真实解析，否则连不到节点
    if (m_serverHosts.contains(needle0)) return true;
    if (m_fakeIpFilter.isEmpty()) return false;

    const QString needle = needle0;
    bool          listed = false;

    for (const QString &rawPattern : m_fakeIpFilter) {
        const QString pattern = rawPattern.trimmed().toLower();
        if (pattern.isEmpty()) continue;

        bool hit = false;
        if (pattern == QLatin1String("*")) {
            hit = true;
        } else if (pattern.startsWith(QLatin1String("+."))) {
            const QString suffix = pattern.mid(2);
            hit = needle == suffix
                  || (needle.endsWith(suffix) && needle.size() > suffix.size()
                      && needle.at(needle.size() - suffix.size() - 1) == QLatin1Char('.'));
        } else if (pattern.startsWith(QLatin1String("*."))) {
            const QString suffix = pattern.mid(2);
            hit = needle.endsWith(suffix) && needle.size() > suffix.size()
                  && needle.at(needle.size() - suffix.size() - 1) == QLatin1Char('.');
        } else {
            hit = needle == pattern;
        }

        if (hit) {
            listed = true;
            break;
        }
    }

    // blacklist：命中过滤列表则走真实解析；whitelist：只有命中者才用 fake-ip
    return m_fakeIpFilterMode == FilterMode::Blacklist ? listed : !listed;
}

std::optional<QHostAddress> Resolver::mappingFakeIp(const QString &host)
{
    if (host.isEmpty()) return std::nullopt;

    // 分配要改 fake-ip 的双向表，必须与其它线程的 fakeIpToHost 互斥
    QMutexLocker locker(&m_mutex);
    if (!m_fakeIp.enabled()) return std::nullopt;
    if (shouldSkipFakeIpLocked(host)) return std::nullopt;

    // 真实 IP 文本不做 fake-ip 映射
    if (IpNet::parseIp(host)) return std::nullopt;

    const QHostAddress address = m_fakeIp.allocate(host.toLower());
    if (address.isNull()) return std::nullopt;
    return address;
}

std::optional<QString> Resolver::fakeIpToHost(const QHostAddress &address) const
{
    QMutexLocker locker(&m_mutex);
    return m_fakeIp.reverseLookup(address);
}

bool Resolver::isFakeIp(const QHostAddress &address) const
{
    QMutexLocker locker(&m_mutex);
    return m_fakeIp.isFakeIp(address);
}

// ---- 服务器选择 -----------------------------------------------------------

QStringList Resolver::selectNameservers(const QString &host, bool useFallback) const
{
    QMutexLocker locker(&m_mutex);

    if (useFallback && !m_fallback.isEmpty()) return m_fallback;

    const QString needle = host.trimmed().toLower();
    for (const auto &entry : m_nameserverPolicy) {
        const QString &suffix = entry.first;
        if (needle == suffix
            || (needle.endsWith(suffix) && needle.size() > suffix.size()
                && needle.at(needle.size() - suffix.size() - 1) == QLatin1Char('.'))) {
            return entry.second;
        }
    }
    return m_nameservers;
}

// ---- 缓存 -----------------------------------------------------------------

void Resolver::remember(const QString &host, int type,
                        const QList<QHostAddress> &addresses, quint32 ttl)
{
    if (addresses.isEmpty()) return;
    CacheEntry entry;
    entry.addresses = addresses;
    // TTL 上限 1 小时、下限 10 秒，避免内核返回 0 导致缓存失效或异常长缓存
    const quint32 clamped = std::clamp(ttl, 10u, 3600u);
    entry.expireAtMs = QDateTime::currentMSecsSinceEpoch() + qint64(clamped) * 1000;
    cacheStore(QStringLiteral("%1|%2").arg(host.toLower()).arg(type), entry);
}

// ---- 容器访问辅助（全部自带锁，供锁外跑网络的路径复用）-------------------

bool Resolver::isUpstreamHost(const QString &host) const
{
    QMutexLocker locker(&m_mutex);
    return m_upstreamHosts.contains(host.trimmed().toLower());
}

std::optional<Resolver::CacheEntry> Resolver::cacheLookup(const QString &key) const
{
    QMutexLocker locker(&m_mutex);
    const auto it = m_cache.constFind(key);
    if (it == m_cache.constEnd()) return std::nullopt;
    if (it.value().expireAtMs <= QDateTime::currentMSecsSinceEpoch()) return std::nullopt;
    return it.value();
}

void Resolver::cacheStore(const QString &key, const CacheEntry &entry)
{
    QMutexLocker locker(&m_mutex);
    m_cache.insert(key, entry);
}

void Resolver::clearCache()
{
    QMutexLocker locker(&m_mutex);
    m_cache.clear();
}

int Resolver::cacheSize() const
{
    QMutexLocker locker(&m_mutex);
    return int(m_cache.size());
}

// ---- 查询 -----------------------------------------------------------------

QList<QHostAddress> Resolver::queryUdp(const Server &server, const QString &host,
                                       quint16 type, int timeoutMs)
{
    QList<QHostAddress> result;

    QUdpSocket socket;
    const quint16 id = dns::randomId();
    const QByteArray query = dns::buildQuery(id, host, type);
    if (query.isEmpty()) return result;

    if (socket.writeDatagram(query, QHostAddress(server.host), quint16(server.port)) < 0) {
        return result;
    }

    QEventLoop loop;
    QTimer     timer;
    timer.setSingleShot(true);

    QObject::connect(&socket, &QUdpSocket::readyRead, &loop, [&]() {
        while (socket.hasPendingDatagrams()) {
            QByteArray  buffer;
            buffer.resize(int(socket.pendingDatagramSize()));
            socket.readDatagram(buffer.data(), buffer.size());

            const auto message = dns::parseMessage(buffer);
            if (!message || message->id != id || !message->isResponse) continue;
            if (message->rcode != 0) continue;

            for (const dns::Answer &answer : message->answers) {
                if (answer.type == type && !answer.address.isNull()) {
                    result.append(answer.address);
                }
            }
            if (!result.isEmpty()) remember(host, type, result, message->answers.isEmpty() ? 60
                                                                                          : message->answers.first().ttl);
        }
        if (!result.isEmpty()) loop.quit();
    });

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(&socket, &QUdpSocket::errorOccurred, &loop,
                     [&](QAbstractSocket::SocketError) { loop.quit(); });

    timer.start(std::max(500, timeoutMs));
    loop.exec();

    return result;
}

QList<QHostAddress> Resolver::queryTcp(const Server &server, const QString &host,
                                       quint16 type, int timeoutMs)
{
    QList<QHostAddress> result;

    QTcpSocket socket;
    const quint16 id = dns::randomId();
    const QByteArray query = dns::buildQuery(id, host, type);
    if (query.isEmpty()) return result;

    socket.connectToHost(server.host, quint16(server.port));
    if (!socket.waitForConnected(std::max(500, timeoutMs))) return result;

    // DNS over TCP 需要 2 字节长度前缀
    QByteArray framed;
    framed.append(static_cast<char>((query.size() >> 8) & 0xFF));
    framed.append(static_cast<char>(query.size() & 0xFF));
    framed.append(query);

    socket.write(framed);
    if (!socket.waitForBytesWritten(std::max(500, timeoutMs))) return result;
    if (!socket.waitForReadyRead(std::max(500, timeoutMs))) return result;

    QByteArray buffer = socket.readAll();
    while (buffer.size() < 2) {
        if (!socket.waitForReadyRead(std::max(500, timeoutMs))) break;
        buffer.append(socket.readAll());
    }
    if (buffer.size() < 2) return result;

    const int length = (static_cast<unsigned char>(buffer.at(0)) << 8)
                       | static_cast<unsigned char>(buffer.at(1));
    while (buffer.size() < 2 + length) {
        if (!socket.waitForReadyRead(std::max(500, timeoutMs))) break;
        buffer.append(socket.readAll());
    }
    if (buffer.size() < 2 + length) return result;

    const auto message = dns::parseMessage(buffer.mid(2, length));
    if (!message || message->id != id || message->rcode != 0) return result;

    for (const dns::Answer &answer : message->answers) {
        if (answer.type == type && !answer.address.isNull()) result.append(answer.address);
    }
    if (!result.isEmpty()) {
        remember(host, type, result,
                 message->answers.isEmpty() ? 60 : message->answers.first().ttl);
    }
    return result;
}

QNetworkAccessManager *Resolver::networkForCurrentThread()
{
    // 每个线程一份，首次使用时在该线程内创建。
    // 这里**故意不释放**：thread_local 的析构发生在 main 返回之后，那时
    // QCoreApplication 可能已经销毁，QNAM 析构会踩到已释放的内部对象
    //（实测表现为退出时 free(): invalid pointer）。一个 QNAM 几十 KB，
    // 每个线程留一个完全可以接受。
    static thread_local QNetworkAccessManager *manager = nullptr;
    if (!manager) manager = new QNetworkAccessManager();
    return manager;
}

QList<QHostAddress> Resolver::queryHttps(const Server &server, const QString &host,
                                         quint16 type, int timeoutMs)
{
    QList<QHostAddress> result;
    QNetworkAccessManager *network = networkForCurrentThread();
    if (!network || server.httpsUrl.isEmpty()) return result;

    QUrl url(server.httpsUrl);
    // RFC 8484：POST + application/dns-message
    url.setQuery(QStringLiteral("dns=%1").arg(
        QString::fromLatin1(dns::buildQuery(dns::randomId(), host, type).toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals))));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/dns-message"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(std::max(1000, timeoutMs));

    // 使用 GET + dns 参数形式，兼容性最好，无需关心分块 POST
    QNetworkReply *reply = network->get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer::singleShot(std::max(1000, timeoutMs), &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        const QByteArray payload = reply->readAll();
        const auto message = dns::parseMessage(payload);
        if (message && message->rcode == 0) {
            for (const dns::Answer &answer : message->answers) {
                if (answer.type == type && !answer.address.isNull()) result.append(answer.address);
            }
            if (!result.isEmpty()) {
                remember(host, type, result,
                         message->answers.isEmpty() ? 60 : message->answers.first().ttl);
            }
        }
    }
    reply->deleteLater();
    return result;
}

QList<QHostAddress> Resolver::queryServer(const Server &server, const QString &host,
                                          quint16 type, int timeoutMs)
{
    if (!server.httpsUrl.isEmpty()) return queryHttps(server, host, type, timeoutMs);
    if (server.tcp || server.tls) {
        // tls:// 需要 DoT 的 TLS 包装，本阶段以 TCP 明文回退并在日志中说明
        if (server.tls) {
            emit logMessage(QStringLiteral("warning"),
                            QCoreApplication::translate("clashqt::core", "DoT (tls://) currently connects over plain TCP to %1").arg(server.host));
        }
        return queryTcp(server, host, type, timeoutMs);
    }
    return queryUdp(server, host, type, timeoutMs);
}

QList<QHostAddress> Resolver::resolve(const QString &host, int timeoutMs, bool ipv6)
{
    return resolveImpl(host, timeoutMs, ipv6, /*allowFakeIp=*/true);
}

QList<QHostAddress> Resolver::resolveReal(const QString &host, int timeoutMs, bool ipv6)
{
    return resolveImpl(host, timeoutMs, ipv6, /*allowFakeIp=*/false);
}

QList<QHostAddress> Resolver::resolveImpl(const QString &host, int timeoutMs, bool ipv6,
                                          bool allowFakeIp)
{
    const QString needle = host.trimmed();
    if (needle.isEmpty()) return {};

    // 1) 字面 IP 直接返回
    if (const auto literal = IpNet::parseIp(needle)) {
        return {*literal};
    }

    // 1.5) 上游服务器自身的域名：只能走 default-nameserver（字面 IP 的 DoT/DNS），
    //      绝不能再走 DoH —— 那等于解析「解析器自己」，在 TUN 下会自锁。
    if (isUpstreamHost(needle)) {
        // 先看缓存：这条路径会被 TUN 线程反复调用（每次 DIRECT 解析都可能触发），
        // 每次都去问引导服务器会明显拖慢建连。
        const QString upstreamKey =
            QStringLiteral("%1|%2").arg(needle.toLower()).arg(int(ipv6 ? 28 : 1));
        if (const auto cached = cacheLookup(upstreamKey)) return cached->addresses;

        // 引导服务器列表取一份副本再遍历：容器在本线程查询期间可能被
        // applyDnsConfig 整体替换（换订阅 / 改设置），直接引用会读到坏内存。
        const QStringList bootstrapServers = defaultNameservers();

        for (const QString &spec : bootstrapServers) {
            const auto server = parseServer(spec);
            if (!server) continue;
            if (!IpNet::parseIp(server->host)) continue; // 引导服务器也必须是字面 IP

            // 引导阶段一律用明文 UDP/53：tls:// 目前没有真正的 DoT 实现（queryServer
            // 会退化成明文 TCP 到 853 端口，必然失败），而这一步只用来解析
            // 「DNS 上游自己的域名」，走明文 53 完全够用且最可靠。
            Server probe = *server;
            probe.tls    = false;
            probe.tcp    = false;
            probe.port   = 53;

            const QList<QHostAddress> resolved =
                queryUdp(probe, needle, quint16(ipv6 ? 28 : 1), std::min(timeoutMs, 3000));
            if (!resolved.isEmpty()) {
                remember(needle, int(ipv6 ? 28 : 1), resolved, 600);
                return resolved;
            }
        }
        // AAAA 查不到是常态（多数 DoH / 节点域名只有 A 记录），客户端本来就会
        // 回落到 A。只有 A 查询失败才值得告警，否则每换一次网络就刷一条假故障。
        emit logMessage(ipv6 ? QStringLiteral("debug") : QStringLiteral("warning"),
                        QCoreApplication::translate("clashqt::core", "Could not resolve the DNS upstream host through the bootstrap server: %1").arg(needle));
        return {};
    }

    // 2) hosts 优先
    if (const auto fromHosts = lookupHosts(needle, ipv6)) return {*fromHosts};

    // 3) fake-ip（仅入站客户端查询路径启用）
    if (allowFakeIp) {
        if (const auto fake = mappingFakeIp(needle)) return {*fake};
    }

    // 4) 缓存
    const int  cacheType = ipv6 ? 28 : 1;
    const QString cacheKey = QStringLiteral("%1|%2").arg(needle.toLower()).arg(cacheType);
    if (const auto cached = cacheLookup(cacheKey)) return cached->addresses;

    // 5) 逐服务器查询
    const QStringList servers = selectNameservers(needle, false);
    QList<QHostAddress> result;

    for (const QString &spec : servers) {
        const auto server = parseServer(spec);
        if (!server) {
            emit logMessage(QStringLiteral("warning"),
                            QCoreApplication::translate("clashqt::core", "Could not parse the DNS server config: %1").arg(spec));
            continue;
        }
        // 服务器本身是域名时才需要引导解析；DoH 不需要（走 URL，由 QNAM 自己解）。
        //
        // 注意引导必须用**明文 UDP/53**：m_defaultNameservers 通常是
        // `tls://223.5.5.5`，若照着它的 853 端口发明文 UDP，必然收不到应答，
        // 每次查询都会白白等满 3 秒超时 —— 这正是「TUN 下网页像打不开」的元凶之一。
        Server effective = *server;
        if (effective.httpsUrl.isEmpty() && !IpNet::parseIp(effective.host)
            && effective.host != QLatin1String("system")) {
            const auto bootstrap = parseServer(defaultNameservers().value(0));
            if (bootstrap) {
                Server probe = *bootstrap;
                probe.tls    = false;
                probe.tcp    = false;
                probe.port   = 53;
                const auto resolved = queryUdp(probe, effective.host, 1,
                                               std::min(timeoutMs, 1500));
                if (!resolved.isEmpty()) effective.host = resolved.first().toString();
            }
        }

        result = queryServer(effective, needle, quint16(cacheType), timeoutMs);
        if (!result.isEmpty()) break;
    }

    // 6) fallback 组（仅在上一步失败时使用，对应参照实现的 fallback 语义）
    const QStringList fallbacks = fallbackServers();
    if (result.isEmpty() && !fallbacks.isEmpty()) {
        for (const QString &spec : fallbacks) {
            const auto server = parseServer(spec);
            if (!server) continue;
            result = queryServer(*server, needle, quint16(cacheType), timeoutMs);
            if (!result.isEmpty()) break;
        }
    }

    if (result.isEmpty()) {
        // 同上：AAAA 解析不到不算故障（客户端会回落 A），降为 debug 免得刷假告警
        emit logMessage(ipv6 ? QStringLiteral("debug") : QStringLiteral("warning"),
                        QCoreApplication::translate("clashqt::core", "DNS resolution failed: %1").arg(needle));
        // 负缓存 30 秒：解析失败的域名会被反复重试，每次都要等 DoH 超时，
        // 而这条路径跑在 TUN 数据面线程里，会直接卡住所有连接。
        CacheEntry negative;
        negative.expireAtMs = QDateTime::currentMSecsSinceEpoch() + 30 * 1000;
        cacheStore(cacheKey, negative);
    }
    return result;
}

std::optional<QHostAddress> Resolver::resolveOne(const QString &host, int timeoutMs, bool ipv6)
{
    const QList<QHostAddress> addresses = resolve(host, timeoutMs, ipv6);
    if (addresses.isEmpty()) return std::nullopt;
    return addresses.first();
}

QStringList Resolver::defaultNameservers() const
{
    QMutexLocker locker(&m_mutex);
    return m_defaultNameservers;
}

QStringList Resolver::fallbackServers() const
{
    QMutexLocker locker(&m_mutex);
    return m_fallback;
}

} // namespace core
