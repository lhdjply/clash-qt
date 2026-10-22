#!/usr/bin/env bash
# ---------------------------------------------------------------------------
#  构建 Linux 便携包（tar.gz）
#
#  与 deb 的分工：deb 只装二进制，Qt / OpenSSL 由 dpkg 提供，体积小但要 root，
#  且目标发行版得有匹配版本的 Qt；tar.gz 要解压即用，所以把 Qt 与非系统库
#  一起打进 lib/，再配一个设置 LD_LIBRARY_PATH 的入口脚本。
#
#  用法：
#    scripts/package-linux-tarball.sh --arch amd64 --from-deb clash-qt_1.0.2_amd64.deb
#    scripts/package-linux-tarball.sh --arch amd64 --bin-dir build/bin --strip
#
#  --from-deb 供 CI 用（直接取 deb 里那份二进制，内容与安装包一致）；
#  --bin-dir 供本地从构建目录出包。
#  依赖：dpkg-deb（仅 --from-deb）、patchelf、ldd、tar
#
#  包内布局：
#    clash-qt             入口脚本（设 LD_LIBRARY_PATH 后转交 bin/）
#    bin/clash-qt         主程序（RUNPATH=$ORIGIN/../lib）
#    bin/clash-qt-tun     TUN 辅助程序，须与主程序同目录（TunController 按
#                         applicationDirPath() 查找）；不含 setuid，首次开启
#                         TUN 时走 pkexec 授权
#    bin/qt.conf          指向包内的 plugins/ 与 translations/
#    lib/ plugins/ translations/ share/
#    LICENSE、README*.md
# ---------------------------------------------------------------------------

set -euo pipefail

readonly REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

log()  { printf '  %s\n' "$*"; }
step() { printf '\n==> %s\n' "$*"; }
die()  { printf '\n[错误] %s\n' "$*" >&2; exit 1; }

# 软件名从 CMake 的 project() 取，与 CLASHQT_APP_NAME 同源，改名不必回来同步
APP_NAME="$(sed -n 's/^project([[:space:]]*\([^ )]*\).*/\1/p' "$REPO_ROOT/CMakeLists.txt" | head -1)"
[ -n "$APP_NAME" ] || die "无法从 CMakeLists.txt 的 project() 读出软件名"
readonly APP_NAME
readonly TUN_HELPER="${APP_NAME}-tun"
# 显示名同理：CMakeLists 里 set(CLASHQT_APP_DISPLAY_NAME "…")，桌面入口用得上
APP_DISPLAY_NAME="$(sed -n 's/.*set(CLASHQT_APP_DISPLAY_NAME[[:space:]]*"\([^"]*\)".*/\1/p' \
                        "$REPO_ROOT/CMakeLists.txt" | head -1)"
[ -n "$APP_DISPLAY_NAME" ] || die "无法从 CMakeLists.txt 读出显示名 CLASHQT_APP_DISPLAY_NAME"
readonly APP_DISPLAY_NAME

ARCH=""
FROM_DEB=""
BIN_DIR=""
DO_STRIP=0
OUT_DIR="$REPO_ROOT/dist"

while [ $# -gt 0 ]; do
    case "$1" in
        --arch)      ARCH="${2:?--arch 需要参数}";      shift 2 ;;
        --from-deb)  FROM_DEB="${2:?--from-deb 需要参数}"; shift 2 ;;
        --bin-dir)   BIN_DIR="${2:?--bin-dir 需要参数}"; shift 2 ;;
        --out)       OUT_DIR="${2:?--out 需要参数}";    shift 2 ;;
        --strip)     DO_STRIP=1; shift ;;
        -h|--help)   awk 'NR > 1 && /^#/ { sub(/^# ?/, ""); if ($0 ~ /^-+$/) next; print; next }
                          NR > 1 { exit }' "${BASH_SOURCE[0]}"; exit 0 ;;
        *)           die "未知参数：$1" ;;
    esac
done

[ -n "$ARCH" ] || die "必须给 --arch（写进包名，如 amd64 / arm64 / loong64 / riscv64）"
if [ -n "$FROM_DEB" ] && [ -n "$BIN_DIR" ]; then
    die "--from-deb 与 --bin-dir 只能二选一"
fi
for tool in patchelf tar ldd; do
    command -v "$tool" > /dev/null 2>&1 || die "缺少工具：$tool"
done

readonly VERSION="$(sed 's/^[vV]//' "$REPO_ROOT/VERSION" | tr -d '[:space:]')"
[ -n "$VERSION" ] || die "VERSION 为空"
readonly STAGE="$OUT_DIR/${APP_NAME}-${VERSION}-linux-${ARCH}"
readonly TARBALL="$OUT_DIR/${APP_NAME}-${VERSION}-linux-${ARCH}.tar.gz"

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

# ---------------------------------------------------------------------------
#  1. 取二进制
# ---------------------------------------------------------------------------
step "准备二进制（${APP_NAME} ${VERSION} / linux-${ARCH}）"

if [ -n "$FROM_DEB" ]; then
    [ -f "$FROM_DEB" ] || die "找不到 deb：$FROM_DEB"
    command -v dpkg-deb > /dev/null 2>&1 || die "缺少工具：dpkg-deb"
    dpkg-deb -x "$FROM_DEB" "$TMP/deb"
    SRC_DIR="$TMP/deb/usr/bin"
    log "来源：$(basename "$FROM_DEB")"
else
    SRC_DIR="${BIN_DIR:-$REPO_ROOT/build/bin}"
    log "来源：$SRC_DIR"
fi

mkdir -p "$OUT_DIR"
rm -rf "$STAGE"   # 上一次的残留会被误当成本次产物，先清掉
mkdir -p "$STAGE/bin" "$STAGE/lib" "$STAGE/plugins" "$STAGE/translations" \
         "$STAGE/share/applications" "$STAGE/share/icons/hicolor/scalable/apps"

for name in "$APP_NAME" "$TUN_HELPER"; do
    [ -f "$SRC_DIR/$name" ] || die "缺少 $SRC_DIR/$name（先构建，或用 --from-deb）"
    cp "$SRC_DIR/$name" "$STAGE/bin/$name"
    # 去掉 setuid：tar 解压不保留属主，留着这个位只会是误导；
    # 包里的 TUN 一律走应用内的 pkexec 授权流程
    chmod 0755 "$STAGE/bin/$name"
    if [ "$DO_STRIP" -eq 1 ]; then
        strip --strip-unneeded "$STAGE/bin/$name"
    fi
    log "bin/$name"
done

# ---------------------------------------------------------------------------
#  2. 收集共享库依赖
#
#  规则：glibc 与图形驱动用宿主的，其余全部随包带一份。
#
#  glibc：加载器与 libc 成对编译，混用不受支持。
#  GL / EGL / drm / vulkan：必须与显卡驱动配套，带一份几乎必然黑屏。
#
#  反过来 X11 / xcb / ICU / fontconfig 必须带：xcb 平台插件依赖 libxcb-cursor，
#  老发行版没有；ICU 的 soname 与 Qt 绑定（libicui18n.so.74），对不上就加载
#  失败。同一 soname 在进程里只有一份，宿主的 GL 库也用打包的 libX11，
#  不存在两份混跑。
# ---------------------------------------------------------------------------
excluded() {
    case "$1" in
        # glibc 家族
        ld-linux*|libc.so.*|libm.so.*|libmvec.so.*|libdl.so.*|libpthread.so.*) return 0 ;;
        librt.so.*|libresolv.so.*|libutil.so.*|libnsl.so.*|libanl.so.*)         return 0 ;;
        # 显卡驱动相关
        libGL.so.*|libGLX*.so.*|libOpenGL.so.*|libGLdispatch.so.*|libEGL.so.*) return 0 ;;
        libdrm*.so.*|libvulkan.so.*|libnvidia-*.so.*|libcuda.so.*)             return 0 ;;
    esac
    return 1
}

# ldd 一个 ELF，输出「soname<TAB>绝对路径」；not found 打成 MISSING 交给调用方
list_deps() {
    ldd "$1" 2>/dev/null | awk '
        /=>/ {
            if ($3 == "not found") { print "MISSING\t" $1; next }
            print $1 "\t" $3
            next
        }
        NF == 1 && $1 ~ /\.so/ { print $1 "\t" $1 }
    '
}

declare -A BUNDLED=()

# 按 soname 落盘：ldd 给的路径可能带完整版本号，但运行时按 DT_NEEDED 里的
# soname 查找，文件名必须与之一致
bundle_file() {
    local name="$1" path="$2"
    [ -n "${BUNDLED[$name]:-}" ] && return 0
    BUNDLED[$name]=1

    if excluded "$name"; then return 0; fi
    if [ ! -f "$path" ]; then
        die "依赖 $name 解析到不存在的路径：$path"
    fi
    cp -L "$path" "$STAGE/lib/$name"
}

collect_from() {
    local file="$1" name path
    while IFS=$'\t' read -r name path; do
        [ -n "$name" ] || continue
        if [ "$name" = "MISSING" ]; then
            die "$file 依赖的 $path 在构建环境里找不到；先补齐构建依赖"
        fi
        bundle_file "$name" "$path"
    done < <(list_deps "$file")
}

step "收集共享库依赖"
collect_from "$STAGE/bin/$APP_NAME"
collect_from "$STAGE/bin/$TUN_HELPER"

# ---------------------------------------------------------------------------
#  3. Qt 插件
#
#  widgets 应用没有平台插件就起不来；HTTPS 订阅下载还依赖 tls 插件，图标
#  与图片解码依赖 imageformats / iconengines。wayland-* 只在装了
#  qt6-wayland 的构建环境里存在，有就带、没有就跳过（宿主走 xcb）。
# ---------------------------------------------------------------------------
step "收集 Qt 插件"

QT_PLUGIN_DIR=""
for probe in qtpaths6 qmake6; do
    if command -v "$probe" > /dev/null 2>&1; then
        if [ "$probe" = qtpaths6 ]; then
            QT_PLUGIN_DIR="$("$probe" --plugin-dir 2>/dev/null || true)"
        else
            QT_PLUGIN_DIR="$("$probe" -query QT_INSTALL_PLUGINS 2>/dev/null || true)"
        fi
        [ -n "$QT_PLUGIN_DIR" ] && [ -d "$QT_PLUGIN_DIR" ] && break
        QT_PLUGIN_DIR=""
    fi
done
if [ -z "$QT_PLUGIN_DIR" ]; then
    for guess in /usr/lib/*/qt6/plugins /usr/lib/qt6/plugins /usr/lib64/qt6/plugins; do
        [ -d "$guess" ] && { QT_PLUGIN_DIR="$guess"; break; }
    done
fi
[ -n "$QT_PLUGIN_DIR" ] || die "找不到 Qt 插件目录（qtpaths6 / qmake6 都没有，也没有 /usr/lib/*/qt6/plugins）"
log "插件目录：$QT_PLUGIN_DIR"

# 按名字挑而不是整目录照搬：构建机的插件目录常混着发行版私有实现，它们
# 挂在桌面框架与 libjxl 上，会把 gtk3、libaom、libOpenEXR 一整套拖进来
# （实测 40MB → 170MB）。界面全部自绘，用不上这些：
#   · platforms     只要 xcb / offscreen / wayland
#   · imageformats、iconengines 只要 Qt 自带的 libq*
#   · platformthemes 只影响原生控件配色且依赖 gtk3，不带
# 要加回某个插件，往下面的列表里补一行名字即可。
copy_plugins() {
    local dir="$1"; shift
    [ -d "$QT_PLUGIN_DIR/$dir" ] || { log "跳过 $dir（构建环境里没有）"; return 0; }
    mkdir -p "$STAGE/plugins/$dir"

    local count=0 pattern file base
    for pattern in "$@"; do
        for file in "$QT_PLUGIN_DIR/$dir"/$pattern; do
            [ -f "$file" ] || continue   # 通配没命中时 $file 就是字面量本身
            base="$(basename "$file")"
            case "$base" in
                # 发行版私有实现，见本节开头
                libdicon.so|libdsvgicon.so|libchameleon.so|libqdeepin.so|libqgtk3.so) continue ;;
                libdxcb.so|libplugin-shell.so|kimg_*.so|libdci.so|libdsvg.so|libxraw.so*) continue ;;
            esac
            cp "$file" "$STAGE/plugins/$dir/"
            count=$((count + 1))
        done
    done
    [ "$count" -eq 0 ] && rmdir "$STAGE/plugins/$dir"
    log "plugins/$dir（$count 个）"
}

copy_plugins platforms libqxcb.so libqoffscreen.so libqminimal.so 'libqwayland-*.so'
copy_plugins platforminputcontexts '*.so'   # 输入法（fcitx / ibus）：中文输入要用
copy_plugins iconengines 'libq*.so'         # qsvgicon：界面图标是 SVG
copy_plugins imageformats 'libq*.so'
copy_plugins tls '*.so'                     # OpenSSL 后端：订阅走 HTTPS
copy_plugins sqldrivers '*.so'
copy_plugins networkinformation '*.so'
copy_plugins xcbglintegrations '*.so'
copy_plugins wayland-decoration-client '*.so'
copy_plugins wayland-graphics-integration-client '*.so'
copy_plugins wayland-shell-integration '*.so'
# styles 不带：Fusion 编在 QtWidgets 里，该目录通常只剩发行版私有样式

# 平台插件是硬要求：没有它连 --version 都跑不起来
[ -f "$STAGE/plugins/platforms/libqxcb.so" ] || \
    die "没有拿到 xcb 平台插件（libqxcb.so），检查构建环境是否装了 qt6-qpa-plugins"

step "收集插件依赖"
while IFS= read -r plugin; do
    collect_from "$plugin"
done < <(find "$STAGE/plugins" -type f -name '*.so')

# ---------------------------------------------------------------------------
#  4. Qt 自带翻译
#
#  程序的文案编在自己的资源里，标准对话框（按钮、右键菜单）由
#  qtbase_zh_CN 翻译，下面把 .qm 放进 translations/ 并交给 qt.conf 指路。
# ---------------------------------------------------------------------------
step "收集 Qt 中文翻译"

QT_TRANSLATIONS_DIR=""
if command -v qtpaths6 > /dev/null 2>&1; then
    QT_TRANSLATIONS_DIR="$(qtpaths6 --translations-dir 2>/dev/null || true)"
fi
for guess in "$QT_TRANSLATIONS_DIR" /usr/share/qt6/translations /usr/share/qt/translations; do
    [ -n "$guess" ] && [ -f "$guess/qtbase_zh_CN.qm" ] && { QT_TRANSLATIONS_DIR="$guess"; break; }
    QT_TRANSLATIONS_DIR=""
done
if [ -n "$QT_TRANSLATIONS_DIR" ]; then
    cp "$QT_TRANSLATIONS_DIR"/qtbase_zh_CN.qm "$STAGE/translations/" 2>/dev/null || true
    log "translations/qtbase_zh_CN.qm"
else
    log "警告：没找到 qtbase_zh_CN.qm，标准对话框会显示英文"
fi

# ---------------------------------------------------------------------------
#  5. RUNPATH 与包内启动脚本
#
#  patchelf 只改可执行文件与 lib/ 里的库：
#    bin/ → $ORIGIN/../lib，lib/ → $ORIGIN
#
#  **插件一律不碰**：Qt 6 的插件元数据在 ELF 的 .note.qt.metadata 段里，
#  patchelf 改动态段会连带破坏它，Qt 随后把插件当成「不是 Qt 插件」，报的
#  却是 plugin "xcb" could not be found。
#
#  但插件绕不开 RPATH：Qt 用 dlopen 拉插件，插件自身的依赖不看可执行文件的
#  RPATH，实测会加载宿主的 libxcb-cursor（老系统上没有）。所以在包根放一个
#  入口脚本，用 LD_LIBRARY_PATH 把 lib/ 顶到最前 —— 它对 dlopen 同样有效。
# ---------------------------------------------------------------------------
step "写 RUNPATH 与启动脚本"

for name in "$APP_NAME" "$TUN_HELPER"; do
    patchelf --set-rpath '$ORIGIN/../lib' "$STAGE/bin/$name"
done
find "$STAGE/lib" -type f -name '*.so*' -print0 | while IFS= read -r -d '' lib; do
    patchelf --set-rpath '$ORIGIN' "$lib"
done
log "bin/ → \$ORIGIN/../lib，lib/ → \$ORIGIN（插件保持原样）"

cat > "$STAGE/$APP_NAME" <<LAUNCHER
#!/bin/sh
# 入口脚本：把 lib/ 放进动态库搜索路径最前。Qt 插件是 dlopen 起的，
# 插件自身的依赖不走可执行文件的 RPATH，只有 LD_LIBRARY_PATH 管得到。
here=\$(CDPATH= cd -- "\$(dirname -- "\$0")" && pwd -P)
LD_LIBRARY_PATH="\$here/lib\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH
exec "\$here/bin/$APP_NAME" "\$@"
LAUNCHER
chmod 0755 "$STAGE/$APP_NAME"
log "$APP_NAME（启动脚本）"

# qt.conf 放在二进制旁边：Qt 以它所在目录为基准解析下面的相对路径，
# 于是 Prefix 指向包根、插件与翻译各就各位，不依赖 QT_PLUGIN_PATH 这类环境变量
cat > "$STAGE/bin/qt.conf" <<'QTCONF'
[Paths]
Prefix = ..
Plugins = plugins
Translations = translations
QTCONF
log "bin/qt.conf"

# ---------------------------------------------------------------------------
#  6. 桌面入口、图标、许可证
# ---------------------------------------------------------------------------
step "补桌面入口与许可证"

# .desktop 的 Exec 只有软件名：解压位置不固定，用户要自己改一行再放进
# ~/.local/share/applications（直接运行时用包根的启动脚本）
cp "$REPO_ROOT/resources/linux/clash-qt.desktop.in" "$STAGE/share/applications/${APP_NAME}.desktop"
# 模板里的 @...@ 占位符由 CMake 替换，这里手工等价处理一遍
sed -i "s|@CLASHQT_APP_NAME@|${APP_NAME}|g; s|@CLASHQT_APP_DISPLAY_NAME@|${APP_DISPLAY_NAME}|g" \
    "$STAGE/share/applications/${APP_NAME}.desktop"
cp "$REPO_ROOT/resources/icons/app.svg" "$STAGE/share/icons/hicolor/scalable/apps/${APP_NAME}.svg"

cp "$REPO_ROOT/LICENSE" "$STAGE/LICENSE"
for doc in README.md README-zh.md; do
    [ -f "$REPO_ROOT/$doc" ] && cp "$REPO_ROOT/$doc" "$STAGE/$doc"
done

# ---------------------------------------------------------------------------
#  7. 自检
#
#  四条判据，任一不过即退出：
#    1) 可执行文件的依赖闭包全在 $STAGE/lib 下（系统库除外），没偷用宿主 Qt；
#    2) 每个插件的依赖也都在 lib/ 里，缺文件就是漏拷；
#    3) 用入口脚本跑一次 offscreen --version，验证插件与 qt.conf 链路；
#    4) 无 X 时跑一次 xcb 并读 ld.so 调试输出，确认包内也带的库确实从包内
#       加载，没被宿主同名库顶掉。
# ---------------------------------------------------------------------------
step "自检"

LEAKED=0
STAGE_REAL="$(readlink -f "$STAGE")"
while IFS=$'\t' read -r name path; do
    [ -n "$name" ] || continue
    # ldd 会保留 RPATH 的原始写法（.../bin/../lib/libfoo.so），先归一化再比较
    resolved="$(readlink -f "$path" 2>/dev/null || printf '%s' "$path")"
    case "$resolved" in
        "$STAGE_REAL/lib/"*) continue ;;
        *) excluded "$name" && continue
           printf '    %s -> %s\n' "$name" "$path"
           LEAKED=$((LEAKED + 1)) ;;
    esac
done < <(list_deps "$STAGE/bin/$APP_NAME")
if [ "$LEAKED" -ne 0 ]; then
    die "有 $LEAKED 个依赖仍在用宿主库（上面列出的），包的 RUNPATH 没生效"
fi
log "可执行文件的依赖闭包全部落在包内（系统库除外）"

MISSING_DEPS=0
while IFS= read -r plugin; do
    while IFS=$'\t' read -r name path; do
        [ -n "$name" ] || continue
        if [ "$name" = "MISSING" ]; then
            printf '    %s 缺 %s\n' "${plugin#$STAGE/}" "$path"
            MISSING_DEPS=$((MISSING_DEPS + 1))
            continue
        fi
        excluded "$name" && continue
        [ -f "$STAGE/lib/$name" ] || {
            printf '    %s 需要 %s，但 lib/ 里没有\n' "${plugin#$STAGE/}" "$name"
            MISSING_DEPS=$((MISSING_DEPS + 1))
        }
    done < <(list_deps "$plugin")
done < <(find "$STAGE/plugins" -type f -name '*.so')
if [ "$MISSING_DEPS" -ne 0 ]; then
    die "插件依赖有 $MISSING_DEPS 处没落到 lib/ 里"
fi
log "插件的依赖也都在 lib/ 里"

export HOME="$TMP/home"
mkdir -p "$HOME"
if ! env -i HOME="$HOME" PATH=/usr/bin:/bin QT_QPA_PLATFORM=offscreen \
        "$STAGE/$APP_NAME" --version > "$TMP/version.txt" 2>&1; then
    printf '\n--- 程序输出 ---\n' >&2
    cat "$TMP/version.txt" >&2
    die "解压后无法启动（QT_QPA_PLATFORM=offscreen --version 失败）"
fi
log "启动自检通过：$(grep -m1 "v[0-9]" "$TMP/version.txt" || head -1 "$TMP/version.txt")"

# 没有 DISPLAY 时 xcb 插件注定起不来，它的报错无所谓；要看的是 ld.so 到底
# 从哪儿加载了那些包内也有的库。外层再吞一次 stderr：进程被 abort 时由 shell
# 打印的那行 "已放弃" 不是错误，留在 CI 日志里只会误导人
{ env -i HOME="$HOME" PATH=/usr/bin:/bin QT_QPA_PLATFORM=xcb LD_DEBUG=libs \
    "$STAGE/$APP_NAME" --version > "$TMP/xcb.txt" 2>&1; } 2>/dev/null || true

SHADOWED=0
while IFS= read -r line; do
    lib_path="${line#*calling init: }"
    [ "$lib_path" = "$line" ] && continue
    base="$(basename "$lib_path")"
    [ -f "$STAGE/lib/$base" ] || continue          # 只关心「包里也带了」的库
    case "$(readlink -f "$lib_path")" in
        "$STAGE_REAL/lib/$base") continue ;;
        *) printf '    %s 实际加载自 %s\n' "$base" "$lib_path"
           SHADOWED=$((SHADOWED + 1)) ;;
    esac
done < "$TMP/xcb.txt"
if [ "$SHADOWED" -ne 0 ]; then
    die "有 $SHADOWED 个库被宿主的同名库顶掉了（上面列出的），插件的 RUNPATH 没生效"
fi
log "xcb 插件的依赖同样走包内（无显示环境下的加载验证）"

# ---------------------------------------------------------------------------
#  8. 打包
#
#  --owner/--group/--numeric-owner 把属主固定成 0：不同机器上构建出来的包
#  内容一致，比较时只差时间戳
# ---------------------------------------------------------------------------
step "打包"

mkdir -p "$OUT_DIR"
rm -f "$TARBALL"
tar -czf "$TARBALL" \
    --owner=0 --group=0 --numeric-owner \
    -C "$OUT_DIR" "$(basename "$STAGE")"

log "已生成 $TARBALL（$(du -h "$TARBALL" | cut -f1)）"
log "解压后运行：$(basename "$STAGE")/$APP_NAME"
