#include "utils/SafeFile.h"

#include "utils/Paths.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutexLocker>
#include <QSaveFile>

namespace clashqt {
namespace safefile {

bool writeAtomic(const QString &path, const QString &content, QString *error)
{
    if (!paths::ensureParentDir(path)) {
        if (error) *error = QStringLiteral("无法创建目录：%1").arg(QFileInfo(path).absolutePath());
        return false;
    }

    // QSaveFile 内部即「写临时文件 + 成功提交时 rename」，并在提交前 fsync。
    // 相比手写临时文件更可靠：异常路径下会自动删除临时文件。
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) *error = QStringLiteral("无法写入 %1：%2").arg(path, file.errorString());
        return false;
    }

    const QByteArray payload = content.toUtf8();
    if (file.write(payload) != payload.size()) {
        if (error) *error = QStringLiteral("写入 %1 不完整：%2").arg(path, file.errorString());
        file.cancelWriting();
        return false;
    }

    if (!file.commit()) {
        if (error) *error = QStringLiteral("提交 %1 失败：%2").arg(path, file.errorString());
        return false;
    }
    return true;
}

QString readText(const QString &path, bool *ok)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (ok) *ok = false;
        return QString();
    }
    const QString content = QString::fromUtf8(file.readAll());
    if (ok) *ok = true;
    return content;
}

} // namespace safefile

void WriteQueue::run(const std::function<void()> &task)
{
    // 串行执行：同队列的任务不会交错，保证「读-改-写」序列的原子性
    QMutexLocker locker(&m_mutex);
    task();
}

} // namespace clashqt
