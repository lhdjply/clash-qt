# Debian packaging

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

## Portable tarball

Besides the deb, each release also ships `clash-qt-<version>-linux-<arch>.tar.gz`:
extract and run, no root required, and the target machine does not need Qt installed.
The script is `scripts/package-linux-tarball.sh`:

```bash
# from an already built deb (what CI does: same binary as the package)
scripts/package-linux-tarball.sh --arch amd64 --from-deb clash-qt_1.0.0_amd64.deb

# or straight from the build tree (local use; --strip drops debug info)
scripts/package-linux-tarball.sh --arch amd64 --bin-dir build/bin --strip
```

It needs `patchelf` and writes to `dist/`. CI builds one per architecture inside the
matching container.

Layout:

| Path | Purpose |
|---|---|
| `clash-qt` | Entry script: puts `lib/` first on the library search path, then hands over to `bin/clash-qt` |
| `bin/clash-qt` | Main program, RUNPATH `$ORIGIN/../lib` |
| `bin/clash-qt-tun` | TUN helper; must sit next to the main program, no setuid bit in the tarball |
| `bin/qt.conf` | Points Qt at `../plugins` and `../translations` |
| `lib/` | Bundled shared libraries (Qt, OpenSSL, ICU, xcb, …) |
| `plugins/` | Platform plugins (xcb / wayland / offscreen), icon engines, image formats, TLS, input methods |
| `translations/` | Qt's own Chinese catalogue (standard dialogs, context menus) |
| `share/` | `.desktop` entry and icon, for you to install into `~/.local/share` |
| `LICENSE`, `README*.md` | Licence and readme |

```bash
tar xzf clash-qt-1.0.0-linux-amd64.tar.gz
./clash-qt-1.0.0-linux-amd64/clash-qt
```

**Why an entry script**: Qt loads plugins with `dlopen`, and a plugin's own dependencies
do not go through the executable's RUNPATH. Adding a RUNPATH to the plugin instead breaks
the `.note.qt.metadata` section Qt reads plugin metadata from — the plugin is then dropped
as "not a Qt plugin" while the error still reads
`Could not find the Qt platform plugin "xcb"`. `LD_LIBRARY_PATH` is the only way to cover
plugins without editing them, which is all the entry script does.

**What is left to the host**: glibc and the graphics stack (libGL / EGL / GLX / drm /
vulkan). The former is compiled as a pair with the dynamic loader; the latter has to match
the installed GPU driver. Everything else (including libX11 / xcb, ICU, freetype) is
bundled, because Qt 6.5+ needs `libxcb-cursor`, which older distributions do not ship.

**TUN mode**: the tarball carries no setuid bit. The first time TUN is enabled the app asks
`pkexec` to give `bin/clash-qt-tun` `root:root` and `sx` (needs polkit and a password
prompt). Skipping that only disables the TUN switch; everything else works.

**Compatibility**: binaries are built inside the deepin crimson container, so the target
needs a glibc at least as new as that release (deepin 25 / Debian trixie generation). On
older systems, build from source or use the deb.

**Desktop integration**: `Exec` in `share/applications/clash-qt.desktop` is just the
program name, and the extraction path varies, so edit one line before installing it into
`~/.local/share/applications`:

```bash
sed "s|^Exec=.*|Exec=$PWD/clash-qt-1.0.0-linux-amd64/clash-qt|" \
    clash-qt-1.0.0-linux-amd64/share/applications/clash-qt.desktop \
    > ~/.local/share/applications/clash-qt.desktop
```

To keep the configuration next to the program (USB stick, several versions side by side),
create an empty file named `PORTABLE` in `bin/`: the app then uses `bin/data/` instead of
`~/.config/clash-qt/`.

