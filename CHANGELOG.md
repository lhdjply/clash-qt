# 更新日志

本项目的重要变更都记在这里。格式参考
[Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，
版本号遵循[语义化版本](https://semver.org/lang/zh-CN/)。
版本号的唯一来源是仓库根目录的 `VERSION` 文件（写法与 tag 一致，如 `v1.0.0`）。

## [v1.0.0] - 2026-10-19

首个正式版本。内核、界面、打包与更新链路都已打通，可以日常使用。

### 内核

- 自研 C++ 内核以 OBJECT 库方式编入应用：不驱动外部进程，不产出中间静态库，
  一次编译出一个可执行文件。
- 配置模型支持 YAML 往返（未知字段原样保留）、`deepMerge` 与 `<key>` / `key!` /
  `+key` 修饰语法。
- 规则引擎：DOMAIN / DOMAIN-SUFFIX / DOMAIN-KEYWORD / DOMAIN-REGEX / IP-CIDR(-6) /
  SRC-IP-CIDR / GEOIP / GEOSITE / IP-ASN / DST-PORT / SRC-PORT / IN-PORT / NETWORK /
  PROCESS-* / UID / RULE-SET / AND / OR / NOT / MATCH。
- DNS：hosts（含通配）、nameserver-policy、UDP / TCP / DoH 查询、缓存与 TTL、
  fake-ip 地址池与反查、fallback。
- 出站：DIRECT / REJECT / REJECT-DROP / HTTP / SOCKS5（含认证）、
  VLESS（REALITY、`xtls-rprx-vision`、WebSocket）、Hysteria2，以及
  Selector / URLTest / Fallback 代理组。
- 入站：HTTP（CONNECT + 明文转发）、SOCKS5、MIXED（同端口双协议嗅探）。
- TUN：自研 TCP/UDP 协议栈，fake-ip 域名还原、53 端口 DNS 劫持、路由接管、
  ICMP 应答、每连接独立线程拨号与并发上限。
- 运行期能力：切换模式与节点、节点测速、规则启停、连接查询与关闭、流量统计。

### 界面

- 代理 / 订阅 / 连接 / 规则 / 日志 / 流量 / 覆写 / 设置八个页面；流量页用
  Qt Charts 画实时曲线，网速文本按列宽自适应。
- 浅色与深色两套主题，可跟随桌面配色方案；侧栏与卡片为半透明玻璃质感。
- 订阅管理：远程订阅下载与更新（解析 `subscription-userinfo` 的流量与到期）、
  本地导入、编辑与切换；节点列表带协议类型标签，可单独测试延迟。
- 覆写：全局 / 订阅级 / 规则覆写，支持 prepend / append / delete / disabled 合并语义。
- 系统集成：Linux 系统代理（gsettings 与 KDE kioslaverc）、TUN 模式、XDG 自启动、
  系统托盘（图标颜色随接管方式变化：仅系统代理红、仅 TUN 绿、两者紫）。
- 开关记忆：系统代理与 TUN 的开关状态写入配置，下次启动自动恢复；退出时先把
  桌面上的系统代理撤成直连再停内核，避免整机指向已停止的本机端口。
- 检查更新：查询 GitHub Releases（API 优先，403 限流时自动改用 Atom），
  展示更新日志并可跳转下载页；只提示，不自动下载安装。
- 日志：应用与内核双通道落盘、按天滚动、按保留天数清理。

### 修复

- 开启 TUN 后 `ping` 域名无响应：域名解析成 fake-ip，没有隧道能承载 ICMP，
  改由协议栈在 fake-ip 上本地应答，真实 IP 则回 host unreachable。
- VLESS + REALITY + `xtls-rprx-vision` 节点上传较大数据被截断：Vision 切换到
  原样透传后，外层 TLS 记录层仍把内层密文当记录解密并直接断流，
  表现为 `git push` 报 `GnuTLS recv error (-110)`、大文件下载只收到几 KB。
- TUN 模式下上行 / 下行速度恒为 0：TUN 连接由协议栈自己拨号，不经过
  `Tunnel::handleStream`，流量没有汇入全局统计。
- 退出软件后系统代理仍指向本机端口，导致整机断网。
- 重启后系统代理 / TUN 开关丢失，需要手动再开一次。
- 开着 TUN 退出再启动，托盘图标停在默认色，要手动开关一次才变绿。

### 构建与发布

- 顶层单一 `CMakeLists.txt`，内核与应用共用一套构建配置。
- 版本号单一来源：根目录 `VERSION`，CMake 读取后注入 `CLASHQT_VERSION`；
  改完直接构建会自动重跑 CMake。与 `debian/changelog` 不一致时配置阶段告警。
- 单元测试 14 组（内核基础设施、DNS 编解码、fake-ip、规则、TUN 数据面、
  版本比较等），`ctest` 全绿。
- Debian 打包（`debian/`，3.0 (native)）与四架构 CI（amd64 / arm64 / loong64 /
  riscv64），推 tag 出正式 Release。

## [v0.1.0]

开发期版本，未单独发布；包含内核骨架、基础界面与首次 Debian 打包。

[v1.0.0]: https://github.com/lhdjply/clash-qt/releases/tag/v1.0.0
