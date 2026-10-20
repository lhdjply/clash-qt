# Clash Qt

[![构建](https://github.com/lhdjply/clash-qt/actions/workflows/build.yaml/badge.svg)](https://github.com/lhdjply/clash-qt/actions/workflows/build.yaml)
[![测试](https://github.com/lhdjply/clash-qt/actions/workflows/test.yaml/badge.svg)](https://github.com/lhdjply/clash-qt/actions/workflows/test.yaml)

[English](README.md) · **简体中文**

clash-party 的 Qt6 / C++ 重写，包含**自研 C++ 内核**（随应用一同构建、目标文件直接编入应用、进程内直调）
与 **Qt6 图形界面**。

## 组件

```
clash-qt/
├── CMakeLists.txt   顶层唯一构建配置（内核 + 应用，src/ 下不再有 CMakeLists）
├── VERSION          版本号唯一来源（改版本只改这里）
├── src/
│   ├── core/        自研 C++ 内核（OBJECT 库目标 clashqt_core，编入应用
│   │                而不单独产出 .a 或可执行文件）
│   │   ├── include/     按模块分目录：adapter / base / common / component /
│   │   │                config / constant / hub / listener / rules / tunnel
│   │   └── src/
│   └── app/         clash-qt —— Qt6 Widgets 图形界面
│       ├── include/     按模块分目录：config / i18n / sys / ui / updater /
│       │                utils（另有根目录下的 AppContext.h）
│       └── src/
├── tests/           单元测试（Qt Test + CTest，默认不参与构建）
└── third_party/     git 子模块：yaml-cpp、ml-kem、sha3、randomshake、subtle
```

## 构建

```bash
# 首次克隆（需要 yaml-cpp / ml-kem 子模块）
git clone --recursive <repo>
# 已有克隆时
git submodule update --init --recursive

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/bin/clash-qt          # 唯一产物：图形界面（内核在其中）
```

依赖：Qt 6.2+（Core / Network / Widgets / Sql / Svg / Charts）、CMake 3.21+、
支持 C++20 的编译器（GCC 11+ / Clang 13+）。

`yaml-cpp` 的取用顺序：系统已安装（`find_package`）→ 内置子模块 → 报错并提示修复命令。
0.8 之前的发行版包只导出不带命名空间的 `yaml-cpp` 目标，CMakeLists 里按实际存在的
目标名选择，两者都能链接。

**配置与编译完全离线**：内核 ML-KEM 所需的 `sha3` / `randomshake` / `subtle`
三个 header-only 依赖同样是 `third_party/` 下的 git 子模块。CMake **不再使用
FetchContent**（它会每次配置都联网拉依赖，网络不通时 `cmake ..` 会一直卡住），
只挂载子模块的 `include` 目录；因此联网只在首次
`git submodule update --init --recursive` 时发生，之后配置与编译完全离线。

**改软件名只改 `CMakeLists.txt`**：可执行文件名、CMake 目标名、数据目录
（`$XDG_CONFIG_HOME/<名称>`）、单实例标识、日志文件名前缀、TUN 辅助程序
（`<名称>-tun`）、TUN 接口名、桌面入口与图标名，全部由 `project()` 派生
（`CLASHQT_TARGET` / `CLASHQT_APP_NAME` / `CLASHQT_TUN_HELPER` 等），源码里
没有 `"clash-qt"` 字面量。显示名（`Clash Qt`）单独放在
`CLASHQT_APP_DISPLAY_NAME`，桌面入口是 `resources/clash-qt.desktop.in`
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

## 测试

单元测试默认关闭（日常构建不需要 Qt6::Test），单独开启：

```bash
cmake -S . -B build -DCLASHQT_BUILD_TESTS=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

测试可执行文件在 `build/bin/tests/`，也可单独跑某一个，例如
`./build/bin/tests/test_rules`。

| 用例 | 覆盖内容 |
|---|---|
| `test_common` | `IpNet` 解析/包含（含 IPv4 字节序回归）、字符串工具 |
| `test_domain_trie` | 域名后缀树的精确/后缀匹配、标签边界 |
| `test_cidr` | CIDR 集合与最长前缀优先 |
| `test_rules` | 规则解析与匹配、逻辑规则、`no-resolve`、规则集、停用 |
| `test_dns_codec` | DNS 报文编解码、名字压缩指针、损坏报文 |
| `test_fakeip` | fake-ip 池分配/复用/反查、回绕 |
| `test_yaml` | YAML ↔ JSON 往返、整数不被写成浮点、坏配置报错 |
| `test_json_util` | 深合并与 `<key>` / `key!` / `+key` 修饰语法 |
| `test_calc` | 流量格式化、单位进位、窄列收缩 |
| `test_paths` | 数据目录为 `<XDG_CONFIG_HOME>/<应用名>`（不嵌套）、旧布局迁移 |
| `test_rule_override` | 覆写页的 prepend / append / delete / disabled 合并语义 |
| `test_tun_icmp` | TUN 下 ICMP：fake-ip 回 echo reply、真实 IP 回 host unreachable |
| `test_tun_traffic` | TUN 流量统计上报：上下行字节要进 Tunnel 累计（否则界面速度恒为 0） |

这些用例只跑纯逻辑：不需要 root、不联网、不弹窗口，全部在 1 秒内跑完。
涉及配置目录的用例用临时 `XDG_CONFIG_HOME` 隔离，不会读写用户的真实配置。

## 界面翻译

界面文案的**源语言是英文**：源码里 `tr("...")` 的原文就是英文，中文译文放在
`translations/clash-qt_zh_CN.ts`。英文界面不加载翻译文件，`tr()` 原样返回，
所以译文缺失时只会显示英文，不会出现空白。

`.ts` 是译文的唯一来源，`.qm` 是构建产物（由 `lrelease` 从 `.ts` 生成并作为资源
嵌入），**不提交进仓库**。改了界面文案后同步 `.ts`：

```bash
python3 scripts/update-translations.py
```

脚本驱动 CMake 的 `update_translations` 目标 —— 源文件清单由 CMake 维护，
不会随着文件增删漏掉 `tr()`。新增或改动的条目会被标成「未翻译」，脚本末尾会
列出每份 `.ts` 的待补数量；补完译文重新构建即可。放进 CI 时可加 `--strict`，
存在未翻译条目就让构建失败。

语言在启动时确定（`QTranslator` 只影响其之后创建的控件），设置页改完会提示重启。

## Debian 打包

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

## 当前实现范围

### 内核（clashqt_core 目标）
- 配置模型与 YAML 往返（未知字段原样保留），`deepMerge` 与 `<key>` / `key!` / `+key` 修饰语法
- 规则引擎：DOMAIN / DOMAIN-SUFFIX / DOMAIN-KEYWORD / DOMAIN-REGEX / IP-CIDR(-6) / SRC-IP-CIDR /
  GEOIP / GEOSITE / IP-ASN / DST-PORT / SRC-PORT / IN-PORT / NETWORK / PROCESS-* / UID /
  RULE-SET / AND / OR / NOT / MATCH
- DNS：hosts（含通配）、nameserver-policy、UDP / TCP / DoH 查询、缓存与 TTL、
  fake-ip 地址池与反查、fallback
- 出站适配器：DIRECT / REJECT / REJECT-DROP / HTTP / SOCKS5（含认证）以及代理组
  （Selector / URLTest / Fallback 的选择与回退语义）
- 入站：HTTP（CONNECT + 明文转发）、SOCKS5（含 RFC 1929 认证）、MIXED（同端口双协议嗅探）
- 隧道：规则匹配 → 出站 → 双向中继（TCP 与 UDP），连接跟踪与流量统计
- UDP 中继：TUN 数据面按四元组建会话，首包触发拨号，DIRECT 直发 /
  VLESS `CMD=0x02`（隧道内 2 字节长度前缀分帧）两种承载；拨号在独立线程，
  失败目标 30 秒内直接回 ICMP 促使 QUIC 回落 TCP
- 运行期接口：切模式、切节点、测延迟、规则启停、连接查询与关闭

### 应用（clash-qt）
- 配置体系：`config.yaml`（应用，其 `kernel:` 段为受控内核配置）/ `profile.yaml`（订阅）/
  `override.yaml`（覆写），目录布局与 clash-party 一致，可互相迁移
- 配置生成链路：订阅 → 全局覆写 → 订阅级覆写 → 规则覆写 → 受控配置合并 → `work/config.yaml`
- 订阅管理：远程订阅下载与更新（解析 `subscription-userinfo` 流量/到期）、本地导入、切换
- 界面：代理 / 订阅 / 连接 / 规则 / 日志 / 流量（Qt Charts 曲线）/ 设置
- 系统集成：Linux 系统代理（gsettings 与 KDE kioslaverc）、XDG 自启动、系统托盘
- 检查更新：设置页一键查询 GitHub Releases，展示更新日志并可跳转下载页
  （只提示，不自动下载/安装）
- 日志：应用与内核双通道落盘、按天滚动、按保留天数清理

## 尚未实现（按优先级）

| 模块 | 说明 |
|------|------|
| 出站协议 | VLESS 已实现（裸 TCP / TLS / WebSocket / REALITY + XTLS Vision，含 X25519MLKEM768 混合密钥交换）、Hysteria2 已实现（QUIC + HTTP/3 鉴权，见下）；VMess / Trojan / Shadowsocks / Hysteria v1 / TUIC / WireGuard 等其余协议注册为「暂不支持」，选中时**明确报错而非静默直连**，避免流量绕过代理 |
| 外部控制器 | REST / WebSocket API（`external-controller`），供 zashboard 等第三方面板使用 |
| redir / tproxy | 透明代理入站（`SO_ORIGINAL_DST` / `IP_TRANSPARENT`）；TUN 模式已实现 |
| JS 覆写引擎 | 需要嵌入 JS 引擎（QuickJS 或 Qt Qml 的 QJSEngine）；当前遇 js 覆写会告警并跳过 |
| Geo 数据层 | geoip.metadb / geosite.dat 的读取（GEOIP / GEOSITE 规则依赖） |
| 远程规则集 | `type: http` 的 rule-provider 下载与缓存 |
| 其余 | 悬浮窗、WebDAV 备份、Sub-Store 集成、插件（.cpx）、Gist 同步；界面目前只有中英文 |

## 已用真实机场订阅验证的兼容性

使用一份 71 节点 / 9 代理组 / 约 2000 条规则的机场订阅做过端到端验证，
期间修复了四类真实数据暴露的兼容性问题：

| 问题 | 表现 | 修复 |
|------|------|------|
| 订阅 User-Agent | 机场按 UA 识别客户端，非 clash 标识返回精简配置并提示「不支持你的代理软件」 | 默认 UA 为 `clash-qt/v<版本> (clash.meta)`，与 clash-party 的策略一致；可在设置中覆盖 |
| 代理组 `type: select` | 解析表只认 `selector`，导致 9 组仅 4 组建成 | `parseGroupType` 补齐 `select` / `urltest` / `loadbalance` / `smart` 等别名 |
| 组成员含内置策略 | `proxies: [REJECT, DIRECT]` 因 find() 查不到而整组跳过 | DIRECT / REJECT / REJECT-DROP / PASS 注册进名称表 |
| 逻辑规则子规则 | `AND,((DST-PORT,443),(NETWORK,UDP)),REJECT` 的子规则无目标策略，被判定字段不足 | 子规则补占位目标后继承外层策略 |

验证结论：内核正确加载全部节点与代理组；命中 DIRECT 规则的流量正常转发；
VLESS 组流量可正常出网（已在真实机场的 REALITY + Vision、VLESS + TLS + WebSocket
节点上验证）；Hysteria2 节点同样已跑通（见下）；未实现的协议则明确失败，
不会静默直连。

## Hysteria2 支持说明

Hysteria2（`type: hysteria2` / `hy2`）不走 TCP，而是在 UDP 上跑 QUIC，因此与
VLESS 系列完全不同的一套协议栈，实现位于 `src/core/src/common/Hysteria2Client.cpp`：

| 层 | 实现 |
|----|------|
| QUIC | OpenSSL 3.2+ 自带的 QUIC 客户端 API（`OSSL_QUIC_client_method` + `BIO_new_dgram`），不引入第三方 QUIC 库 |
| TLS | QUIC 内置 TLS 1.3，ALPN 固定 `h3`；`skip-cert-verify` 与 `fingerprint`（证书 SHA-256 指纹固定）均支持 |
| 鉴权 | HTTP/3 最小子集：控制流 SETTINGS + `POST https://hysteria/auth`；QPACK 仅用静态表（动态表容量声明为 0），并按 RFC 7541 解码服务端 Huffman 字符串 |
| 隧道 | 每条目标连接一条双向流：`0x401`(varint) \| 地址长度 \| `host:port` \| 填充长度 \| 填充 |
| 端口跳跃 | `ports: 60000-63000` 形式，每次建连随机取一个端口 |
| 重试 | 网络类失败（握手/鉴权超时）在总超时预算内最多换端口重试 3 次；鉴权被拒等确定性失败不重试 |

**明确的边界**（不做静默降级）：

- **不支持 UDP 中继**：OpenSSL 3.2 尚未实现 QUIC DATAGRAM 帧，因此选中
  Hysteria2 节点时 UDP 会话会明确失败并回 ICMP（促使应用回落 TCP），
  不会退化为直连。UDP 中继本身在 TUN 数据面已实现（DIRECT 与 VLESS 均可承载），
  只有选中 Hysteria2 节点时 UDP 会话不可用。
- **不支持 Salamander 混淆（`obfs`）与 realm**：选中时给出明确原因。
- **不做流复用**：每条入站连接独立建立一条 QUIC 会话（因此新建连接比原生实现
  多一次握手往返，约几十毫秒量级）。这样每个连接的对象都归属自己的工作线程，
  无跨线程共享。后续若需降低首包延迟，可在不改协议层的前提下改为按节点共享
  会话 + 专用 I/O 线程。

## 已知实现差异

- **yaml 键顺序**：yaml-cpp 重新发射映射时不保序。YAML 映射键顺序无语义影响，
  功能不受影响；需要逐字节还原排版时需引入保序解析器。
- **DoT**：`tls://` DNS 服务器当前按明文 TCP 回退，日志中会提示。
  默认 DNS 配置因此改用 DoH。
- **明文 HTTP 转发**：统一按 `Connection: close` 处理，不做 keep-alive 复用。
- **线程模型**：入站连接为「每连接一线程 + 独立事件循环」（上限 512），
  后续将改为事件循环线程池 + 异步拨号。
- **流量统计**：系统代理的字节由中继里的钩子累加；TUN 的连接由协议栈自己拨号，
  不经过 `handleStream`，因此由 `TunIpStack` 通过 `setTrafficSink()` 主动上报到
  `Tunnel::addTraffic()`。两条路都汇到同一对原子计数，界面按 1 秒采样算速度。
- **检查更新**：数据源是 `api.github.com` 的发布列表，失败（未认证 API 配额按
  出口 IP 计，走公共节点常被 403）时自动退到 `releases.atom`。两条路都会跳过
  `continuous_build` 这类「tag 不是版本号」的发布与预发布版。请求经本机混合端口
  发出，因此开不开系统代理都能用；只展示更新日志与发布页链接，下载安装交给用户。
- **退出时的系统代理**：程序退出（关窗、托盘退出、SIGTERM）会先把系统代理恢复成
  直连，再停内核。判断依据是「桌面上的手动代理 http/https/socks 是否全部指向
  本机入站端口」—— 只有指向自己才动，用户自己的企业代理不会被误改。
  进程被 `SIGKILL` / 崩溃时来不及收尾，届时桌面上会残留一个指向空端口的代理。
- **开关记忆**：系统代理与 TUN 的状态分别记在 `sysProxy.enable` 与 `tun.enable`，
  启动时（事件循环起来、窗口显示之后）按记录恢复：TUN 会重新拉起数据面，系统
  代理会重新指向本机端口。恢复失败只提示，不改记录。退出时**只撤桌面上的系统
  代理设置，不改开关记录** —— 桌面设置是运行时状态，开关记录是用户意图。
  TUN 未授权时恢复会弹 pkexec 密码框，因此这一步不放在窗口出现之前。
- **ICMP（ping）**：系统代理模式下 ping 不经过代理（HTTP/SOCKS 没有 ICMP
  语义），能否 ping 通只取决于本机到目标的直连状态；TUN 模式下 ping 域名
  会由协议栈在 fake-ip 上本地应答（RTT 约 0.1ms，**不代表节点质量**），
  真实 IP 则立刻回 host unreachable。想看节点延迟请用代理页的测速按钮。

## 持续集成

工作流在 `.github/workflows/`，按职责拆成三个文件：

| 工作流 | 触发 | 做什么 |
|---|---|---|
| `build.yaml` | push 到 main/master、PR、手动 | 以 `RelWithDebInfo` 构建（不导出二进制产物） |
| `test.yaml` | push 到 main/master、PR、手动 | 构建全部目标（含测试二进制），跑 `ctest`；失败时用 `--rerun-failed -V` 重跑失败用例 |
| `release.yaml` | push 任意分支或 tag、PR、手动 | 在 deepin crimson 容器里用 QEMU 构建 **amd64 / arm64 / loong64 / riscv64** 四份 `.deb`；非 PR 时创建 GitHub Release 并附上 deb 与 SHA-256 |

几点约定：

- 三个工作流都带 `submodules: recursive`。yaml-cpp 与 ML-KEM 的四个 header-only
  依赖都是子模块，漏拉一个，CMake 配置阶段就会直接失败。
- `build.yaml` / `test.yaml` 跑在 `ubuntu-26.04`（Qt >= 6.5）。界面层用到
  `QStyleHints::colorScheme()`，Qt 6.4 的 `ubuntu-24.04` 会编译失败；工作流里有
  显式的版本检查，会直接说明「需要 Qt >= 6.5」而不是抛出一堆
  `no member named 'colorScheme'`。
- `release.yaml` 用 `linuxdeepin/deepin:crimson*` 镜像构建，保证打包环境与发行版
  一致（Qt、debhelper 版本都对得上），amd64 原生、其余三架构走 QEMU 仿真。
- 构建依赖只有 `debian/control` 一个来源：容器内用 `mk-build-deps` 从 control
  安装，不在工作流里再抄一份包名列表。
- `release.yaml` 的发布行为：推 tag 出正式 Release；推普通分支或手动触发会覆盖
  同名的 `continuous_build` 预发布（对应「每次提交都能下载安装包」）；PR 只跑
  四架构打包，不建 Release。
- Release 正文取 `CHANGELOG.md` 里对应 tag 的那一节（`## [v1.0.0]` 到下一个
  `## [` 之间），原样贴出；小节标题的 `v` 前缀可有可无，`v1.0.0` 与 `1.0.0`
  两种 tag 都能匹配。持续构建固定为一段说明 + 指向 `CHANGELOG.md` 的链接。
- 正式版发布前会对账，任一不满足就**直接失败**（`::error::`），不会发出版本号
  或更新日志对不上的 Release：
  * tag 与 `VERSION` 必须同版本（`v1.0.1` 配 `v1.0.0` 会被拦下）；
  * `CHANGELOG.md` 必须有对应小节（缺了就提示补 `## [v1.0.1]`）。
- 发布走 `debuild -us -uc -b`，不做 GPG 签名（CI 没有密钥）。`debian/source/format`
  是 `3.0 (native)`，版本号直接取 `debian/changelog`（当前为 `1.0.0`，不带 Debian
  修订号）；`-b` 只是为了跳过源码包、只出安装包。发版前记得让 changelog 的
  上游版本与根目录 `VERSION` 一致（`dch -v $(sed 's/^v//' VERSION)`）。
