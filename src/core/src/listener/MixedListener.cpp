#include "listener/MixedListener.h"
#include <QCoreApplication>

#include "base/Common.h"
#include "common/SocketUtil.h"

#include <QTcpSocket>

namespace core {
namespace {

constexpr int kSniffTimeoutMs = 5000;

} // namespace

MixedListener::MixedListener(QString name, std::shared_ptr<Tunnel> tunnel, QObject *parent)
    : Listener(QString::fromLatin1(kType), std::move(name), std::move(tunnel), parent)
{
    // 两个处理器只作为握手逻辑的载体，不会各自监听端口。
    // 它们共享本监听器的 Tunnel，因此组与规则行为完全一致。
    m_httpHandler  = std::make_shared<HttpListener>(QStringLiteral("MIXED/HTTP"), this->tunnel());
    m_socksHandler = std::make_shared<Socks5Listener>(QStringLiteral("MIXED/SOCKS"), this->tunnel());
}

void MixedListener::setCredentials(const QStringList &credentials)
{
    if (m_socksHandler) m_socksHandler->setCredentials(credentials);
}

void MixedListener::handleConnection(QTcpSocket *clientSocket)
{
    // 嗅探首字节：SOCKS5 一定以 0x05 开头。
    // 先看已缓冲的数据，避免「数据已在内部缓冲时 waitForReadyRead 误判为无数据」。
    QByteArray sniffed = sockutil::drainAvailable(clientSocket);
    if (sniffed.isEmpty()
        && !sockutil::readAtLeast(clientSocket, &sniffed, 1, kSniffTimeoutMs)) {
        Logger::instance().debug(LogCategory::Listener, QCoreApplication::translate("clashqt::core", "MIXED inbound timed out while sniffing the protocol"));
        clientSocket->abort();
        return;
    }

    // 把嗅探到的字节放回缓冲，交给协议处理器完整解析。
    // Qt 没有回灌接口，因此改用「取出后由处理器作为预读数据接收」的方式：
    // 这里通过 ungetChar 逐字节退回，保证处理器看到完整的首部。
    for (int i = sniffed.size() - 1; i >= 0; --i) {
        clientSocket->ungetChar(sniffed.at(i));
    }

    // 把真实监听端口同步给处理器，保证连接元数据中的 in-port 正确
    m_httpHandler->setInboundPort(port());
    m_socksHandler->setInboundPort(port());

    if (static_cast<unsigned char>(sniffed.at(0)) == 0x05) {
        m_socksHandler->processConnection(clientSocket);
    } else {
        m_httpHandler->processConnection(clientSocket);
    }
}

} // namespace core
