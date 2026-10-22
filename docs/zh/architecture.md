# 架构

## 目录结构

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
├── scripts/         维护脚本（翻译同步、界面出图）
├── docs/            本文档
└── third_party/     git 子模块：yaml-cpp、ml-kem、sha3、randomshake、subtle
```

界面不会另起内核进程：`AppContext` 持有 `Hub`，侧栏上的每个开关最终都落到这个对象
上，入站连接由内核自己的线程在同一个进程里处理 —— 这也是退出时必须显式停掉监听
的原因（见 `AppContext::shutdown`）。

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
  `override.yaml`（覆写），三层各占一个文件，应用设置、订阅元数据与规则覆写互不混杂
- 配置生成链路：订阅 → 全局覆写 → 订阅级覆写 → 规则覆写 → 受控配置合并 → `work/config.yaml`
- 订阅管理：远程订阅下载与更新（解析 `subscription-userinfo` 流量/到期）、本地导入、切换
- 界面：瓦片式侧栏（出站模式、系统代理 / TUN 开关、当前订阅与用量、六格导航、实时速率）
  加代理 / 订阅 / 连接 / 规则 / 覆写 / 日志 / 流量（Qt Charts 曲线）/ 设置各页
- 系统集成：Linux 系统代理（gsettings 与 KDE kioslaverc）、XDG 自启动、系统托盘
- 检查更新：启动时自动查一次（设置页可关），设置页也可手动查询；展示更新日志
  并可跳转下载页（只提示，不自动下载/安装）
- 日志：应用与内核双通道落盘、按天滚动、按保留天数清理

## 尚未实现（按优先级）

| 模块 | 说明 |
|------|------|
| 出站协议 | VLESS 已实现（裸 TCP / TLS / WebSocket / REALITY + XTLS Vision，含 X25519MLKEM768 混合密钥交换）、Hysteria2 已实现（QUIC + HTTP/3 鉴权，见 [protocols.md](protocols.md)）；VMess / Trojan / Shadowsocks / Hysteria v1 / TUIC / WireGuard 等其余协议注册为「暂不支持」，选中时**明确报错而非静默直连**，避免流量绕过代理 |
| 外部控制器 | REST / WebSocket API（`external-controller`），供 zashboard 等第三方面板使用 |
| redir / tproxy | 透明代理入站（`SO_ORIGINAL_DST` / `IP_TRANSPARENT`）；TUN 模式已实现 |
| JS 覆写引擎 | 需要嵌入 JS 引擎（QuickJS 或 Qt Qml 的 QJSEngine）；当前遇 js 覆写会告警并跳过 |
| Geo 数据层 | geoip.metadb / geosite.dat 的读取（GEOIP / GEOSITE 规则依赖） |
| 远程规则集 | `type: http` 的 rule-provider 下载与缓存 |
| 其余 | 悬浮窗、WebDAV 备份、Sub-Store 集成、插件（.cpx）、Gist 同步；界面目前只有中英文 |
