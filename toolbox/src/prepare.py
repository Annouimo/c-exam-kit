# SPDX-License-Identifier: MIT
"""Standalone submission preparation. Generated into each problem folder."""
import argparse
import ctypes
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import time

VERSION = "@VERSION@"
EXPORT_MARKER = "/* C Toolbox submission"

def scan_sources(folder):
    candidates = []
    for path in folder.iterdir():
        if path.is_file() and path.suffix.lower() == ".c":
            start = path.read_text(encoding="utf-8-sig")[:128]
            if not start.startswith(EXPORT_MARKER):
                candidates.append(path)
    return sorted(candidates, key=lambda p: (p.name.casefold(), p.name))

def choose_source(folder, source=None, ask=input):
    if source is not None:
        path = Path(source)
        path = path if path.is_absolute() else folder / path
        if not path.is_file() or path.suffix.lower() != ".c":
            raise ValueError("找不到 C 源文件：" + str(path))
        return path.resolve()
    candidates = scan_sources(folder)
    if not candidates:
        raise ValueError("同目录没有可提交的 C 源文件。")
    if len(candidates) == 1:
        return candidates[0].resolve()
    print("选择要提交的 C 文件：")
    for i, path in enumerate(candidates, 1):
        print("  %d. %s" % (i, path.name))
    answer = ask("编号（q 退出）：").strip()
    if answer.lower() == "q":
        raise KeyboardInterrupt()
    if not answer.isdigit() or not 1 <= int(answer) <= len(candidates):
        raise ValueError("编号无效，请重新运行。")
    return candidates[int(answer) - 1].resolve()

def logical_lines(text):
    pieces = text.split("\n")
    lines = [piece + "\n" for piece in pieces[:-1]] + ([pieces[-1]] if pieces[-1] else [])
    i = 0
    while i < len(lines):
        start = i + 1
        raw = lines[i]
        i += 1
        while raw.endswith("\\\n") and i < len(lines):
            raw += lines[i]
            i += 1
        yield start, i + 1, raw

def strip_comments(text, in_block):
    """Leave literals intact so include-looking text inside strings cannot match."""
    result = []
    quote = None
    i = 0
    while i < len(text):
        ch = text[i]
        pair = text[i:i + 2]
        if in_block:
            if pair == "*/":
                result.append("  "); in_block = False; i += 2
            else:
                result.append("\n" if ch == "\n" else " "); i += 1
        elif quote:
            result.append(ch); i += 1
            if ch == "\\" and i < len(text):
                result.append(text[i]); i += 1
            elif ch == quote:
                quote = None
        elif pair == "/*":
            result.append("  "); in_block = True; i += 2
        elif pair == "//":
            result.extend("\n" if c == "\n" else " " for c in text[i:])
            break
        else:
            result.append(ch); i += 1
            if ch in ("'", '"'):
                quote = ch
    return "".join(result), in_block

def expand_source(source):
    root = source.parent
    dependencies = set()
    active = []
    def label(path):
        try:
            return path.relative_to(root).as_posix()
        except ValueError:
            return path.as_posix()
    def line(number, path):
        return "#line %d %s\n" % (number, json.dumps(label(path), ensure_ascii=True))
    def expand(path):
        path = path.resolve()
        if path in active:
            raise ValueError("本地头文件循环引用：" + " -> ".join(p.name for p in active + [path]))
        if not path.is_file():
            raise ValueError("找不到本地引用：" + str(path))
        dependencies.add(path)
        active.append(path)
        text = path.read_text(encoding="utf-8-sig")
        result = [line(1, path)]
        in_block = False
        for start, following, raw in logical_lines(text):
            view, in_block = strip_comments(raw.replace("\\\n", ""), in_block)
            directive = re.match(r"^\s*#\s*include\b(.*)$", view, re.S)
            if not directive:
                result.append(view + "\n" * max(0, raw.count("\n") - view.count("\n")))
                continue
            argument = directive.group(1).strip()
            if re.fullmatch(r"<[^>\n]+>", argument):
                result.append(view + "\n" * max(0, raw.count("\n") - view.count("\n")))
                continue
            quoted = re.fullmatch(r'"([^"\n]+)"', argument)
            if not quoted:
                raise ValueError("%s:%d 不支持宏形式的 include，请改为字面文件名。" % (label(path), start))
            included = (path.parent / quoted.group(1)).resolve()
            # Expand at each occurrence: a prior include may be in #if 0.
            # Original include guards remain responsible for deduplication.
            result.append(expand(included))
            result.append(line(following, path))
        active.pop()
        return "".join(result) + "\n"
    code = EXPORT_MARKER + " v" + VERSION + " | generated; edit the original source */\n"
    code += expand(source)
    return code, dependencies

def compiler_command(override=None):
    if override:
        return [override]
    if os.environ.get("CC"):
        parts = shlex.split(os.environ["CC"], posix=os.name != "nt")
        return [p.strip('"') for p in parts] if os.name == "nt" else parts
    for name in ("cc", "gcc", "clang"):
        executable = shutil.which(name)
        if executable:
            return [executable]
    raise ValueError("找不到 C 编译器。请把 GCC/Clang 加入 PATH，或使用 --cc 编译器路径。")

def check_code(output, folder, compiler=None, run=False, cwd=None):
    command = compiler_command(compiler)
    workdir = Path(cwd or output.parent.parent).resolve()
    scratch = workdir / ".toolbox"
    scratch.mkdir(exist_ok=True)
    # Use the writable problem directory; no dependency on a system temp folder.
    with tempfile.TemporaryDirectory(prefix="check_", dir=str(scratch)) as temp:
        binary = Path(temp) / ("answer.exe" if os.name == "nt" else "answer")
        staged = Path(temp) / "answer.c"
        staged.write_bytes(output.read_bytes())
        args = command + ["-std=c99", "-O2", "-Wall", "-Wextra", "-Wno-unused-function",
                          "-x", "c", os.path.relpath(staged, workdir),
                          "-o", os.path.relpath(binary, workdir), "-lm"]
        compiled = subprocess.run(args, cwd=str(workdir),
                                  capture_output=True, text=True, errors="replace")
        if compiled.stderr:
            print(compiled.stderr, file=sys.stderr, end="")
        if compiled.returncode:
            raise ValueError("编译检查失败，未复制到剪贴板；请修改原 C 文件。")
        if run:
            return subprocess.run([str(binary)], cwd=str(workdir)).returncode
    return 0

def windows_clipboard(text):
    from ctypes import wintypes
    user = ctypes.WinDLL("user32", use_last_error=True)
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    user.OpenClipboard.argtypes = [wintypes.HWND]
    user.OpenClipboard.restype = wintypes.BOOL
    user.EmptyClipboard.restype = wintypes.BOOL
    user.SetClipboardData.argtypes = [wintypes.UINT, wintypes.HANDLE]
    user.SetClipboardData.restype = wintypes.HANDLE
    user.CloseClipboard.restype = wintypes.BOOL
    kernel.GlobalAlloc.argtypes = [wintypes.UINT, ctypes.c_size_t]
    kernel.GlobalAlloc.restype = wintypes.HANDLE
    kernel.GlobalLock.argtypes = [wintypes.HANDLE]
    kernel.GlobalLock.restype = ctypes.c_void_p
    kernel.GlobalUnlock.argtypes = [wintypes.HANDLE]
    kernel.GlobalFree.argtypes = [wintypes.HANDLE]
    kernel.GlobalFree.restype = wintypes.HANDLE
    payload = (text + "\0").encode("utf-16-le")
    handle = kernel.GlobalAlloc(0x0002, len(payload))
    if not handle:
        raise OSError("剪贴板内存分配失败")
    opened = False
    transferred = False
    try:
        pointer = kernel.GlobalLock(handle)
        if not pointer:
            raise OSError("剪贴板内存锁定失败")
        ctypes.memmove(pointer, payload, len(payload))
        kernel.GlobalUnlock(handle)
        for _ in range(10):
            if user.OpenClipboard(None):
                opened = True
                break
            time.sleep(0.05)
        if not opened or not user.EmptyClipboard():
            raise OSError("无法打开剪贴板")
        if not user.SetClipboardData(13, handle):  # CF_UNICODETEXT
            raise OSError("无法写入剪贴板")
        transferred = True
    finally:
        if opened:
            user.CloseClipboard()
        if not transferred:
            kernel.GlobalFree(handle)

def copy_clipboard(text):
    if os.name == "nt":
        windows_clipboard(text)
        return
    if sys.platform == "darwin":
        commands = [["pbcopy"]]
    else:
        commands = []
        if os.environ.get("WAYLAND_DISPLAY"):
            commands.append(["wl-copy"])
        if os.environ.get("DISPLAY"):
            commands.extend([["xclip", "-selection", "clipboard"], ["xsel", "--clipboard", "--input"]])
    for command in commands:
        if shutil.which(command[0]):
            completed = subprocess.run(command, input=text.encode("utf-8"),
                                       stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
            if completed.returncode == 0:
                return
    raise OSError("剪贴板不可用")

def prepare(folder, source=None, output=None, compiler=None, no_copy=False, run=False, ask=input):
    folder = Path(folder).resolve()
    selected = choose_source(folder, source, ask)
    code, dependencies = expand_source(selected)
    target = Path(output) if output else folder / ".toolbox" / "submit.c"
    if not target.is_absolute():
        target = folder / target
    target = target.resolve()
    if target in dependencies:
        raise ValueError("输出位置不能覆盖原文件或其本地引用。")
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(code, encoding="utf-8")
    print("源文件：" + selected.name)
    status = check_code(target, target.parent, compiler, run, selected.parent)
    if run:
        return status
    print("编译检查通过，已保存：" + str(target))
    if no_copy:
        return 0
    try:
        copy_clipboard(code)
    except OSError as error:
        print(str(error) + "；请从上述文件复制源码。", file=sys.stderr)
        return 2
    print("已复制到剪贴板，可以提交。")
    return 0

def prepare_cli(argv=None):
    for stream in (sys.stdin, sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(description="扫描同目录 C 文件，展开、编译检查并复制提交源码。")
    parser.add_argument("source", nargs="?", help="可选：指定文件，省略则自动扫描")
    parser.add_argument("--output", "-o")
    parser.add_argument("--cc", help="编译器可执行文件路径")
    parser.add_argument("--no-copy", action="store_true", help="仅检查和保存，不访问剪贴板")
    parser.add_argument("--run", action="store_true", help="编译并运行，不复制")
    parser.add_argument("--version", action="version", version=VERSION)
    args = parser.parse_args(argv)
    try:
        return prepare(Path(__file__).resolve().parent, args.source, args.output,
                       args.cc, args.no_copy, args.run)
    except (ValueError, OSError, UnicodeError) as error:
        print(str(error), file=sys.stderr)
        return 1
    except (KeyboardInterrupt, EOFError):
        print("已取消。", file=sys.stderr)
        return 130

if __name__ == "__main__":
    raise SystemExit(prepare_cli())
