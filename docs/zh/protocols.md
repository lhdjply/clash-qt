# 协议与兼容性

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

## 已用真实机场订阅验证的兼容性

使用一份 71 节点 / 9 代理组 / 约 2000 条规则的机场订阅做过端到端验证，
期间修复了四类真实数据暴露的兼容性问题：

| 问题 | 表现 | 修复 |
|------|------|------|
| 订阅 User-Agent | 机场按 UA 识别客户端，非 clash 标识返回精简配置并提示「不支持你的代理软件」 | 默认 UA 为 `clash-qt/v<版本> (clash.meta)`，沿用主流 clash 客户端的写法；可在设置中覆盖 |
| 代理组 `type: select` | 解析表只认 `selector`，导致 9 组仅 4 组建成 | `parseGroupType` 补齐 `select` / `urltest` / `loadbalance` / `smart` 等别名 |
| 组成员含内置策略 | `proxies: [REJECT, DIRECT]` 因 find() 查不到而整组跳过 | DIRECT / REJECT / REJECT-DROP / PASS 注册进名称表 |
| 逻辑规则子规则 | `AND,((DST-PORT,443),(NETWORK,UDP)),REJECT` 的子规则无目标策略，被判定字段不足 | 子规则补占位目标后继承外层策略 |

验证结论：内核正确加载全部节点与代理组；命中 DIRECT 规则的流量正常转发；
VLESS 组流量可正常出网（已在真实机场的 REALITY + Vision、VLESS + TLS + WebSocket
节点上验证）；Hysteria2 节点同样已跑通（见上）；未实现的协议则明确失败，
不会静默直连。
