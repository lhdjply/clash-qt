#include "component/dns/DnsServer.h"
#include <QCoreApplication>

#include "common/JsonUtil.h"
#include "constant/Constants.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

namespace core {
namespace {

// 假 IP 的 TTL 取 1：映射由本进程维护，客户端缓存久了会在映射失效后
// 拿到一个指向错误目标的地址。
constexpr quint32 kFakeIpTtl = 1;
constexpr quint32 kRealTtl   = 60;

QString normalizeHost(const QString &name)
{
    QString host = name.trimmed().toLower();
    while (host.endsWith(QLatin1Char('.'))) host.chop(1);
    return host;
}

} // namespace

DnsServer::DnsServer(std::shared_ptr<Resolver> resolver, QObject *parent)
    : QObject(parent), m_resolver(std::move(resolver))
{
}

DnsServer::~DnsServer() { stopListening(); }

void DnsServer::applyDnsConfig(const QJsonObject &dnsConfig)
{
    m_enabled    = jsonutil::boolValue(dnsConfig, QStringLiteral("enable"), false);
    m_ipv6       = jsonutil::boolValue(dnsConfig, QStringLiteral("ipv6"), false);
    m_fakeIpMode = jsonutil::stringValue(dnsConfig, QStringLiteral("enhanced-mode"))
                   == QLatin1String("fake-ip");

    const QString listen = jsonutil::stringValue(dnsConfig, QStringLiteral("listen"));
    m_listenPort         = 0;
    if (!listen.isEmpty()) {
        // 支持 "127.0.0.1:1053"、"1053"、":1053"
        const int colon = listen.lastIndexOf(QLatin1Char(':'));
        if (colon >= 0) {
            const QString hostPart = listen.left(colon);
            m_listenAddr = hostPart.isEmpty() ? QHostAddress(QHostAddress::LocalHost)
                                              : QHostAddress(hostPart);
            m_listenPort = quint16(listen.mid(colon + 1).toUShort());
        } else {
            m_listenAddr = QHostAddress(QHostAddress::LocalHost);
            m_listenPort = quint16(listen.toUShort());
        }
    }

    // dns-hijack：只关心有没有 "any:53"（TUN 用），其余形式不影响本类
    m_hijackAny = false;
    for (const QString &spec : jsonutil::stringArrayValue(dnsConfig, QStringLiteral("dns-hijack"))) {
        if (spec.startsWith(QLatin1String("any:"), Qt::CaseInsensitive)) m_hijackAny = true;
    }

    if (m_enabled && m_listenPort > 0) {
        QString error;
        if (!startListening(&error)) {
            emit logMessage(QStringLiteral("warning"),
                            QCoreApplication::translate("clashqt::core", "DNS listener not started: %1").arg(error));
        }
    } else {
        stopListening();
    }
}

QString DnsServer::listenSummary() const
{
    if (!m_udp) return QCoreApplication::translate("clashqt::core", "Not listening");
    return QStringLiteral("%1:%2").arg(m_listenAddr.toString()).arg(m_listenPort);
}

bool DnsServer::startListening(QString *error)
{
    stopListening();

    if (m_listenPort == 0) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "dns.listen is not configured");
        return false;
    }

    auto *udp = new QUdpSocket(this);
    if (!udp->bind(m_listenAddr, m_listenPort,
                   QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        const QString reason = udp->errorString();
        delete udp;
        if (error) *error = QCoreApplication::translate("clashqt::core", "Failed to bind UDP %1:%2: %3")
                                .arg(m_listenAddr.toString())
                                .arg(m_listenPort)
                                .arg(reason);
        return false;
    }
    QObject::connect(udp, &QUdpSocket::readyRead, this, &DnsServer::onUdpReadyRead);
    m_udp = udp;

    // DNS over TCP：应答超过 512 字节或部分客户端会走 TCP
    auto *tcp = new QTcpServer(this);
    if (tcp->listen(m_listenAddr, m_listenPort)) {
        QObject::connect(tcp, &QTcpServer::newConnection, this, [this, tcp]() {
            while (QTcpSocket *socket = tcp->nextPendingConnection()) {
                QObject::connect(socket, &QTcpSocket::disconnected, socket,
                                 &QTcpSocket::deleteLater);
                QObject::connect(socket, &QTcpSocket::readyRead, socket, [this, socket]() {
                    // TCP 上 DNS 报文前有 2 字节长度前缀
                    QByteArray data = socket->property("dnsBuffer").toByteArray();
                    data += socket->readAll();
                    while (data.size() >= 2) {
                        const int length = (quint8(data.at(0)) << 8) | quint8(data.at(1));
                        if (data.size() < 2 + length) break;
                        const QByteArray query = data.mid(2, length);
                        data.remove(0, 2 + length);

                        const QByteArray reply =
                            handleQuery(query, socket->peerAddress());
                        if (!reply.isEmpty()) {
                            QByteArray framed;
                            framed.append(char((reply.size() >> 8) & 0xFF))
                                .append(char(reply.size() & 0xFF));
                            framed.append(reply);
                            socket->write(framed);
                        }
                    }
                    socket->setProperty("dnsBuffer", data);
                });
            }
        });
        m_tcp = tcp;
    }

    emit logMessage(QStringLiteral("info"),
                    QCoreApplication::translate("clashqt::core", "DNS listener started: %1 (enhanced-mode=%2)")
                        .arg(listenSummary(), m_fakeIpMode ? QStringLiteral("fake-ip")
                                                           : QStringLiteral("normal")));
    return true;
}

void DnsServer::stopListening()
{
    if (m_udp) {
        m_udp->close();
        m_udp->deleteLater();
        m_udp = nullptr;
    }
    if (m_tcp) {
        m_tcp->close();
        m_tcp->deleteLater();
        m_tcp = nullptr;
    }
}

void DnsServer::onUdpReadyRead()
{
    while (m_udp && m_udp->hasPendingDatagrams()) {
        QByteArray    payload;
        payload.resize(int(m_udp->pendingDatagramSize()));
        QHostAddress  sender;
        quint16       senderPort = 0;
        m_udp->readDatagram(payload.data(), payload.size(), &sender, &senderPort);

        const QByteArray reply = handleQuery(payload, sender);
        if (!reply.isEmpty()) m_udp->writeDatagram(reply, sender, senderPort);
    }
}

QList<dns::Answer> DnsServer::answerFor(const dns::Question &question, const QHostAddress &client)
{
    QList<dns::Answer> answers;
    const QString      host = normalizeHost(question.name);
    if (host.isEmpty()) return answers;

    const bool isA    = (question.type == 1);
    const bool isAaaa = (question.type == 28);
    if (!isA && !isAaaa) return answers; // 非 A/AAAA 一律空应答

    // 1) hosts 静态映射优先（与 enhanced-mode 无关）
    if (auto fromHosts = m_resolver->lookupHosts(host, isAaaa)) {
        if ((isA && fromHosts->protocol() == QAbstractSocket::IPv4Protocol)
            || (isAaaa && fromHosts->protocol() == QAbstractSocket::IPv6Protocol)) {
            answers.append(dns::Answer{host, question.type, kRealTtl, *fromHosts, QString()});
            return answers;
        }
    }

    // 2) fake-ip：A 分配假 IP；AAAA 故意空应答，让客户端回落到 A，
    //    避免应用拿真实 IPv6 直连绕过代理。
    //    注意判据是「**这个域名**是否走 fake-ip」，而不是「是否 A 查询」——
    //    否则 AAAA 会漏出真实 IPv6 记录（自检时就是这么抓到的）。
    if (m_fakeIpMode && !m_resolver->shouldSkipFakeIp(host)) {
        if (!isA) {
            emit logMessage(QStringLiteral("debug"),
                            QCoreApplication::translate("clashqt::core", "DNS fake-ip: AAAA query for %1 returned an empty answer").arg(host));
            return answers;
        }

        const QHostAddress fake = m_resolver->mappingFakeIp(host).value_or(QHostAddress());
        if (!fake.isNull()) {
            answers.append(dns::Answer{host, 1, kFakeIpTtl, fake, QString()});
            emit logMessage(QStringLiteral("debug"),
                            QStringLiteral("DNS fake-ip：%1 -> %2").arg(host, fake.toString()));
            return answers;
        }
    }

    // 3) 真实解析（normal 模式，或被 fake-ip-filter 排除的域名）
    const QList<QHostAddress> addresses = m_resolver->resolveReal(host, m_timeoutMs, isAaaa);
    for (const QHostAddress &address : addresses) {
        const bool v4 = address.protocol() == QAbstractSocket::IPv4Protocol;
        if ((isA && v4) || (isAaaa && !v4)) {
            answers.append(dns::Answer{host, question.type, kRealTtl, address, QString()});
        }
    }

    emit logMessage(QStringLiteral("debug"),
                    QCoreApplication::translate("clashqt::core", "DNS real resolution: %1 -> %2 record(s)")
                        .arg(host)
                        .arg(answers.size()));
    return answers;
}

QByteArray DnsServer::handleQuery(const QByteArray &payload, const QHostAddress &client)
{
    Q_UNUSED(client);

    if (!m_enabled) return QByteArray();
    if (!m_resolver) return QByteArray();

    const auto parsed = dns::parseMessage(payload);
    if (!parsed || !parsed->isValid() || parsed->isResponse || parsed->questions.isEmpty()) {
        return QByteArray(); // 非法报文 / 应答 / 无问题段：丢弃
    }

    QList<dns::Answer> answers;
    for (const dns::Question &question : parsed->questions) {
        answers += answerFor(question, client);
    }

    return dns::buildResponse(*parsed, answers);
}

} // namespace core
