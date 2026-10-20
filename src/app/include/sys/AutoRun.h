#pragma once

// ---------------------------------------------------------------------------
//  开机自启动
//
//  对应 clash-party 的 src/main/sys/autoRun.ts。
//
//  Linux 实现：写 XDG 自启动项 ~/.config/autostart/clash-qt.desktop。
//  使用 XDG 目录而不是 systemd user unit，原因：
//    * 无需 root，也不需要 dbus 会话权限
//    * 在 GNOME / KDE / XFCE 等所有遵循 XDG 的桌面上行为一致
//    * 卸载时只需删除一个文件，不留残留
// ---------------------------------------------------------------------------

#include <QObject>
#include <QString>

namespace clashqt {

class AutoRun : public QObject {
    Q_OBJECT

public:
    explicit AutoRun(QObject *parent = nullptr);

    // 是否已启用（检查自启动项是否存在且与当前可执行文件一致）
    bool isEnabled() const;

    // 启用自启动；silent 为 true 时追加 --silent 参数（静默启动到托盘）
    bool enable(bool silent, QString *error = nullptr);
    bool disable(QString *error = nullptr);

    // 自启动项的完整路径
    static QString desktopFilePath();

private:
    static bool writeDesktopFile(const QString &content, QString *error);
};

} // namespace clashqt
