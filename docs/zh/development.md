# 开发

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

## 界面出图

`docs/images/main-*.png` 是从真实界面渲染出来的，不是截屏工具拍出来的：

```bash
python3 scripts/ui-snapshot.py              # 中文图 → docs/images
python3 scripts/ui-snapshot.py --lang en    # 英文图
```

脚本把 `scripts/ui_snapshot.cpp` 链接到应用**已经编好的目标文件**上（把
`main.cpp` 换成这个工具再链接一次），然后以 `QT_QPA_PLATFORM=offscreen` 运行：
不依赖显示服务，也不受窗口管理器影响，出图结果固定。工具通过点击真实的侧栏瓦片
切页，因此图里的选中态、徽标与布局都和用户看到的一致。

它跑在配置的**副本**上（订阅名与订阅链接会先替换成示例值），所以出图可以直接
提交，不会带出真实的机场或令牌。前提是应用已构建过一次，且当前没有 clash-qt
在运行 —— 内核要占用 7890-7892，端口被占时图里会显示内核未启动。

默认只出订阅页（README 用的就是这张）；加 `--pages all` 会连同代理 / 规则 / 覆写 /
设置一起出，那几张含你自己的节点与规则，不要提交。

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
