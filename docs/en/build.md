# Building

## Requirements

- Qt 6.2+ with Core / Network / Widgets / Sql / Svg / Charts
- CMake 3.21+
- A C++20 compiler (GCC 11+ / Clang 13+)
- OpenSSL 3.2+ if you want Hysteria2 (it uses the built-in QUIC client API)

```bash
# first clone (needs the yaml-cpp / ml-kem submodules)
git clone --recursive <repo>
# in an existing clone
git submodule update --init --recursive

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/bin/clash-qt          # the only artifact: the GUI (kernel included)
```

`build/bin/clash-qt` is the whole product: the kernel is an OBJECT library
(`clashqt_core`) whose object files are linked straight into the executable and
called in-process, so there is no separate core binary or `.a` to ship.

## Windows

Windows has no rpath and no pkg-config, so Qt and OpenSSL are found only through
explicit prefixes at configure time:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_PREFIX_PATH=D:/Qt -DOPENSSL_ROOT_DIR=D:/OpenSSL
cmake --build build -j
```

Qt must be the MinGW build that matches the compiler — an MSVC-built Qt cannot
link against MinGW objects. To run `build/bin/clash-qt.exe`, put `<Qt>/bin` on
`PATH`: the Qt DLLs and the `platforms/qwindows.dll` plugin live there.

What Windows supports today:

| Feature | Implementation |
|---|---|
| Kernel (rules / DNS / fake-ip / VLESS / Hysteria2 / HTTP+SOCKS inbounds) | identical to Linux; the only portability change is `WSADuplicateSocket` instead of `dup()` when handing a connection to a worker thread |
| System proxy | `HKCU\...\Internet Settings` (ProxyEnable / ProxyServer / ProxyOverride) + `InternetSetOption(INTERNET_OPTION_REFRESH)` so running apps pick it up immediately |
| Autostart | `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` |
| Crash report | `crash-<tid>.txt` (symbolized stack via dbghelp) plus `crash-<tid>.dmp` (minidump) |
| Memory usage in the UI | `GetProcessMemoryInfo` working set |
| TUN mode | Wintun adapter (loads `wintun.dll` at runtime) with addresses and routes configured through IP Helper; only the fake-ip range is routed into TUN, the system default route is left alone |

TUN mode on Windows needs two things: `wintun.dll` next to `clash-qt.exe` (the switch
reports an explicit error when it is missing) and an elevated process (UAC cannot be
satisfied from inside a running one). The data plane runs on a Wintun session, with a
dedicated reader thread handing packets to the user-space stack in batches; DNS points
at the TUN address and is answered on port 53 there, the same arrangement as on Linux.

Two more Windows-specific limits worth knowing: `ProxyOverride` has no CIDR syntax, so
only the 8-bit and 16-bit entries of the bypass list become wildcards (`10.*`,
`192.168.*`); and DNS does not use an NRPT, so on multi-homed machines nothing forces
every query through the tunnel (harmless while only the fake-ip range is routed).

## Dependencies

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

## Renaming and versioning

**To rename the app, edit `CMakeLists.txt` only**: the executable name, CMake target
name, data directory (`$XDG_CONFIG_HOME/<name>`), single-instance ID, log file prefix,
TUN helper (`<name>-tun`), TUN interface name, desktop entry and icon name are all
derived from `project()` (`CLASHQT_TARGET` / `CLASHQT_APP_NAME` / `CLASHQT_TUN_HELPER`
and friends); there is no `"clash-qt"` literal in the sources. The display name
(`Clash Qt`) lives separately in `CLASHQT_APP_DISPLAY_NAME`, and the desktop entry is
generated at configure time from the `resources/linux/clash-qt.desktop.in` template. The
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
