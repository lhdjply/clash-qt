#include "sys/SingleInstance.h"
#include <QCoreApplication>

#include "utils/AppLogger.h"

#include <QLocalServer>
#include <QLocalSocket>

namespace clashqt {

namespace {

// 探测已有实例的超时：本机回环套接字，300ms 足够；
// 取值过大只会拖慢「重复启动」时的退出速度。
constexpr int kProbeTimeoutMs = 300;

// 唤醒请求内容。当前只有一个动作，用定长字符串便于日后扩展。
constexpr char kActivateCommand[] = "activate";

} // namespace

SingleInstance::SingleInstance(const QString &key, QObject *parent)
    : QObject(parent), m_key(key)
{
}

bool SingleInstance::acquire()
{
    // ---- 先探测：能连上说明已有实例在运行 --------------------------------
    {
        QLocalSocket probe;
        probe.connectToServer(m_key);
        if (probe.waitForConnected(kProbeTimeoutMs)) {
            probe.write(kActivateCommand);
            probe.flush();
            probe.waitForBytesWritten(kProbeTimeoutMs);
            probe.disconnectFromServer();
            return false;
        }
    }

    // ---- 无存活实例：清理残留套接字文件后接管 ------------------------------
    // 上次异常退出（崩溃 / 被 kill -9）会留下套接字文件，导致 listen 报
    // AddressInUseError；上面的探测已确认没有进程在监听，可以安全移除。
    QLocalServer::removeServer(m_key);

    m_server = new QLocalServer(this);
    // 仅允许当前用户连接，避免同机其他用户误触发本实例
    m_server->setSocketOptions(QLocalServer::UserAccessOption);
    if (!m_server->listen(m_key)) {
        // 监听失败（例如运行时目录不可写）不应该让用户打不开程序：
        // 退化为多实例运行，只给出告警。
        AppLogger::instance().warn(QCoreApplication::translate("clashqt::app", "The single-instance guard failed to start (%1); running as multiple instances this time: %2")
                                       .arg(m_key, m_server->errorString()));
        m_server->deleteLater();
        m_server = nullptr;
        return true;
    }

    QObject::connect(m_server, &QLocalServer::newConnection, this,
                     &SingleInstance::onNewConnection);
    return true;
}

void SingleInstance::onNewConnection()
{
    while (QLocalSocket *socket = m_server->nextPendingConnection()) {
        QObject::connect(socket, &QLocalSocket::disconnected, socket,
                         &QLocalSocket::deleteLater);
        QObject::connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
            if (socket->readAll().contains(kActivateCommand)) {
                emit activationRequested();
            }
        });
    }
}

} // namespace clashqt
