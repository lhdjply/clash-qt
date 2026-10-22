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

## 便携包（tar.gz）

发布页除 deb 之外还带一个 `clash-qt-<版本>-linux-<架构>.tar.gz`：解压即用，不需要
root，也不要求目标机装过 Qt。脚本是 `scripts/package-linux-tarball.sh`：

```bash
# 用已打好的 deb 出包（CI 走的就是这条，二进制与安装包完全一致）
scripts/package-linux-tarball.sh --arch amd64 --from-deb clash-qt_1.0.0_amd64.deb

# 或直接从构建目录出包（本地调试用；--strip 去掉调试信息）
scripts/package-linux-tarball.sh --arch amd64 --bin-dir build/bin --strip
```

需要 `patchelf`，产物落在 `dist/`。四个架构由 CI 在各自容器里各出一份。

包内布局：

| 路径 | 说明 |
|---|---|
| `clash-qt` | 入口脚本：把 `lib/` 顶到动态库搜索路径最前面，再交给 `bin/clash-qt` |
| `bin/clash-qt` | 主程序，自带 RUNPATH `$ORIGIN/../lib` |
| `bin/clash-qt-tun` | TUN 辅助程序，必须与主程序同目录；包里不带 setuid |
| `bin/qt.conf` | 告诉 Qt 去 `../plugins`、`../translations` 找插件与翻译 |
| `lib/` | 随包的共享库（Qt、OpenSSL、ICU、xcb 等） |
| `plugins/` | 平台插件（xcb / wayland / offscreen）、图标引擎、图片格式、TLS、输入法 |
| `translations/` | Qt 自带中文翻译（标准对话框、右键菜单） |
| `share/` | `.desktop` 与图标，供自行装进 `~/.local/share` |
| `LICENSE`、`README*.md` | 许可证与说明 |

```bash
tar xzf clash-qt-1.0.0-linux-amd64.tar.gz
./clash-qt-1.0.0-linux-amd64/clash-qt
```

**为什么要有入口脚本**：Qt 插件是用 `dlopen` 拉起来的，插件自身的依赖不走可执行
文件的 RUNPATH；而直接给插件加 RUNPATH 会破坏 `.note.qt.metadata` 段（Qt 6 从这段
读插件元数据），插件随即被当成「不是 Qt 插件」丢掉，报错却是
`Could not find the Qt platform plugin "xcb"`。用 `LD_LIBRARY_PATH` 是既能覆盖插件、
又不必改动插件文件的唯一办法，入口脚本干的就这一件事。

**哪些库不带**：glibc 与图形驱动（libGL / EGL / GLX / drm / vulkan）用宿主的 ——
前者与动态加载器成对编译，后者必须与机器上的显卡驱动配套。其余（含 libX11 / xcb、
ICU、freetype）都随包带一份：Qt 6.5+ 的 xcb 插件依赖 `libxcb-cursor`，老发行版没有
这个库，不带上插件就起不来。

**TUN 模式**：包里不带 setuid 位。首次开启 TUN 时应用自己用 `pkexec` 给
`bin/clash-qt-tun` 补 `root:root` + `sx`（需要 polkit，会弹密码框）；不授权也不影响
其余功能，只是 TUN 开关用不了。

**兼容性**：二进制在 deepin crimson 容器里构建，所以要求目标机的 glibc 不低于该
发行版（deepin 25 / Debian trixie 这一代）。更老的系统请用源码构建或装 deb。

**桌面集成**：`share/applications/clash-qt.desktop` 的 `Exec` 只有软件名，而解压位置
不固定，所以要先改一行再放进 `~/.local/share/applications`：

```bash
sed "s|^Exec=.*|Exec=$PWD/clash-qt-1.0.0-linux-amd64/clash-qt|" \
    clash-qt-1.0.0-linux-amd64/share/applications/clash-qt.desktop \
    > ~/.local/share/applications/clash-qt.desktop
```

想把配置也放在解压目录里（U 盘、多版本并存），在 `bin/` 下建一个空文件 `PORTABLE`
即可，应用会改用 `bin/data/` 而不是 `~/.config/clash-qt/`。

