# Clash Qt

[![Build](https://github.com/lhdjply/clash-qt/actions/workflows/build.yaml/badge.svg)](https://github.com/lhdjply/clash-qt/actions/workflows/build.yaml)
[![Tests](https://github.com/lhdjply/clash-qt/actions/workflows/test.yaml/badge.svg)](https://github.com/lhdjply/clash-qt/actions/workflows/test.yaml)

**English** · [简体中文](README-zh.md)

A Qt6 / C++ rewrite of clash-party, containing a **self-contained C++ kernel** (built
together with the app, its object files linked straight into the executable and called
in-process) and a **Qt6 GUI**.

## Layout

```
clash-qt/
├── CMakeLists.txt   the only top-level build config (kernel + app; no per-dir
│                    CMakeLists under src/)
├── VERSION          single source of truth for the version (bump it here only)
├── src/
│   ├── core/        the C++ kernel (OBJECT library target clashqt_core, linked
│   │                into the app rather than shipped as a .a or an executable)
│   │   ├── include/     one directory per module: adapter / base / common /
│   │   │                component / config / constant / hub / listener /
│   │   │                rules / tunnel
│   │   └── src/
│   └── app/         clash-qt — the Qt6 Widgets GUI
├── tests/           unit tests (Qt Test + CTest, off by default)
└── third_party/     git submodules: yaml-cpp, ml-kem, sha3, randomshake, subtle
```

## Building

```bash
# first clone (needs the yaml-cpp / ml-kem submodules)
git clone --recursive <repo>
# in an existing clone
git submodule update --init --recursive

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/bin/clash-qt          # the only artifact: the GUI (kernel included)
```

Requirements: Qt 6.2+ (Core / Network / Widgets / Sql / Svg / Charts), CMake 3.21+, and
a C++20 compiler (GCC 11+ / Clang 13+).

`yaml-cpp` is picked up in this order: system install (`find_package`) → bundled
submodule → error with a hint on how to fix it. Distro packages older than 0.8 only
export the un-namespaced `yaml-cpp` target; CMakeLists picks whichever target actually
exists, so both link.

**Configure and compile fully offline**: the kernel's ML-KEM needs `sha3` /
`randomshake` / `subtle`, three header-only dependencies that are also git submodules
under `third_party/`. CMake **no longer uses FetchContent** (it went to the network on
every configure, and `cmake ..` would hang whenever the network was down) — it only adds
the submodules' `include` directories, so the network is touched once, during
`git submodule update --init --recursive`, and never again.

**To rename the app, edit `CMakeLists.txt` only**: the executable name, CMake target
name, data directory (`$XDG_CONFIG_HOME/<name>`), single-instance ID, log file prefix,
TUN helper (`<name>-tun`), TUN interface name, desktop entry and icon name are all
derived from `project()` (`CLASHQT_TARGET` / `CLASHQT_APP_NAME` / `CLASHQT_TUN_HELPER`
and friends); there is no `"clash-qt"` literal in the sources. The display name
(`Clash Qt`) lives separately in `CLASHQT_APP_DISPLAY_NAME`, and the desktop entry is
generated at configure time from the `resources/clash-qt.desktop.in` template. The
Debian package name (`debian/control`) is a distro-side identifier and still has to be
changed by hand.

**To bump the version, edit `VERSION` at the repo root only**: same form as the release
tag, with a `v` prefix (e.g. `v1.0.0`). CMake reads it at configure time, strips the
prefix for `project(VERSION)` and injects two macros: `CLASHQT_VERSION` (digits only,
used for comparisons and the subscription UA) and `CLASHQT_VERSION_TAG` (with the `v`,
used by `--version` and the startup log). The file is registered as a configure
dependency, so a plain rebuild re-runs CMake after an edit — no manual `cmake .`.

The Debian package version can still only come from `debian/changelog` (dpkg requires it
to start with a digit, hence `1.0.0` there); the `CHANGELOG.md` section headings match
`VERSION` and read `## [v1.0.0]`. A configure-time WARNING fires when the three disagree.

## Tests

Unit tests are off by default (a day-to-day build doesn't need Qt6::Test); enable them
explicitly:

```bash
cmake -S . -B build -DCLASHQT_BUILD_TESTS=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

The test binaries land in `build/bin/tests/`; you can also run a single one, e.g.
`./build/bin/tests/test_rules`.

| Case | Coverage |
|---|---|
| `test_common` | `IpNet` parsing/containment (including an IPv4 byte-order regression), string helpers |
| `test_domain_trie` | Exact/suffix matching in the domain trie, label boundaries |
| `test_cidr` | CIDR sets and longest-prefix-first matching |
| `test_rules` | Rule parsing and matching, logical rules, `no-resolve`, rule sets, disabling |
| `test_dns_codec` | DNS message encode/decode, name compression pointers, corrupt packets |
| `test_fakeip` | fake-ip pool allocation/reuse/reverse lookup, wraparound |
| `test_yaml` | YAML ↔ JSON round-trip, integers not emitted as floats, bad configs reported |
| `test_json_util` | Deep merge and the `<key>` / `key!` / `+key` modifier syntax |
| `test_calc` | Traffic formatting, unit carry, narrow-column shrinking |
| `test_paths` | Data directory is `<XDG_CONFIG_HOME>/<app name>` (not nested), old layout migration |
| `test_rule_override` | prepend / append / delete / disabled merge semantics on the overrides page |
| `test_tun_icmp` | ICMP under TUN: echo reply for a fake-ip, host unreachable for a real IP |
| `test_tun_traffic` | TUN traffic reporting: up/down bytes must reach the Tunnel counters (otherwise the UI speed reads 0 forever) |

These cases only exercise pure logic: no root, no network, no windows, and all of them
finish within a second. Cases that touch the config directory isolate themselves with a
temporary `XDG_CONFIG_HOME` and never read or write your real config.

## UI translations

The **source language of the UI strings is English**: the `tr("...")` literals in the
sources are English, and the Chinese translations live in
`translations/clash-qt_zh_CN.ts`. The English UI loads no translation file — `tr()`
returns its argument — so a missing translation shows English rather than a blank string.

`.ts` is the single source of truth for translations; `.qm` is a build artifact
(generated from `.ts` by `lrelease` and embedded as a resource) and is **not committed**.
After changing UI strings, sync the `.ts` files:

```bash
python3 scripts/update-translations.py
```

The script drives CMake's `update_translations` target — the list of source files is
maintained by CMake, so `tr()` calls in added or removed files can't silently fall out of
sync. New or changed entries are marked unfinished and the script prints how many are
left per `.ts`; fill them in and rebuild. In CI, pass `--strict` to fail the build when
anything is untranslated.

The language is decided at startup (`QTranslator` only affects widgets created
afterwards), which is why the settings page asks for a restart.

## Debian packaging

Packaging lives in `debian/` and uses debhelper's cmake sequence, sharing the same
`CMakeLists.txt` as the local build (install paths come from `GNUInstallDirs`).

```bash
# build dependencies
sudo apt install build-essential debhelper cmake ninja-build \
     qt6-base-dev qt6-charts-dev qt6-svg-dev \
     qt6-tools-dev qt6-l10n-tools \
     libssl-dev libyaml-cpp-dev

# build the binary package (output lands in the parent directory)
dpkg-buildpackage -us -uc -b

# install
sudo apt install ../clash-qt_$(sed 's/^v//' VERSION)_amd64.deb
```

Note: debhelper's clean step recreates `build/` (`dh_auto_clean` deletes the
out-of-source build directory outright), so existing local build output is wiped — no
need to clean up before packaging.

Package layout:

| Path | Purpose |
|---|---|
| `/usr/bin/clash-qt` | Main program (GUI + built-in kernel), run as a normal user |
| `/usr/bin/clash-qt-tun` | TUN helper, `4755 root:root` |
| `/usr/share/applications/clash-qt.desktop` | Desktop entry |
| `/usr/share/icons/hicolor/scalable/apps/clash-qt.svg` | Application icon |
| `/usr/share/doc/clash-qt/` | README (English + Chinese), copyright, README.Debian |

The helper and the main program must live in the same directory: `TunController` looks
for `applicationDirPath()/clash-qt-tun`, and splitting them up means TUN can't be
enabled.

**The setuid trade-off**: creating a TUN device needs `CAP_NET_ADMIN`. The project
confines the privileged work to a tiny program that doesn't link Qt (it only creates the
TUN device, configures the address and passes the fd over `SCM_RIGHTS`), so the main
program always runs as a normal user. If you'd rather not keep setuid, run
`sudo chmod 0755 /usr/bin/clash-qt-tun` and the first TUN enable goes through the pkexec
authorization flow instead, with no loss of functionality (an upgrade restores the bit
via `postinst`). Details in `debian/README.Debian`.

Uninstalling leaves `~/.config/clash-qt/` (subscriptions and custom rules) in place;
remove it by hand if you want it gone.

## What's implemented

### Kernel (the `clashqt_core` target)
- Config model with YAML round-tripping (unknown fields preserved verbatim), `deepMerge`
  and the `<key>` / `key!` / `+key` modifier syntax
- Rule engine: DOMAIN / DOMAIN-SUFFIX / DOMAIN-KEYWORD / DOMAIN-REGEX / IP-CIDR(-6) /
  SRC-IP-CIDR / GEOIP / GEOSITE / IP-ASN / DST-PORT / SRC-PORT / IN-PORT / NETWORK /
  PROCESS-* / UID / RULE-SET / AND / OR / NOT / MATCH
- DNS: hosts (including wildcards), nameserver-policy, UDP / TCP / DoH queries, cache and
  TTL, fake-ip pool and reverse lookup, fallback
- Outbound adapters: DIRECT / REJECT / REJECT-DROP / HTTP / SOCKS5 (including auth) and
  proxy groups (Selector / URLTest / Fallback selection and fallback semantics)
- Inbound: HTTP (CONNECT + plaintext forwarding), SOCKS5 (including RFC 1929 auth),
  MIXED (both protocols sniffed on one port)
- Tunnel: rule matching → outbound → bidirectional relay (TCP and UDP), connection
  tracking and traffic accounting
- UDP relay: the TUN data plane keys sessions by 4-tuple, the first packet triggers
  dialing, and traffic is carried either directly (DIRECT) or as VLESS `CMD=0x02`
  (2-byte length-prefixed framing inside the tunnel); dialing runs on its own thread, and
  a failing target gets an ICMP reply within 30 seconds so QUIC falls back to TCP
- Runtime API: switch mode, switch node, latency test, enable/disable rules, list and
  close connections

### App (clash-qt)
- Config layering: `config.yaml` (app; its `kernel:` section is the controlled kernel
  config) / `profile.yaml` (subscription) / `override.yaml` (overrides) — the same
  directory layout as clash-party, so the two can be migrated between each other
- Config generation pipeline: subscription → global overrides → per-subscription
  overrides → rule overrides → controlled config merge → `work/config.yaml`
- Subscription management: remote subscription download and update (parses
  `subscription-userinfo` for traffic and expiry), local import, switching
- UI: Proxies / Profiles / Connections / Rules / Logs / Traffic (Qt Charts curves) /
  Settings
- System integration: Linux system proxy (gsettings and KDE kioslaverc), XDG autostart,
  system tray
- Update check: one click on the settings page queries GitHub Releases, shows the
  changelog and links to the download page (notification only, no auto download/install)
- Logging: separate app and kernel channels on disk, daily rotation, retention-based
  cleanup

## Not implemented yet (in priority order)

| Module | Notes |
|------|------|
| Outbound protocols | VLESS is implemented (plain TCP / TLS / WebSocket / REALITY + XTLS Vision, including X25519MLKEM768 hybrid key exchange) and Hysteria2 is implemented (QUIC + HTTP/3 auth, see below); VMess / Trojan / Shadowsocks / Hysteria v1 / TUIC / WireGuard and the rest are registered as "not supported yet" and **fail loudly instead of silently going direct**, so traffic can't bypass the proxy |
| External controller | REST / WebSocket API (`external-controller`) for third-party panels such as zashboard |
| redir / tproxy | Transparent-proxy inbounds (`SO_ORIGINAL_DST` / `IP_TRANSPARENT`); TUN mode is already implemented |
| JS override engine | Needs an embedded JS engine (QuickJS or Qt Qml's QJSEngine); JS overrides currently warn and are skipped |
| Geo data | Reading geoip.metadb / geosite.dat (GEOIP / GEOSITE rules depend on it) |
| Remote rule sets | Downloading and caching `type: http` rule-providers |
| Misc | Floating window, WebDAV backup, Sub-Store integration, plugins (.cpx), Gist sync; the UI currently ships Chinese and English only |

## Compatibility verified against a real subscription

A subscription with 71 nodes, 9 proxy groups and roughly 2000 rules was used for
end-to-end verification; four classes of real-world compatibility problems surfaced and
were fixed:

| Problem | Symptom | Fix |
|------|------|------|
| Subscription User-Agent | Airports identify clients by UA; a non-clash UA gets a trimmed config plus "your proxy software isn't supported" | Default UA is `clash-qt/v<version> (clash.meta)`, the same strategy as clash-party; overridable in settings |
| Group `type: select` | The parser table only knew `selector`, so only 4 of 9 groups were built | `parseGroupType` now accepts `select` / `urltest` / `loadbalance` / `smart` |
| Built-in policies in a member list | `proxies: [REJECT, DIRECT]` made find() come up empty and the whole group was skipped | DIRECT / REJECT / REJECT-DROP / PASS are registered in the name table |
| Logical rule sub-rules | `AND,((DST-PORT,443),(NETWORK,UDP)),REJECT` — the sub-rules have no target policy and were rejected as having too few fields | Sub-rules get a placeholder target and inherit the outer policy |

Conclusion: the kernel loads every node and group correctly; traffic matching a DIRECT
rule is forwarded normally; VLESS group traffic reaches the internet (verified on real
REALITY + Vision and VLESS + TLS + WebSocket nodes); Hysteria2 nodes work as well (see
below); unimplemented protocols fail loudly rather than silently going direct.

## Hysteria2 support

Hysteria2 (`type: hysteria2` / `hy2`) doesn't use TCP — it runs QUIC over UDP, so it is
an entirely different protocol stack from the VLESS family. The implementation lives in
`src/core/src/common/Hysteria2Client.cpp`:

| Layer | Implementation |
|----|------|
| QUIC | OpenSSL 3.2+'s built-in QUIC client API (`OSSL_QUIC_client_method` + `BIO_new_dgram`), no third-party QUIC library |
| TLS | QUIC carries TLS 1.3 internally; ALPN pinned to `h3`; both `skip-cert-verify` and `fingerprint` (certificate SHA-256 pinning) are supported |
| Auth | A minimal HTTP/3 subset: control-stream SETTINGS + `POST https://hysteria/auth`; QPACK uses the static table only (the dynamic table is declared with capacity 0), and server Huffman strings are decoded per RFC 7541 |
| Tunneling | One bidirectional stream per target connection: `0x401`(varint) \| address length \| `host:port` \| padding length \| padding |
| Port hopping | `ports: 60000-63000` style; a random port is picked for each connection |
| Retry | Network-class failures (handshake/auth timeouts) retry on up to 3 other ports within the overall timeout budget; deterministic failures such as a rejected auth are not retried |

**Explicit boundaries** (no silent degradation):

- **No UDP relay**: OpenSSL 3.2 doesn't implement QUIC DATAGRAM frames, so UDP sessions on
  a Hysteria2 node fail loudly and get an ICMP reply (nudging apps back to TCP) instead of
  degrading to a direct connection. UDP relay itself is implemented in the TUN data plane
  (both DIRECT and VLESS can carry it); it is only unavailable when a Hysteria2 node is
  selected.
- **No Salamander obfuscation (`obfs`) and no realm**: selecting them reports a clear
  reason.
- **No stream multiplexing**: every inbound connection establishes its own QUIC session
  (so a new connection costs one extra handshake round-trip, on the order of tens of
  milliseconds). This keeps each connection's objects on its own worker thread with
  nothing shared across threads. If first-byte latency needs work later, per-node session
  sharing plus dedicated I/O threads can be added without touching the protocol layer.

## Known differences

- **yaml key order**: yaml-cpp doesn't preserve order when re-emitting maps. Key order in
  a YAML map carries no semantic meaning, so functionality is unaffected; reproducing the
  layout byte for byte would require an order-preserving parser.
- **DoT**: `tls://` DNS servers currently fall back to plaintext TCP, with a note in the
  log. The default DNS config therefore uses DoH.
- **Plaintext HTTP forwarding**: everything is treated as `Connection: close`, with no
  keep-alive reuse.
- **Threading model**: each inbound connection gets a thread plus its own event loop
  (capped at 512); the plan is to move to an event-loop thread pool with async dialing.
- **Traffic accounting**: bytes on the system-proxy path are accumulated by a hook in the
  relay; TUN connections are dialed by the IP stack itself and don't go through
  `handleStream`, so `TunIpStack` reports them via `setTrafficSink()` into
  `Tunnel::addTraffic()`. Both paths feed the same pair of atomic counters, and the UI
  samples them once a second to compute speed.
- **Update check**: the data source is the release list on `api.github.com`; on failure
  (unauthenticated API quota is per egress IP, and shared proxy nodes often get a 403) it
  falls back to `releases.atom`. Both paths skip releases whose tag isn't a version (e.g.
  `continuous_build`) and skip prereleases. Requests leave through the local mixed port,
  so it works with or without the system proxy; it only shows the changelog and a link to
  the release page — downloading and installing is left to the user.
- **System proxy on exit**: quitting (closing the window, tray exit, SIGTERM) restores the
  system proxy to direct before stopping the kernel. The test is whether the desktop's
  manual http/https/socks proxies all point at the local inbound port — it only touches
  settings that point at itself, so a corporate proxy you configured is never clobbered.
  A `SIGKILL` or a crash leaves no time for cleanup, and a proxy pointing at an empty port
  stays behind on the desktop.
- **Switch memory**: system-proxy and TUN state are stored separately in `sysProxy.enable`
  and `tun.enable` and restored at startup (once the event loop is running and the window
  is shown): TUN brings the data plane back up, the system proxy points at the local port
  again. A failed restore only shows a message and doesn't change the record. On exit
  **only the desktop proxy settings are reverted, the switch records are left alone** —
  desktop settings are runtime state, the switch records are user intent. Restoring TUN
  when it isn't authorized raises a pkexec password prompt, which is why this doesn't
  happen before the window appears.
- **ICMP (ping)**: with the system proxy, ping doesn't go through the proxy (HTTP/SOCKS
  have no ICMP semantics) — whether it succeeds depends only on direct connectivity from
  the host. In TUN mode, pinging a domain is answered locally by the IP stack on the
  fake-ip (RTT around 0.1 ms, **not an indication of node quality**), while a real IP
  immediately returns host unreachable. Use the latency test on the proxies page to judge
  node latency.

## Continuous integration

The workflows live in `.github/workflows/` and are split by responsibility:

| Workflow | Trigger | What it does |
|---|---|---|
| `build.yaml` | push to main/master, PRs, manual | Builds with `RelWithDebInfo` (no binaries exported) |
| `test.yaml` | push to main/master, PRs, manual | Builds all targets (including the test binaries) and runs `ctest`; on failure it re-runs the failing cases with `--rerun-failed -V` |
| `release.yaml` | push to any branch or tag, PRs, manual | Builds **amd64 / arm64 / loong64 / riscv64** `.deb` packages in a deepin crimson container using QEMU; outside PRs it creates a GitHub Release with the debs and their SHA-256 |

A few conventions:

- All three workflows use `submodules: recursive`. yaml-cpp and the four header-only
  ML-KEM dependencies are submodules; miss one and CMake configuration fails outright.
- `build.yaml` / `test.yaml` run on `ubuntu-26.04` (Qt >= 6.5). The UI uses
  `QStyleHints::colorScheme()`, which won't compile against Qt 6.4 on `ubuntu-24.04`; the
  workflows check the version explicitly and say "Qt >= 6.5 required" instead of dumping a
  pile of `no member named 'colorScheme'` errors.
- `release.yaml` builds in `linuxdeepin/deepin:crimson*` images so the packaging
  environment matches the distro (Qt and debhelper versions line up): amd64 natively, the
  other three architectures under QEMU.
- Build dependencies have exactly one source: `debian/control`. The container installs
  them with `mk-build-deps`, so the package list isn't duplicated in the workflow.
- `release.yaml` publishing behaviour: pushing a tag produces a real Release; pushing a
  regular branch or a manual run overwrites the same-named `continuous_build` prerelease
  (the "every commit has a downloadable package" flow); PRs only run the four-architecture
  build and create no Release.
- The Release body is the matching section of `CHANGELOG.md` (`## [v1.0.0]` up to the next
  `## [`), pasted verbatim; the `v` prefix on the heading is optional, so both `v1.0.0` and
  `1.0.0` tags match. Continuous builds get a fixed note plus a link to `CHANGELOG.md`.
- Before a real release the versions are cross-checked and any mismatch **fails the run**
  (`::error::`) rather than publishing a Release whose version or changelog doesn't line
  up:
  * the tag and `VERSION` must be the same version (`v1.0.1` against `v1.0.0` is rejected);
  * `CHANGELOG.md` must have the matching section (if it's missing, the job tells you to
    add `## [v1.0.1]`).
- Releases are built with `debuild -us -uc -b` and are not GPG-signed (CI has no key).
  `debian/source/format` is `3.0 (native)` and the version comes straight from
  `debian/changelog` (currently `1.0.0`, no Debian revision); `-b` only skips the source
  package and emits the binary one. Before releasing, make the changelog upstream version
  match `VERSION` (`dch -v $(sed 's/^v//' VERSION)`).
