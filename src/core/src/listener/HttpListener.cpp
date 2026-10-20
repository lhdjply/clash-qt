#include "listener/HttpListener.h"
#include <QCoreApplication>

#include "base/Common.h"
#include "common/SocketUtil.h"

#include <QDateTime>
#include <QTcpSocket>
#include <QUrl>

namespace core {
namespace {

constexpr int kMaxHeadBytes  = 32 * 1024;
constexpr int kHeadTimeoutMs = 8000;

// 逐跳首部：不应转发给上游（RFC 7230 §6.1）
bool isHopByHopHeader(const QString &name)
{
    static const QStringList kHopByHop = {
        QStringLiteral("proxy-connection"), QStringLiteral("proxy-authorization"),
        QStringLiteral("proxy-authenticate"), QStringLiteral("connection"),
        QStringLiteral("keep-alive"),       QStringLiteral("te"),
        QStringLiteral("trailer"),          QStringLiteral("transfer-encoding"),
        QStringLiteral("upgrade"),
    };
    return kHopByHop.contains(name.trimmed().toLower());
}

} // namespace

HttpListener::HttpListener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent)
    : Listener(QString::fromLatin1(kType), std::move(name), std::move(tunnel), parent)
{
    setIdleTimeoutMs(0);
}

bool HttpListener::readRequestHead(QTcpSocket *client, QByteArray *rawHead, QByteArray *body,
                                   QString *error) const
{
    // 先取出已缓冲的数据（可能连请求体一起到达），再等待新数据。
    // 直接调用 waitForReadyRead 会在「数据已在内部缓冲」时误判为无数据而阻塞。
    QByteArray buffer = sockutil::drainAvailable(client);

    if (!sockutil::readUntil(client, &buffer, "\r\n\r\n", kHeadTimeoutMs, kMaxHeadBytes, error)) {
        return false;
    }

    const int headerEnd = buffer.indexOf("\r\n\r\n");
    *rawHead = buffer.left(headerEnd + 4);
    // 请求头之后若已带上请求体，需要单独保留并随请求一起转发出站
    *body = buffer.mid(headerEnd + 4);
    return true;
}

bool HttpListener::parseRequest(const QByteArray &rawHead, ParsedRequest *request,
                                QString *error) const
{
    const int firstLineEnd = rawHead.indexOf("\r\n");
    if (firstLineEnd <= 0) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Missing HTTP request line");
        return false;
    }

    const QByteArray requestLine = rawHead.left(firstLineEnd);
    const QList<QByteArray> parts = requestLine.split(' ');
    if (parts.size() < 3) {
        if (error) *error = QCoreApplication::translate("clashqt::core", "Malformed HTTP request line: %1")
                                .arg(QString::fromLatin1(requestLine.left(120)));
        return false;
    }

    request->method  = QString::fromLatin1(parts.at(0)).trimmed();
    request->target  = QString::fromUtf8(parts.at(1)).trimmed();
    request->version = QString::fromLatin1(parts.at(2)).trimmed();
    request->isConnect = request->method.compare(QLatin1String("CONNECT"), Qt::CaseInsensitive) == 0;

    // 逐行解析首部
    int cursor = firstLineEnd + 2;
    while (cursor < rawHead.size()) {
        const int lineEnd = rawHead.indexOf("\r\n", cursor);
        if (lineEnd < 0) break;
        if (lineEnd == cursor) break; // 空行，首部结束

        const QByteArray line = rawHead.mid(cursor, lineEnd - cursor);
        const int        colon = line.indexOf(':');
        if (colon > 0) {
            request->headers.append({QString::fromLatin1(line.left(colon)).trimmed(),
                                     QString::fromUtf8(line.mid(colon + 1)).trimmed()});
        }
        cursor = lineEnd + 2;
    }
    return true;
}

QString HttpListener::headerValue(const ParsedRequest &request, const QString &name)
{
    for (const auto &header : request.headers) {
        if (header.first.compare(name, Qt::CaseInsensitive) == 0) return header.second;
    }
    return QString();
}

QByteArray HttpListener::rebuildRequest(const ParsedRequest &request,
                                        const QString &originForm) const
{
    QByteArray out;
    out.append(request.method.toUtf8());
    out.append(' ');
    out.append(originForm.toUtf8());
    out.append(' ');
    out.append(request.version.toUtf8());
    out.append("\r\n");

    for (const auto &header : request.headers) {
        if (isHopByHopHeader(header.first)) continue;
        out.append(header.first.toUtf8());
        out.append(": ");
        out.append(header.second.toUtf8());
        out.append("\r\n");
    }
    // 强制短连接：响应结束即关闭，简化连接生命周期管理
    out.append("Connection: close\r\n\r\n");
    return out;
}

void HttpListener::handleConnection(QTcpSocket *clientSocket)
{
    QByteArray rawHead;
    QByteArray body;
    QString    error;

    if (!readRequestHead(clientSocket, &rawHead, &body, &error)) {
        Logger::instance().warn(LogCategory::Listener,
                                QCoreApplication::translate("clashqt::core", "HTTP inbound read failed: %1").arg(error));
        clientSocket->abort();
        return;
    }

    ParsedRequest request;
    if (!parseRequest(rawHead, &request, &error)) {
        static const QByteArray kBadRequest =
            "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        clientSocket->write(kBadRequest);
        clientSocket->flush();
        clientSocket->abort();
        return;
    }

    Metadata metadata;
    metadata.network     = QStringLiteral("tcp");
    metadata.inboundType = QStringLiteral("HTTP");
    metadata.srcIp       = clientSocket->peerAddress().toString();
    metadata.srcPort     = clientSocket->peerPort();
    metadata.inboundPort = port();
    metadata.inboundName = name();

    if (request.isConnect) {
        // ---- CONNECT：建立隧道 --------------------------------------------
        const int colon = request.target.lastIndexOf(QLatin1Char(':'));
        if (colon <= 0) {
            static const QByteArray kBadRequest =
                "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            clientSocket->write(kBadRequest);
            clientSocket->flush();
            clientSocket->abort();
            return;
        }

        metadata.host    = request.target.left(colon).trimmed();
        metadata.dstPort = request.target.mid(colon + 1).trimmed().toInt();

        if (!IpNet::parseIp(metadata.host)) {
            // 目标是域名，交由规则与出站处理
        } else {
            metadata.dstIp = metadata.host;
        }
        // CONNECT 的 authority 允许 IPv6 字面量 [::1]:443
        if (metadata.host.startsWith(QLatin1Char('[')) && metadata.host.endsWith(QLatin1Char(']'))) {
            metadata.host = metadata.host.mid(1, metadata.host.size() - 2);
            metadata.dstIp = metadata.host;
        }

        if (metadata.dstPort <= 0 || metadata.host.isEmpty()) {
            static const QByteArray kBadRequest =
                "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            clientSocket->write(kBadRequest);
            clientSocket->flush();
            clientSocket->abort();
            return;
        }

        static const QByteArray kEstablished =
            "HTTP/1.1 200 Connection established\r\nProxy-Agent: clash-qt\r\n\r\n";
        clientSocket->write(kEstablished);
        clientSocket->flush();

        if (tunnel()) tunnel()->handleStream(clientSocket, metadata);
        return;
    }

    // ---- 明文 HTTP：转发 ------------------------------------------------
    QString hostHeader = headerValue(request, QStringLiteral("Host"));
    if (hostHeader.isEmpty() && request.target.startsWith(QLatin1String("http://"),
                                                         Qt::CaseInsensitive)) {
        hostHeader = QUrl(request.target).authority();
    }
    if (hostHeader.isEmpty()) {
        static const QByteArray kBadRequest =
            "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        clientSocket->write(kBadRequest);
        clientSocket->flush();
        clientSocket->abort();
        return;
    }

    QString originForm = request.target;

    // 绝对 URI -> origin-form
    if (request.target.startsWith(QLatin1String("http://"), Qt::CaseInsensitive)
        || request.target.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)) {
        const QUrl url(request.target);
        originForm = url.path(QUrl::FullyEncoded);
        if (originForm.isEmpty()) originForm = QStringLiteral("/");
        if (url.hasQuery()) originForm += QLatin1Char('?') + url.query(QUrl::FullyEncoded);
    }

    // Host 首部可能带端口
    QString portText = hostHeader.section(QLatin1Char(':'), 1);
    if (hostHeader.startsWith(QLatin1Char('['))) {
        // IPv6 字面量：[::1]:8080
        const int bracket = hostHeader.indexOf(QLatin1Char(']'));
        if (bracket > 0) {
            metadata.host = hostHeader.mid(1, bracket - 1);
            const QString rest = hostHeader.mid(bracket + 1);
            if (rest.startsWith(QLatin1Char(':'))) portText = rest.mid(1);
        }
    } else {
        metadata.host = hostHeader.section(QLatin1Char(':'), 0, 0);
    }

    metadata.dstPort = portText.isEmpty() ? 80 : portText.toInt();
    if (metadata.dstPort <= 0) metadata.dstPort = 80;
    if (IpNet::parseIp(metadata.host)) metadata.dstIp = metadata.host;

    const QByteArray forwarded = rebuildRequest(request, originForm) + body;

    if (tunnel()) tunnel()->handleStream(clientSocket, metadata, forwarded);
}

} // namespace core
