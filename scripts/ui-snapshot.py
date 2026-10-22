#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把 clash-qt 的各页面渲染成 PNG，供 README / 文档配图。

    python3 scripts/ui-snapshot.py              # 中文图，写到 docs/images
    python3 scripts/ui-snapshot.py --lang en    # 英文图
    python3 scripts/ui-snapshot.py --out /tmp/shots --lang en

产物：main-<lang>.png（订阅页）+ proxies / rules / overrides / settings-<lang>.png。

几个刻意的取舍：

  · 不在应用里加 --snapshot 开关。出图只有维护文档时才用，没必要让正式二进制
    多一个只有维护者会用到的参数，所以渲染代码放在 scripts/ui_snapshot.cpp。
  · 不用桌面截图。依赖窗口管理器的截图不稳定（窗口可能被遮挡或被重排），
    没有显示服务的环境里也跑不了；这里用 QT_QPA_PLATFORM=offscreen，
    窗口只在进程内 render 到 QPixmap。
  · 不为它重编一遍应用。脚本从 ninja 的命令表里取用 clash-qt 的编译与链接命令，
    把 main.cpp 换成 ui_snapshot.cpp，其余对象文件直接复用（因此只需一次链接）。

前提：应用已构建（脚本会先 `ninja bin/clash-qt` 补齐），且当前没有 clash-qt 在运行
——内核要占用 7890 等端口，端口被占时图里的内核状态会是「未启动」、徽标也会隐藏。
"""

import argparse
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
TOOL_SRC = ROOT / "scripts" / "ui_snapshot.cpp"
TOOL_OBJ = BUILD / "ui-snapshot" / "ui_snapshot.cpp.o"
TOOL_BIN = BUILD / "bin" / "ui-snapshot"
APP_OBJ = "CMakeFiles/clash-qt.dir/src/app/src/main.cpp.o"

PAGES = ["profiles", "proxies", "rules", "overrides", "settings"]


def die(message):
    print(f"error: {message}", file=sys.stderr)
    sys.exit(1)


def ninja(*args):
    return subprocess.run(["ninja", "-C", str(BUILD), *args],
                          check=True, capture_output=True, text=True).stdout


def build_tool():
    if not BUILD.is_dir():
        die(f"缺少构建目录 {BUILD}，先 cmake -S . -B build 并构建一次")

    # 先确保应用本身是编译过的：复用的是它的对象文件
    subprocess.run(["ninja", "-C", str(BUILD), "bin/clash-qt"], check=True)

    commands = ninja("-t", "commands", "bin/clash-qt").splitlines()
    compile_line = next((line for line in commands
                         if "src/app/src/main.cpp" in line and " -c " in line), None)
    link_line = next((line for line in reversed(commands)
                      if " -o bin/clash-qt " in line), None)
    if not compile_line or not link_line:
        die("没能从 ninja 的命令表里取到 clash-qt 的编译/链接命令（构建系统改用 Ninja 了吗？）")

    # ---- 编译：同样的宏与 include，只换源文件与输出 ----
    TOOL_OBJ.parent.mkdir(parents=True, exist_ok=True)
    tokens = shlex.split(compile_line)
    source = next((t for t in tokens if t.endswith("src/app/src/main.cpp")), None)
    if not source:
        die("编译命令里找不到 main.cpp 源文件参数")
    tokens = [str(TOOL_SRC) if t == source else t for t in tokens]
    tokens[tokens.index("-o") + 1] = str(TOOL_OBJ)
    # 依赖文件参数去掉：否则会和主工程的 .d 互相覆盖
    cleaned = []
    skip_next = False
    for token in tokens:
        if skip_next:
            skip_next = False
            continue
        if token in ("-MD", "-MMD"):
            continue
        if token in ("-MT", "-MF"):
            skip_next = True
            continue
        cleaned.append(token)
    print(f"[1/3] 编译 {TOOL_SRC.relative_to(ROOT)}")
    subprocess.run(cleaned, cwd=BUILD, check=True)

    # ---- 链接：把 main.cpp.o 换成快照工具的对象文件 ----
    tokens = shlex.split(link_line.strip().removeprefix(": && ").removesuffix(" && :"))
    tokens = [str(TOOL_OBJ) if t == APP_OBJ else t for t in tokens]
    tokens = [t for t in tokens if not t.startswith("-Wl,--dependency-file=")]
    tokens[tokens.index("-o") + 1] = str(TOOL_BIN)
    print("[2/3] 链接 ui-snapshot（复用已有对象文件，不重编应用）")
    subprocess.run(tokens, cwd=BUILD, check=True)


FAKE_NAMES = {"zh": "示例订阅", "en": "Example profile"}


def patch_profiles(path, lang):
    """把订阅名与订阅链接换成示例值：图片要进仓库，不该带上真实的机场与令牌"""
    text = path.read_text(encoding="utf-8")
    text = re.sub(r"('name': ')[^']*(')", rf"\g<1>{FAKE_NAMES[lang]}\g<2>", text)
    text = re.sub(r"('url': ')[^']*(')", r"\g<1>https://example.com/subscribe\g<2>", text)
    path.write_text(text, encoding="utf-8")


def patch_config(path, lang):
    """把临时配置改成「出图友好」：不动桌面代理、不自动更新、固定浅色与语言"""
    text = path.read_text(encoding="utf-8")

    def set_scalar(key, value):
        nonlocal text
        pattern = rf"^'{re.escape(key)}': '[^']*'$"
        replacement = f"'{key}': '{value}'"
        if re.search(pattern, text, flags=re.MULTILINE):
            text = re.sub(pattern, replacement, text, count=1, flags=re.MULTILINE)
        else:
            text = replacement + "\n" + text

    set_scalar("appTheme", "light")
    set_scalar("language", "zh-CN" if lang == "zh" else "en-US")
    set_scalar("autoCheckUpdate", "false")
    set_scalar("autoUpdateProfileOnStart", "false")
    # 系统代理要关掉：出图不该改动桌面设置，也不能让内核状态显示成「已开启」
    text = re.sub(r"('sysProxy':\n(?:  .*\n)*?  'enable': )'true'", r"\1'false'", text)

    path.write_text(text, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description="渲染 clash-qt 界面截图（离屏）")
    parser.add_argument("--lang", choices=["zh", "en"], default="zh",
                        help="出图语言（默认 zh）")
    parser.add_argument("--out", default=str(ROOT / "docs" / "images"),
                        help="输出目录（默认 docs/images）")
    parser.add_argument("--pages", choices=["main", "all"], default="main",
                        help="main 只出订阅页（默认，可入库）；all 连同代理/规则/"
                             "覆写/设置一起出（含你自己的节点与规则，别提交）")
    args = parser.parse_args()

    build_tool()

    # 用真实配置的副本：订阅、规则、日志都是真的，画面才不是空壳
    base = Path(os.environ.get("XDG_CONFIG_HOME") or (Path.home() / ".config"))
    source_config = base / "clash-qt"
    workdir = Path(tempfile.mkdtemp(prefix="clash-qt-snapshot-"))
    staged = workdir / "clash-qt"
    try:
        if source_config.is_dir():
            shutil.copytree(source_config, staged)
            patch_config(staged / "config.yaml", args.lang)
            if (staged / "profile.yaml").is_file():
                patch_profiles(staged / "profile.yaml", args.lang)
        else:
            staged.mkdir()

        out = Path(args.out)
        out.mkdir(parents=True, exist_ok=True)

        print(f"[3/3] 离屏渲染（语言 {args.lang}）")
        env = dict(os.environ,
                   QT_QPA_PLATFORM="offscreen",
                   XDG_CONFIG_HOME=str(workdir),
                   NO_AT_BRIDGE="1")
        subprocess.run([str(TOOL_BIN), str(workdir / "shots")], env=env, check=True)

        pages = PAGES if args.pages == "all" else ["profiles"]
        for page in pages:
            source = workdir / "shots" / f"{page}.png"
            if not source.is_file():
                die(f"没有生成 {source}")
            name = f"main-{args.lang}.png" if page == "profiles" else f"{page}-{args.lang}.png"
            shutil.copyfile(source, out / name)
            print(f"  {out / name}")
    finally:
        shutil.rmtree(workdir, ignore_errors=True)


if __name__ == "__main__":
    main()
