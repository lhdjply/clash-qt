# 构建

## 依赖

- Qt 6.2+（Core / Network / Widgets / Sql / Svg / Charts）
- CMake 3.21+
- 支持 C++20 的编译器（GCC 11+ / Clang 13+）
- OpenSSL 3.2+（Hysteria2 用它的 QUIC 客户端 API）

```bash
# 首次克隆（需要 yaml-cpp / ml-kem 子模块）
git clone --recursive <repo>
# 已有克隆时
git submodule update --init --recursive

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/bin/clash-qt          # 唯一产物：图形界面（内核在其中）
```

`build/bin/clash-qt` 就是全部：内核是 OBJECT 库（`clashqt_core`），目标文件直接编进
可执行文件、进程内直调，所以没有单独的内核二进制或 `.a` 需要分发。

## Windows

Windows 没有 rpath，也没有 pkg-config，Qt 与 OpenSSL 只能靠显式前缀找到：

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_PREFIX_PATH=D:/Qt -DOPENSSL_ROOT_DIR=D:/OpenSSL
cmake --build build -j
```

Qt 必须是与该编译器配套的 MinGW 版本 —— MSVC 编的 Qt 无法和 MinGW 目标文件链接。
运行 `build/bin/clash-qt.exe` 前要把 `<Qt>/bin` 加进 `PATH`：Qt 的 DLL 和
`platforms/qwindows.dll` 插件都在那里。

Windows 上当前的能力：

| 功能 | 实现方式 |
|---|---|
| 内核（规则 / DNS / fake-ip / VLESS / Hysteria2 / HTTP+SOCKS 入站） | 与 Linux 相同；唯一的移植改动是把连接交给工作线程时用 `WSADuplicateSocket` 而不是 `dup()` |
| 系统代理 | `HKCU\...\Internet Settings`（ProxyEnable / ProxyServer / ProxyOverride）+ `InternetSetOption(INTERNET_OPTION_REFRESH)`，让已运行的程序立刻重读 |
| 开机自启动 | `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` |
| 崩溃记录 | `crash-<tid>.txt`（经 dbghelp 符号化的调用栈）+ `crash-<tid>.dmp`（minidump） |
| 界面里的内存占用 | `GetProcessMemoryInfo` 的工作集 |
| TUN 模式 | Wintun 适配器（运行时加载 `wintun.dll`）+ IP Helper 配地址与路由；只接管 fake-ip 网段，不动系统默认路由 |

TUN 模式在 Windows 上的两个前提：`wintun.dll` 必须与 `clash-qt.exe` 同目录（缺失时
开关会直接报错并说明原因），并且程序要以管理员身份运行（UAC 提权无法在进程内完成）。
收发的数据面在 Wintun 会话上跑，由专职读线程成批交给用户态协议栈；DNS 指向 TUN 地址
并在该地址上监听 53，与 Linux 侧等价。

另外两处 Windows 固有限制需要知道：`ProxyOverride` 不认 CIDR，忽略列表里只有 8 位和
16 位前缀能转成通配符（`10.*`、`192.168.*`），`/12` 这类跨段区间会被跳过；DNS 未做
NRPT，多网卡环境下不保证「只走 TUN 的 DNS」（只接管 fake-ip 网段时不影响连通性）。

## 依赖获取

`yaml-cpp` 的取用顺序：系统已安装（`find_package`）→ 内置子模块 → 报错并提示修复命令。
0.8 之前的发行版包只导出不带命名空间的 `yaml-cpp` 目标，CMakeLists 里按实际存在的
目标名选择，两者都能链接。

**配置与编译完全离线**：内核 ML-KEM 所需的 `sha3` / `randomshake` / `subtle`
三个 header-only 依赖同样是 `third_party/` 下的 git 子模块。CMake **不再使用
FetchContent**（它会每次配置都联网拉依赖，网络不通时 `cmake ..` 会一直卡住），
只挂载子模块的 `include` 目录；因此联网只在首次
`git submodule update --init --recursive` 时发生，之后配置与编译完全离线。

## 改名与版本号

**改软件名只改 `CMakeLists.txt`**：可执行文件名、CMake 目标名、数据目录
（`$XDG_CONFIG_HOME/<名称>`）、单实例标识、日志文件名前缀、TUN 辅助程序
（`<名称>-tun`）、TUN 接口名、桌面入口与图标名，全部由 `project()` 派生
（`CLASHQT_TARGET` / `CLASHQT_APP_NAME` / `CLASHQT_TUN_HELPER` 等），源码里
没有 `"clash-qt"` 字面量。显示名（`Clash Qt`）单独放在
`CLASHQT_APP_DISPLAY_NAME`，桌面入口是 `resources/linux/clash-qt.desktop.in`
模板，配置时生成。Debian 包名（`debian/control`）是发行版侧标识，改名时
仍需单独调整。

**改版本号只改根目录 `VERSION`**：写法与 Release tag 一致，带 `v` 前缀（如
`v1.0.0`）。CMake 在配置期读取它，剥掉前缀后作为 `project(VERSION)`，并注入两个
宏：`CLASHQT_VERSION`（纯数字，用于比较与订阅 UA）与 `CLASHQT_VERSION_TAG`
（带 v，用于 `--version` 与启动日志）。文件已登记为配置依赖，改完直接构建会
自动重跑 CMake，不必手工再 `cmake .`。

Debian 安装包的版本仍只能取 `debian/changelog`（dpkg 要求以数字开头，所以那里
写 `1.0.0`）；`CHANGELOG.md` 的小节标题与 `VERSION` 保持一致，写 `## [v1.0.0]`。
三者对不上时，配置阶段会针对 changelog 打一条 WARNING。
