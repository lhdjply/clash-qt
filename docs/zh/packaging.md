# Debian 打包

打包配置在 `debian/`，构建走 debhelper 的 cmake 序列，与本地构建共用同一套
`CMakeLists.txt`（安装路径由 `GNUInstallDirs` 提供）。

```bash
# 构建依赖
sudo apt install build-essential debhelper cmake ninja-build \
     qt6-base-dev qt6-charts-dev qt6-svg-dev \
     qt6-tools-dev qt6-l10n-tools \
     libssl-dev libyaml-cpp-dev

# 生成二进制包（产物在上级目录）
dpkg-buildpackage -us -uc -b

# 安装
sudo apt install ../clash-qt_$(sed 's/^v//' VERSION)_amd64.deb
```

`qt6-tools-dev` 与 `qt6-l10n-tools` 两个都要装：前者提供 `Qt6LinguistTools` 的
CMake 包（没有它 `find_package` 找不到 `Qt6::lrelease`），后者提供 `lrelease` /
`lupdate` 可执行文件。缺任一个都不会报错，只是界面静默变成纯英文。

注意：debhelper 的 clean 步骤会重建 `build/`（`dh_auto_clean` 直接删掉
out-of-source 构建目录），本地已有的编译产物会被清空，打包前不必手工清理。

包内布局：

| 路径 | 说明 |
|---|---|
| `/usr/bin/clash-qt` | 主程序（GUI + 内置内核），普通用户运行 |
| `/usr/bin/clash-qt-tun` | TUN 辅助程序，`4755 root:root` |
| `/usr/share/applications/clash-qt.desktop` | 桌面入口 |
| `/usr/share/icons/hicolor/scalable/apps/clash-qt.svg` | 应用图标 |
| `/usr/share/doc/clash-qt/` | README（英文 + 中文）、copyright、README.Debian |

辅助程序与主程序必须同目录：`TunController` 按
`applicationDirPath()/clash-qt-tun` 查找它，分开放会导致 TUN 无法启用。

**setuid 的取舍**：创建 TUN 需要 `CAP_NET_ADMIN`。项目把提权动作压在一个
不链接 Qt 的极小程序里（只建 TUN、配地址、通过 `SCM_RIGHTS` 传 fd），主程序
始终以普通用户运行。若不想保留 setuid，`sudo chmod 0755 /usr/bin/clash-qt-tun`
后首次开启 TUN 会走 pkexec 授权流程，功能不受影响（升级后会被 `postinst`
恢复）。细节见 `debian/README.Debian`。

卸载不会删除 `~/.config/clash-qt/`（订阅与自定义规则），需要清理时手动删除。
