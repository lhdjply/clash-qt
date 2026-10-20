#pragma once

// ---------------------------------------------------------------------------
//  单实例守卫
//
//  通过本地套接字（QLocalServer / QLocalSocket）保证同一时间只有一个界面
//  实例在运行：
//    · 首个实例 listen 成功，成为主实例；
//    · 后续实例探测到主实例的套接字后，发送一次「activate」请求并立即退出；
//    · 主实例收到请求后把窗口带到前台（见 activationRequested 信号）。
//
//  为什么不用 QLockFile：本机可能同时跑便携版与常规版，套接字方案既能互斥
//  又能把「用户又点了一次图标」转换成「唤醒已有窗口」，符合桌面应用惯例。
//
//  注意：这里的「实例」指界面进程。内嵌内核运行在同一进程内，因此单实例
//  同时也意味着不会出现两个内核争抢混合端口。
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>

class QLocalServer;

namespace clashqt {

class SingleInstance : public QObject {
    Q_OBJECT

public:
    explicit SingleInstance(const QString &key, QObject *parent = nullptr);

    // 尝试成为唯一实例：
    //   true  —— 本进程为主实例，正常继续启动；
    //   false —— 已有实例在运行（已通知其唤醒窗口），调用方应立即退出。
    bool acquire();

signals:
    // 另一个实例请求把界面带到前台
    void activationRequested();

private:
    void onNewConnection();

    QString       m_key;
    QLocalServer *m_server = nullptr;
};

} // namespace clashqt
