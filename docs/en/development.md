# Development

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

## UI snapshots

`docs/images/main-*.png` are rendered from the real UI, not taken with a screenshot tool:

```bash
python3 scripts/ui-snapshot.py              # 中文图 → docs/images
python3 scripts/ui-snapshot.py --lang en    # English images
```

The script links `scripts/ui_snapshot.cpp` against the objects the app was already built
from (it swaps `main.cpp` for the tool and relinks), then runs it with
`QT_QPA_PLATFORM=offscreen`, so no display server and no window manager are involved and
the result is deterministic. The tool clicks the real sidebar tiles, which means the
images show the same selection state, badges and layout a user sees.

It works on a **copy** of your config (subscription names and URLs are replaced with
sample values first), so images can be committed without leaking a real airport or token.
Prerequisites: the app has been built once, and no `clash-qt` instance is running — the
kernel needs ports 7890-7892, and with those taken the images would show a stopped kernel.

By default only the profiles page is rendered, because that is the image the README uses.
Add `--pages all` to also render proxies / rules / overrides / settings — those contain
your own nodes and rules, so keep them out of a commit.

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
