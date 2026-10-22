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
