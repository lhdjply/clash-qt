#include "adapter/VlessAdapter.h"

#include "base/Common.h"
#include "common/JsonUtil.h"
#include "common/SocketUtil.h"
#include "common/Tls13Client.h"
#include "common/WebSocketClient.h"
#include "tunnel/VisionStream.h"
#include "tunnel/WsStream.h"

#include <QEventLoop>
#include <QTcpSocket>
#include <QTimer>

#include <algorithm>



namespace core {
namespace {

// UUID 字符串 -> 16 字节（VLESS 协议要求二进制形式）。
// 兼容带连字符的标准写法；非法时返回 false。
bool parseUuid(const QString &text, unsigned char out[16])
{
    QString hex = text.trimmed().toLower();
    hex.remove(QLatin1Char('-'));
    if (hex.size() != 32) return false;

    bool ok = true;
    for (int i = 0; i < 16; ++i) {
        const QString byte = hex.mid(i * 2, 2);
        bool          converted = false;
        out[i] = static_cast<unsigned char>(byte.toUInt(&converted, 16));
        ok = ok && converted;
    }
    return ok;
}

// 构造 VLESS 请求：VER | UUID | ADDONS_LEN | ADDONS | CMD | PORT | ATYP | ADDR
// addons 为 protobuf 编码（vision 时含 flow 字段），可为空
QByteArray makeVlessRequest(const unsigned char uuid[16], const QByteArray &addons,
                            const QString &host, quint16 port, QString *error)
{
    QByteArray request;
    request.append(char(0x00)); // VER
    request.append(reinterpret_cast<const char *>(uuid), 16);
    request.append(char(addons.size()));
    request.append(addons);
    request.append(char(0x01)); // CMD = TCP
    request.append(char((port >> 8) & 0xFF));
    request.append(char(port & 0xFF));

    const QHostAddress literal(host);
    if (!host.isEmpty() && literal.isNull()) {
        const QByteArray domain = host.toUtf8();
        if (domain.size() > 255) {
            if (error) *error = QStringLiteral("目标域名过长");
            return {};
        }
        request.append(char(0x02)); // ATYP = 域名
        request.append(char(domain.size()));
        request.append(domain);
    } else if (literal.protocol() == QAbstractSocket::IPv4Protocol) {
        request.append(char(0x01));
        const quint32 v4 = literal.toIPv4Address();
        for (int shift = 24; shift >= 0; shift -= 8) {
            request.append(char((v4 >> shift) & 0xFF));
        }
    } else if (literal.protocol() == QAbstractSocket::IPv6Protocol) {
        request.append(char(0x03));
        const Q_IPV6ADDR v6 = literal.toIPv6Address();
        for (int i = 0; i < 16; ++i) request.append(char(v6.c[i]));
    } else {
        if (error) *error = QStringLiteral("目标地址无法解析：%1").arg(host);
        return {};
    }
    return request;
}

} // namespace


VlessAdapter::VlessAdapter(ProxyConfig config) : m_config(std::move(config))
{
    const QJsonObject &extra = m_config.extra;

    m_uuid       = jsonutil::stringValue(extra, QStringLiteral("uuid"));
    m_network    = jsonutil::stringValue(extra, QStringLiteral("network")).toLower();
    m_tls        = jsonutil::boolValue(extra, QStringLiteral("tls"), false);
    m_serverName = jsonutil::stringValue(extra, QStringLiteral("servername"));

    if (const QJsonObject wsOpts = extra.value(QStringLiteral("ws-opts")).toObject();
        !wsOpts.isEmpty()) {
        m_wsPath = jsonutil::stringValue(wsOpts, QStringLiteral("path"), QStringLiteral("/"));
        m_wsHost = jsonutil::stringValue(wsOpts, QStringLiteral("host"));
        if (const QJsonObject headers = wsOpts.value(QStringLiteral("headers")).toObject();
            !headers.isEmpty()) {
            // headers.Host 优先于 ws-opts.host（两者语义相同，前者更常见）
            const QString hostHeader = jsonutil::stringValue(headers, QStringLiteral("Host"));
            if (!hostHeader.isEmpty()) m_wsHost = hostHeader;
        }
    }

    // servername 缺省取服务器地址；WS 场景 Host 亦复用该值
    if (m_serverName.isEmpty()) m_serverName = m_config.server;
    if (m_wsHost.isEmpty()) m_wsHost = m_serverName;
}

// 明文 TCP 拨号（「ws 无 TLS」与「裸 tcp 无 TLS」两种形态共用）。
// 带 TLS 的形态一律走手工 TLS 1.3（RealityTlsStream，reality=false）：Qt 的
// QSslSocket 无法把「TCP 拨号地址」与「SNI / 证书校验名」分开，而 CDN 节点的
// servername 常与 server 解析到不同 IP；它也无法定制握手（REALITY 需要定制的
// ClientHello）。改为自有实现后三种 TLS 形态共用一条代码路径。
std::unique_ptr<QTcpSocket> VlessAdapter::connectPlainTcp(int timeoutMs, QString *error)
{
    if (m_config.server.isEmpty() || m_config.port <= 0) {
        if (error) *error = QStringLiteral("节点 %1 缺少 server/port").arg(m_config.name);
        return nullptr;
    }

    auto socket = std::make_unique<QTcpSocket>();
    QEventLoop loop;
    QTimer     timer;
    timer.setSingleShot(true);
    QObject::connect(socket.get(), &QTcpSocket::connected, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    socket->connectToHost(m_config.server, quint16(m_config.port));
    timer.start(std::max(1000, timeoutMs));
    loop.exec();

    if (socket->state() != QAbstractSocket::ConnectedState) {
        if (error) {
            *error = QStringLiteral("连接 %1:%2 失败：%3")
                         .arg(m_config.server)
                         .arg(m_config.port)
                         .arg(socket->errorString());
        }
        return nullptr;
    }
    return socket;
}

DialResult VlessAdapter::dial(const DialContext &context, int timeoutMs)
{
    DialResult result;

    if (context.network.compare(QLatin1String("udp"), Qt::CaseInsensitive) == 0) {
        // VLESS 的 UDP 承载（CMD=0x02）需要 addons 中的 XUDP 扩展，尚未实现
        result.error = QStringLiteral("VLESS UDP 尚未实现（Phase 2）");
        return result;
    }

    if (m_uuid.isEmpty()) {
        result.error = QStringLiteral("节点 %1 缺少 uuid").arg(m_config.name);
        return result;
    }

    unsigned char uuidBytes[16];
    if (!parseUuid(m_uuid, uuidBytes)) {
        result.error = QStringLiteral("节点 %1 的 uuid 非法：%2").arg(m_config.name, m_uuid);
        return result;
    }

    // ---- 目标与 flow / REALITY 判定 ----------------------------------------
    const QString host = context.host.isEmpty() ? context.dstIp : context.host;
    const QString flow = jsonutil::stringValue(m_config.extra, QStringLiteral("flow"));
    const QJsonObject realityOpts =
        m_config.extra.value(QStringLiteral("reality-opts")).toObject();

    if (!flow.isEmpty()) {
        // ---- XTLS Vision：当前支持 REALITY 传输（flow 要求 TLS1.3 外层） -----
        if (flow != QLatin1String("xtls-rprx-vision")) {
            result.error = QStringLiteral("节点 %1 使用不支持的 flow：%2")
                               .arg(m_config.name, flow);
            return result;
        }
        if (realityOpts.isEmpty()) {
            result.error = QStringLiteral("节点 %1 使用 vision 但未配置 reality-opts，"
                                          "该组合尚未实现").arg(m_config.name);
            return result;
        }

        RealityConfig reality;
        reality.server     = m_config.server;
        reality.port       = quint16(m_config.port);
        reality.serverName = m_serverName;
        // REALITY 公钥是 URL-safe base64（含 - 与 _）
        reality.publicKey  = QByteArray::fromBase64(
            jsonutil::stringValue(realityOpts, QStringLiteral("public-key")).toUtf8(),
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
        reality.shortId = QByteArray::fromHex(
            jsonutil::stringValue(realityOpts, QStringLiteral("short-id")).toUtf8());
        if (reality.publicKey.size() != 32) {
            result.error = QStringLiteral("节点 %1 的 reality-opts.public-key 非法"
                                          "（解码后应为 32 字节）").arg(m_config.name);
            return result;
        }
        if (reality.shortId.size() > 8) {
            result.error = QStringLiteral("节点 %1 的 reality-opts.short-id 过长")
                               .arg(m_config.name);
            return result;
        }

        QString realityError;
        auto tlsStream = RealityTlsStream::connect(reality, timeoutMs, &realityError);
        if (!tlsStream) {
            result.error = QStringLiteral("节点 %1 REALITY 握手失败：%2")
                               .arg(m_config.name, realityError);
            return result;
        }

        QByteArray uuidRaw(16, 0);
        for (int i = 0; i < 16; ++i) uuidRaw[i] = char(uuidBytes[i]);

        // addons：protobuf field 1（flow 字符串）
        static const QByteArray kVisionFlow("xtls-rprx-vision");
        QByteArray addons;
        addons.append(char(0x0A));
        addons.append(char(kVisionFlow.size()));
        addons.append(kVisionFlow);

        QString requestError;
        const QByteArray request =
            makeVlessRequest(uuidBytes, addons, host, quint16(context.dstPort),
                             &requestError);
        if (request.isEmpty()) {
            result.error = requestError;
            return result;
        }

        // 协议层次（对应 Xray/参照实现）：VLESS 请求头与应答头都是「裸帧」，
        // 只有其后的业务数据走 Vision 填充帧。参照实现中 vision 包在
        // vless.Conn 外层，写入顺序同样是「先请求头、再填充帧」。
        if (tlsStream->write(request) < 0) {
            result.error = QStringLiteral("节点 %1 VLESS 请求发送失败").arg(m_config.name);
            return result;
        }

        // 应答头不在此同步等待：服务端会在首次写出业务数据时才发送它，
        // 若在此阻塞会与「先发送请求体」的中继流程互相死锁。
        // 交由 VisionStream 在首次读取时懒消费（见 setLazyVlessResponseHeader）。
        auto vision = std::make_shared<VisionStream>(tlsStream, uuidRaw);
        vision->setLazyVlessResponseHeader(true);
        result.stream = vision;
        m_alive       = true;
        return result;
    }

    if (!m_network.isEmpty() && m_network != QLatin1String("ws")) {
        result.error =
            QStringLiteral("节点 %1 的传输 %2 尚未实现（当前支持 ws、REALITY 与裸 TCP）")
                .arg(m_config.name, m_network);
        return result;
    }

    // ---- 预构建 VLESS 请求（ws+TLS 与其余路径共用） ------------------------
    QString requestError;
    const QByteArray request =
        makeVlessRequest(uuidBytes, QByteArray(), host, quint16(context.dstPort),
                         &requestError);
    if (request.isEmpty()) {
        result.error = requestError;
        return result;
    }

    // ---- ws + TLS：手工 TLS 1.3（拨号地址与 SNI 分离）+ WebSocket 分帧 -----
    if (m_network == QLatin1String("ws") && m_tls) {
        RealityConfig tls;
        tls.server     = m_config.server;
        tls.port       = quint16(m_config.port);
        tls.serverName = m_serverName;
        tls.reality    = false;
        tls.skipVerify =
            jsonutil::boolValue(m_config.extra, QStringLiteral("skip-cert-verify"), false);

        QString tlsError;
        auto tlsStream = RealityTlsStream::connect(tls, timeoutMs, &tlsError);
        if (!tlsStream) {
            result.error = QStringLiteral("节点 %1 TLS 建连失败：%2")
                               .arg(m_config.name, tlsError);
            return result;
        }
        QString wsError;
        auto ws = std::make_shared<WsStream>(tlsStream, m_wsPath, m_wsHost, timeoutMs,
                                             &wsError);
        if (!ws->isOpen()) {
            result.error = QStringLiteral("节点 %1 WebSocket 握手失败：%2")
                               .arg(m_config.name, wsError);
            return result;
        }
        if (ws->write(request) < 0) {
            result.error = QStringLiteral("节点 %1 VLESS 请求发送失败").arg(m_config.name);
            return result;
        }
        // 应答头同样不在拨号阶段同步等待（服务端延迟到首次写数据时发送）
        ws->setLazyVlessResponseHeader(true);
        result.stream = ws;
        m_alive       = true;
        return result;
    }

    // ---- 裸 TCP + TLS：手工 TLS 1.3（拨号地址与 SNI 分离）----------------
    // 与 ws+TLS 分支共用同一套 TLS 实现，不再依赖 QSslSocket。
    if (m_tls) {
        RealityConfig tls;
        tls.server     = m_config.server;
        tls.port       = quint16(m_config.port);
        tls.serverName = m_serverName;
        tls.reality    = false;
        tls.skipVerify =
            jsonutil::boolValue(m_config.extra, QStringLiteral("skip-cert-verify"), false);

        QString tlsError;
        auto tlsStream = RealityTlsStream::connect(tls, timeoutMs, &tlsError);
        if (!tlsStream) {
            result.error = QStringLiteral("节点 %1 TLS 建连失败：%2")
                               .arg(m_config.name, tlsError);
            return result;
        }
        if (tlsStream->write(request) < 0) {
            result.error = QStringLiteral("节点 %1 VLESS 请求发送失败").arg(m_config.name);
            return result;
        }
        // 应答头懒消费（理由同 ws / REALITY 路径：服务端延迟到首次写数据才发送）
        tlsStream->setLazyVlessResponseHeader(true);
        result.stream = tlsStream;
        m_alive       = true;
        return result;
    }

    // ---- 明文 TCP（ws 无 TLS / 裸 tcp 无 TLS）-----------------------------
    auto socket = connectPlainTcp(timeoutMs, &result.error);
    if (!socket) return result;

    // ---- WebSocket 传输：升级握手 ----------------------------------------
    if (m_network == QLatin1String("ws")) {
        QString wsError;
        if (!websocket::handshake(socket.get(), m_wsPath, m_wsHost, timeoutMs, &wsError)) {
            result.error = QStringLiteral("节点 %1 WebSocket 握手失败：%2")
                               .arg(m_config.name, wsError);
            return result;
        }
    }

    // ---- 发送请求 ---------------------------------------------------------
    if (m_network == QLatin1String("ws")) {
        QString writeError;
        if (!websocket::sendBinary(socket.get(), request, &writeError)) {
            result.error = QStringLiteral("VLESS 请求发送失败：%1").arg(writeError);
            return result;
        }
    } else {
        if (!sockutil::writeAll(socket.get(), request, timeoutMs, &result.error)) {
            result.error = QStringLiteral("VLESS 请求发送失败：%1").arg(result.error);
            return result;
        }
    }

    // ---- 读取应答头 -------------------------------------------------------
    // 应答 = VER(1) + ADDONS_LEN(1) + ADDONS(n)。WebSocket 下首帧可能同时
    // 携带应答头与业务数据，因此先按帧取，再从帧负载中切出应答头。
    QByteArray extraData;
    if (m_network == QLatin1String("ws")) {
        auto wsStream = std::make_shared<WebSocketStream>(socket.release(), nullptr);

        // 帧读取需要阻塞语义：先手动从底层 socket 读出第一帧
        uchar   opcode = 0;
        QByteArray frame;
        QString    frameError;
        if (!websocket::readFrame(wsStream->underlyingSocket(), &opcode, &frame,
                                  timeoutMs, &frameError)) {
            result.error = QStringLiteral("VLESS 应答读取失败：%1").arg(frameError);
            return result;
        }
        if (frame.size() < 2) {
            result.error = QStringLiteral("VLESS 应答被截断");
            return result;
        }
        const int addonsLength = static_cast<unsigned char>(frame.at(1));
        if (frame.size() < 2 + addonsLength) {
            result.error = QStringLiteral("VLESS 应答 addons 被截断");
            return result;
        }
        extraData = frame.mid(2 + addonsLength);
        wsStream->feedDecoded(extraData);
        extraData.clear();

        result.stream = wsStream;
        m_alive = true;
        return result;
    }

    // 裸 TCP / TLS：按字节读取应答头（VER + ADDONS_LEN + ADDONS）
    QByteArray response;
    if (!sockutil::readAtLeast(socket.get(), &response, 2, timeoutMs, &result.error)) {
        result.error = QStringLiteral("VLESS 应答读取失败：%1").arg(result.error);
        return result;
    }
    const int addonsLength = static_cast<unsigned char>(response.at(1));
    if (addonsLength > 0
        && !sockutil::readAtLeast(socket.get(), &response, 2 + addonsLength, timeoutMs,
                                  &result.error)) {
        result.error = QStringLiteral("VLESS 应答 addons 读取失败：%1").arg(result.error);
        return result;
    }

    // 应答头之后若已夹带业务数据，交由中继先发给客户端；
    // 此后仍在内核缓冲中的数据由 TcpStream 的 readyRead 正常送达
    result.extraData = response.mid(2 + addonsLength);

    result.stream = std::make_shared<TcpStream>(socket.release(), nullptr);
    m_alive       = true;
    return result;
}

QJsonObject VlessAdapter::status() const
{
    QJsonObject object = Adapter::status();
    object.insert(QStringLiteral("server"), m_config.server);
    object.insert(QStringLiteral("port"), m_config.port);
    object.insert(QStringLiteral("network"),
                  m_network.isEmpty() ? QStringLiteral("tcp") : m_network);
    object.insert(QStringLiteral("tls"), m_tls);
    return object;
}

} // namespace core
