# 更新日志

本项目的重要变更都记在这里。格式参考
[Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，
版本号遵循[语义化版本](https://semver.org/lang/zh-CN/)。
版本号的唯一来源是仓库根目录的 `VERSION` 文件（写法与 tag 一致，如 `v1.0.0`）。

## [v1.0.1] - 2026-10-19

首个补丁版本：节点选择完全交给手动，日志页与连接页不再缺数据，检查更新在 TUN 模式下可用。

### 界面

- 代理页新增「排序」：默认顺序 / 名称 / 类型 / 延迟。名称用 `QCollator` 做数字感知、
  大小写不敏感的比较；类型先按协议再按名称；延迟升序，未测速的排在已测速之后、
  超时最后，同延迟再按名称稳定排序。排序只影响显示，不改变当前选中节点，
  模式写入配置，重启后保持。
- 移除节点自动切换：删除「批量测速后按最低延迟重选」「无固定选择时自动挑最优」
  「拨号失败静默换成员」三条路径，并去掉界面上的「恢复自动」按钮。节点只在
  双击切换与启动恢复时写入；选定节点不可用时连接直接失败并记日志，不再私自跳走。
- 日志页不再丢弃非前台期间产生的日志：改为与页面可见性无关的环形缓冲
  （上限 4000 条），切回页面时按当前等级整屏重建；可见期间每 80ms 合并插入一次，
  TUN 大流量下不会逐行拖慢界面；切换等级保留历史，「清空显示」同时清空缓冲。
- 设置页主题下拉框不再响应鼠标滚轮：事件继续冒泡给页面滚动区，划过只滚动页面，
  不会把主题改掉。
- 设置页「关于」卡片补上图标（卡片标题映射表里缺这个标题，表头只剩文字）。

### 修复

- 更新日志窗口显示异常：Atom 备用源返回的是 HTML，之前一律按 Markdown 解析，
  标题与列表被压成一段纯文本；现在按来源选择 `setHtml` / `setMarkdown`，
  并为 HTML 补上正文、标题、代码与链接的主题样式。
- TUN 模式下连接页始终为空：TUN 流量由用户态协议栈自己拨号，不经过
  `Tunnel::handleStream`，连接从未登记。现在在 TCP SYN 与 UDP 会话建立时登记连接，
  拨号结束后回填命中的规则与出站链，上下行字节按连接累加，FIN/RST、上游关闭、
  拨号失败与空闲回收时关闭条目。
- TUN 模式下检查更新频繁失败：出口 IP 的 GitHub API 额度可能被同 IP 的其他用户
  耗尽而返回 403。改为 Atom 优先、API 兜底；仅对超时 / 提前断开 / 5xx 重试一次；
  单次超时 6 秒、总预算 9 秒；收到 `x-ratelimit-remaining: 0` 后在该窗口内跳过 API。
- 首次检查更新耗时偏长：链路偶发丢包会触发 TCP 重传，单条请求可能卡住数秒。
  现在首条发出后每 700ms 无结果就并行补发一条（同源最多 3 条），先到者胜、
  其余取消。

### 构建与发布

- 测试保持 14 组，新增 `test_tun_traffic::tcpConnectionIsTracked`（TUN 连接登记、
  规则回填、流量累加与关闭）与 `stalledRequestIsHedged`（请求停滞时的对冲）用例。

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

[v1.0.1]: https://github.com/lhdjply/clash-qt/releases/tag/v1.0.1
[v1.0.0]: https://github.com/lhdjply/clash-qt/releases/tag/v1.0.0
