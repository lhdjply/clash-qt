#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""更新界面翻译文件（translations/*.ts）。

用法：
    python3 scripts/update-translations.py                # 自动找构建目录
    python3 scripts/update-translations.py --build-dir build
    python3 scripts/update-translations.py --strict        # 有未翻译条目就返回 1

为什么要包一层脚本，而不是直接跑 lupdate：
  lupdate 需要知道「哪些源文件属于本目标」。这份清单由 CMake 在配置阶段
  生成（qt_add_translations），手写一份必然随着文件增删而失准 —— 漏掉的
  文件里的 tr() 会悄悄从 .ts 里消失。所以这里直接驱动 CMake 生成的
  update_translations 目标，源文件清单与编译选项永远和构建保持一致。

注意 .ts 是译文唯一的来源，.qm 是构建产物：本脚本只改 .ts，改完重新
构建即可，不需要（也不应该）把 .qm 提交进仓库。
"""

from __future__ import annotations

import argparse
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# 构建目录的候选名：与 CMake 常见约定一致（见 .gitignore）
BUILD_DIR_GLOBS = ("build", "build-*", "cmake-build-*")

TARGET = "update_translations"


def repo_root() -> Path:
    """脚本位于 <repo>/scripts/，仓库根就是它的父目录。"""
    return Path(__file__).resolve().parent.parent


def find_build_dir(root: Path, explicit: str | None) -> Path | None:
    """定位已配置的构建目录。

    explicit 给定时只认它；否则在候选名里找带 CMakeCache.txt 的目录，
    多个时取最近修改的一个（多个构建目录并存时，用户通常刚用过最新的那个）。
    """
    if explicit:
        candidate = Path(explicit).expanduser()
        if not candidate.is_absolute():
            candidate = (root / candidate).resolve()
        return candidate if (candidate / "CMakeCache.txt").is_file() else None

    found: list[Path] = []
    for pattern in BUILD_DIR_GLOBS:
        for path in root.glob(pattern):
            if path.is_dir() and (path / "CMakeCache.txt").is_file():
                found.append(path)
    if not found:
        return None
    return max(found, key=lambda p: p.stat().st_mtime)


def count_messages(ts_path: Path) -> tuple[int, int]:
    """统计一份 .ts 的条目总数与未翻译数。

    未翻译的判定：<translation> 带 type="unfinished"、完全没有 <translation>、
    或译文为空（numerus 形式要求每个 <numerusform> 都有内容）。
    """
    root = ET.parse(ts_path).getroot()
    total = 0
    unfinished = 0
    for message in root.iter("message"):
        total += 1
        translation = message.find("translation")
        if translation is None:
            unfinished += 1
            continue
        if translation.get("type") in ("unfinished", "vanished"):
            unfinished += 1
            continue
        forms = translation.findall("numerusform")
        if forms:
            if any(not (form.text or "").strip() for form in forms):
                unfinished += 1
        elif not (translation.text or "").strip():
            unfinished += 1
    return total, unfinished


def main() -> int:
    parser = argparse.ArgumentParser(
        description="更新 translations/*.ts（调用 CMake 的 update_translations 目标）")
    parser.add_argument("--build-dir", help="构建目录；默认自动探测 build / build-* / cmake-build-*")
    parser.add_argument("--strict", action="store_true",
                        help="存在未翻译条目时返回非 0（适合放进 CI）")
    parser.add_argument("--quiet", action="store_true", help="不打印 cmake 的原始输出")
    args = parser.parse_args()

    root = repo_root()
    translations_dir = root / "translations"
    if not translations_dir.is_dir():
        print(f"找不到翻译目录：{translations_dir}", file=sys.stderr)
        return 2

    build_dir = find_build_dir(root, args.build_dir)
    if build_dir is None:
        # 没配置过就没有源文件清单可依据，直接给出下一步命令
        print("未找到已配置的构建目录。先配置一次 CMake，例如：\n"
              "    cmake -S . -B build -G Ninja\n"
              "（已指定 --build-dir 时请检查该目录下是否有 CMakeCache.txt）",
              file=sys.stderr)
        return 2

    print(f"仓库根目录：{root}")
    print(f"构建目录：{build_dir}")
    print(f"执行：cmake --build {build_dir.name} --target {TARGET}\n")
    # 子进程直接写同一个 fd，脚本自己的输出是块缓冲的：不手动 flush 的话，
    # lupdate 的逐文件报告会插到这几行前面（管道里尤其明显）。
    sys.stdout.flush()

    command = ["cmake", "--build", str(build_dir), "--target", TARGET]
    try:
        # 不捕获输出：lupdate 会逐文件报告新增/已有/移除的条目，
        # 这些信息对排查「某段文案没进 .ts」很有用，没必要吞掉。
        result = subprocess.run(command, cwd=root,
                                stdout=subprocess.DEVNULL if args.quiet else None,
                                stderr=None)
    except FileNotFoundError:
        print("找不到 cmake，请确认构建工具已安装并在 PATH 中。", file=sys.stderr)
        return 2

    if result.returncode != 0:
        print(f"\n更新失败（cmake 返回 {result.returncode}）。\n"
              f"常见原因：未安装 qt6-l10n-tools（缺 lupdate），"
              f"或目标 {TARGET} 未生成。", file=sys.stderr)
        return result.returncode

    # ---- 汇总 -------------------------------------------------------------
    files = sorted(translations_dir.glob("*.ts"))
    if not files:
        print(f"\n{translations_dir} 下没有 .ts 文件。", file=sys.stderr)
        return 2

    name_width = max(len(path.name) for path in files)
    total_all = 0
    unfinished_all = 0
    print("\n" + "-" * 56)
    for path in files:
        try:
            total, unfinished = count_messages(path)
        except ET.ParseError as error:
            print(f"{path.name:<{name_width}}  解析失败：{error}", file=sys.stderr)
            return 1
        total_all += total
        unfinished_all += unfinished
        mark = "" if unfinished == 0 else f"  ← 待补 {unfinished} 条"
        print(f"{path.name:<{name_width}}  {total:>4} 条  未翻译 {unfinished}{mark}")
    print("-" * 56)
    print(f"合计 {total_all} 条，未翻译 {unfinished_all} 条")

    if unfinished_all:
        print("\n新增或改动的文案已标记为「未翻译」，请在对应 .ts 里补译文；"
              "译文为空时界面会显示英文原文（源语言），不会出现空白。")
        return 1 if args.strict else 0

    print("\n全部已翻译。重新构建即可生成 .qm（.ts 是源，.qm 不入库）。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
