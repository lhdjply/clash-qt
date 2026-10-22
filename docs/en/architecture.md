# Architecture

## Repository layout

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
│       ├── include/     one directory per module: config / i18n / sys / ui /
│       │                updater / utils (plus AppContext.h at the root)
│       └── src/
├── tests/           unit tests (Qt Test + CTest, off by default)
├── scripts/         maintainer scripts (translation sync, UI snapshots)
├── docs/            this documentation
└── third_party/     git submodules: yaml-cpp, ml-kem, sha3, randomshake, subtle
```

The GUI never spawns a core process. `AppContext` owns the `Hub`; every switch in the
sidebar ends up as a call on that object, and the kernel's own threads serve the
inbound connections inside the same process — which is why quitting has to stop the
listeners explicitly (see `AppContext::shutdown`).

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
  config) / `profile.yaml` (subscription) / `override.yaml` (overrides) — one file per
  layer, so app settings, subscription metadata and rule overrides never mix
- Config generation pipeline: subscription → global overrides → per-subscription
  overrides → rule overrides → controlled config merge → `work/config.yaml`
- Subscription management: remote subscription download and update (parses
  `subscription-userinfo` for traffic and expiry), local import, switching
- UI: sidebar built from tiles (outbound mode, system proxy / TUN switches, the active
  profile with its traffic and expiry, a six-tile navigation grid, live speed) plus the
  Proxies / Profiles / Connections / Rules / Overrides / Logs / Traffic (Qt Charts) /
  Settings pages
- System integration: Linux system proxy (gsettings and KDE kioslaverc), XDG autostart,
  system tray
- Update check: one check on startup (can be turned off in settings) plus a manual
  button; shows the changelog and links to the download page (notification only,
  no auto download/install)
- Logging: separate app and kernel channels on disk, daily rotation, retention-based
  cleanup

## Not implemented yet (in priority order)

| Module | Notes |
|------|------|
| Outbound protocols | VLESS is implemented (plain TCP / TLS / WebSocket / REALITY + XTLS Vision, including X25519MLKEM768 hybrid key exchange) and Hysteria2 is implemented (QUIC + HTTP/3 auth, see [protocols.md](protocols.md)); VMess / Trojan / Shadowsocks / Hysteria v1 / TUIC / WireGuard and the rest are registered as "not supported yet" and **fail loudly instead of silently going direct**, so traffic can't bypass the proxy |
| External controller | REST / WebSocket API (`external-controller`) for third-party panels such as zashboard |
| redir / tproxy | Transparent-proxy inbounds (`SO_ORIGINAL_DST` / `IP_TRANSPARENT`); TUN mode is already implemented |
| JS override engine | Needs an embedded JS engine (QuickJS or Qt Qml's QJSEngine); JS overrides currently warn and are skipped |
| Geo data | Reading geoip.metadb / geosite.dat (GEOIP / GEOSITE rules depend on it) |
| Remote rule sets | Downloading and caching `type: http` rule-providers |
| Misc | Floating window, WebDAV backup, Sub-Store integration, plugins (.cpx), Gist sync; the UI currently ships Chinese and English only |
