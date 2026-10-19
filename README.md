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

**配置与编译完全离线**：内核 ML-KEM 所需的 `sha3` / `randomshake` / `subtle`
三个 header-only 依赖同样是 `third_party/` 下的 git 子模块。CMake **不再使用
FetchContent**（它会每次配置都联网拉依赖，网络不通时 `cmake ..` 会一直卡住），
只挂载子模块的 `include` 目录；因此联网只在首次
`git submodule update --init --recursive` 时发生，之后配置与编译完全离线。

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
| `test_rule_override` | 覆写页的 prepend / append / delete / disabled 合并语义 |

这些用例只跑纯逻辑：不需要 root、不联网、不弹窗口，全部在 1 秒内跑完。
涉及配置目录的用例用临时 `XDG_CONFIG_HOME` 隔离，不会读写用户的真实配置。

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
- 日志：应用与内核双通道落盘、按天滚动、按保留天数清理

## 尚未实现（按优先级）

| 模块 | 说明 |
|------|------|
| 出站协议 | VLESS 已实现（裸 TCP / TLS / WebSocket / REALITY + XTLS Vision，含
  X25519MLKEM768 混合密钥交换）、Hysteria2 已实现（QUIC + HTTP/3 鉴权，见下）；
  VMess / Trojan / Shadowsocks / Hysteria v1 / TUIC / WireGuard 等其余协议注册为
  「暂不支持」，选中时**明确报错而非静默直连**，避免流量绕过代理 |
| 外部控制器 | REST / WebSocket API（`external-controller`），供 zashboard 等第三方面板使用 |
| redir / tproxy | 透明代理入站（`SO_ORIGINAL_DST` / `IP_TRANSPARENT`）；TUN 模式已实现 |
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
