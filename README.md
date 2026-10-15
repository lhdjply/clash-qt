# Clash Qt

clash-party 的 Qt6 / C++ 重写，包含**自研 C++ 内核**（随应用一同构建、目标文件直接编入应用、进程内直调）
与 **Qt6 图形界面**。

## 组件

```
clash-qt/
├── CMakeLists.txt   顶层唯一构建配置（内核 + 应用，src/ 下不再有 CMakeLists）
├── src/
│   ├── core/        自研 C++ 内核（OBJECT 库目标 clashqt_core，编入应用
│   │                而不单独产出 .a 或可执行文件）
│   │   ├── include/     按模块分目录：adapter / base / common / component /
│   │   │                config / constant / hub / listener / rules / tunnel
│   │   └── src/
│   └── app/         clash-qt —— Qt6 Widgets 图形界面
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

**配置与编译完全离线**：内核 ML-KEM 所需的 `sha3` / `randomshake` / `subtle`
三个 header-only 依赖同样是 `third_party/` 下的 git 子模块。CMake **不再使用
FetchContent**（它会每次配置都联网拉依赖，网络不通时 `cmake ..` 会一直卡住），
只挂载子模块的 `include` 目录；因此联网只在首次
`git submodule update --init --recursive` 时发生，之后配置与编译完全离线。

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
- 隧道：规则匹配 → 出站 → 双向中继，连接跟踪与流量统计
- 运行期接口：切模式、切节点、测延迟、规则启停、连接查询与关闭

### 应用（clash-qt）
- 配置体系：`config.yaml`（应用，其 `kernel:` 段为受控内核配置）/ `profile.yaml`（订阅）/
  `override.yaml`（覆写），目录布局与 clash-party 一致，可互相迁移
- 配置生成链路：订阅 → 全局覆写 → 订阅级覆写 → 规则覆写 → 受控配置合并 → `work/config.yaml`
- 订阅管理：远程订阅下载与更新（解析 `subscription-userinfo` 流量/到期）、本地导入、切换
- 界面：代理 / 订阅 / 连接 / 规则 / 日志 / 流量（Qt Charts 曲线）/ 设置
- 系统集成：Linux 系统代理（gsettings 与 KDE kioslaverc）、XDG 自启动、系统托盘
- 日志：应用与内核双通道落盘、按天滚动、按保留天数清理

## 尚未实现（按优先级）

| 模块 | 说明 |
|------|------|
| 出站协议 | VLESS 已实现（裸 TCP / TLS / WebSocket / REALITY + XTLS Vision，含
  X25519MLKEM768 混合密钥交换）；VMess / Trojan / Shadowsocks / Hysteria / TUIC /
  WireGuard 等其余协议注册为「暂不支持」，选中时**明确报错而非静默直连**，避免流量绕过代理 |
| 外部控制器 | REST / WebSocket API（`external-controller`），供 zashboard 等第三方面板使用 |
| TUN / redir / tproxy | 透明代理入站，需要内核网络特性与提权 |
| JS 覆写引擎 | 需要嵌入 JS 引擎（QuickJS 或 Qt Qml 的 QJSEngine）；当前遇 js 覆写会告警并跳过 |
| Geo 数据层 | geoip.metadb / geosite.dat 的读取（GEOIP / GEOSITE 规则依赖） |
| 远程规则集 | `type: http` 的 rule-provider 下载与缓存 |
| 其余 | 悬浮窗、WebDAV 备份、Sub-Store 集成、插件（.cpx）、Gist 同步、多语言 UI |

## 已用真实机场订阅验证的兼容性

使用一份 71 节点 / 9 代理组 / 约 2000 条规则的机场订阅做过端到端验证，
期间修复了四类真实数据暴露的兼容性问题：

| 问题 | 表现 | 修复 |
|------|------|------|
| 订阅 User-Agent | 机场按 UA 识别客户端，非 clash 标识返回精简配置并提示
  「不支持你的代理软件」 | 默认 UA 为 `clash-qt/v<版本> (clash.meta)`，
  与 clash-party 的策略一致；可在设置中覆盖 |
| 代理组 `type: select` | 解析表只认 `selector`，导致 9 组仅 4 组建成 |
  `parseGroupType` 补齐 `select` / `urltest` / `loadbalance` / `smart` 等别名 |
| 组成员含内置策略 | `proxies: [REJECT, DIRECT]` 因 find() 查不到而整组跳过 |
  DIRECT / REJECT / REJECT-DROP / PASS 注册进名称表 |
| 逻辑规则子规则 | `AND,((DST-PORT,443),(NETWORK,UDP)),REJECT` 的子规则无目标策略，
  被判定字段不足 | 子规则补占位目标后继承外层策略 |

验证结论：内核正确加载全部节点与代理组；命中 DIRECT 规则的流量正常转发；
走 VLESS 组的流量可正常出网（已在真实机场的 VLESS + TLS + WebSocket 节点上验证）；
未实现的协议则明确失败，不会静默直连。

## 已知实现差异

- **yaml 键顺序**：yaml-cpp 重新发射映射时不保序。YAML 映射键顺序无语义影响，
  功能不受影响；需要逐字节还原排版时需引入保序解析器。
- **DoT**：`tls://` DNS 服务器当前按明文 TCP 回退，日志中会提示。
  默认 DNS 配置因此改用 DoH。
- **明文 HTTP 转发**：统一按 `Connection: close` 处理，不做 keep-alive 复用。
- **线程模型**：入站连接为「每连接一线程 + 独立事件循环」（上限 512），
  后续将改为事件循环线程池 + 异步拨号。
