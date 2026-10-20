#pragma once

// ---------------------------------------------------------------------------
//  配置文件的原子写入
//
//  对应 clash-party 的 src/main/utils/safeFile.ts。
//
//  为什么必须原子写：配置由多个入口修改（界面、订阅更新、托盘、系统代理
//  切换），若直接覆盖写入，进程在写入中途退出或被并发写交错，会留下半截
//  YAML。下次启动解析失败，用户的所有配置即丢失。
//
//  实现方式：写入同目录下的临时文件 → fsync → 原子 rename 覆盖目标。
//  rename 在同一文件系统内是原子的，因此目标文件要么是旧内容，要么是新
//  内容，不存在中间态。
// ---------------------------------------------------------------------------

#include <QMutex>
#include <QString>

#include <functional>

namespace clashqt {

namespace safefile {

// 原子写入文本内容；成功返回 true，失败时 error 带原因
bool writeAtomic(const QString &path, const QString &content, QString *error = nullptr);

// 读取文本内容；文件不存在返回空字符串且 ok 为 false
QString readText(const QString &path, bool *ok = nullptr);

} // namespace safefile

// 串行化写入队列。
// 多个线程/多个入口可能同时改同一份配置，队列把它们排成序，
// 避免「读-改-写」交错导致的更新丢失。
class WriteQueue {
public:
    // 在队列中执行任务；任务返回值透传
    void run(const std::function<void()> &task);

private:
    QMutex m_mutex;
};

} // namespace clashqt
