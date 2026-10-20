#include "tunnel/FlowWorker.h"
#include <QCoreApplication>

#include "tunnel/OutboundStream.h"

#include <QTimer>

namespace core {

FlowWorker::FlowWorker(DialFn dial, DialContext context, QObject *parent)
    : QObject(parent), m_dial(std::move(dial)), m_context(std::move(context))
{
}

FlowWorker::~FlowWorker()
{
    if (m_stream) {
        m_stream->closeStream();
        m_stream.reset();
    }
}

void FlowWorker::start()
{
    if (!m_dial) {
        emit failed(QCoreApplication::translate("clashqt::core", "No dialer implementation available"));
        return;
    }

    DialResult result = m_dial(m_context);
    if (!result.ok() || !result.stream) {
        emit failed(result.error.isEmpty() ? QCoreApplication::translate("clashqt::core", "Dial failed") : result.error);
        return;
    }

    m_stream = result.stream;

    QObject::connect(m_stream.get(), &OutboundStream::readyToRead, this, [this]() {
        if (!m_stream) return;
        const QByteArray data = m_stream->readAll();
        if (!data.isEmpty()) emit dataReady(data);
    });
    QObject::connect(m_stream.get(), &OutboundStream::streamClosed, this, [this]() {
        if (m_closing) return;
        m_closing = true;
        emit closed();
    });

    emit connected();
}

void FlowWorker::writeData(const QByteArray &data)
{
    if (m_closing || !m_stream || !m_stream->isOpen()) return;
    m_pending.append(data);
    flushWrites();
}

void FlowWorker::flushWrites()
{
    while (!m_pending.isEmpty() && m_stream && m_stream->isOpen() && !m_closing) {
        const qint64 written = m_stream->write(m_pending);
        if (written <= 0) break; // 上游暂时写不动，等下一轮
        m_pending.remove(0, int(written));
    }
    if (m_pending.isEmpty()) return;

    // OutboundStream 没有「可写」信号，只能稍后重试
    QTimer::singleShot(20, this, [this]() { flushWrites(); });
}

void FlowWorker::closeStream()
{
    m_closing = true;
    m_pending.clear();
    if (m_stream) {
        m_stream->closeStream();
        m_stream.reset();
    }
}

} // namespace core
