# SPDX-License-Identifier: MIT
import hashlib
import os
import shlex
import subprocess

def guarded_header(code):
    guard = "C_TOOLBOX_" + hashlib.sha256(code.encode("utf-8")).hexdigest()[:20].upper() + "_H"
    return "#ifndef " + guard + "\n#define " + guard + "\n\n" + code + "\n#endif\n"

def all_header():
    modules = [name for name in BUNDLE["catalog"]["modules"] if name != "core"]
    return guarded_header(generate(modules))

def initialize_folder(answer, base=None):
    base = Path(base or Path(__file__).resolve().parent).resolve()
    version = BUNDLE["catalog"]["version"]
    if not answer.strip():
        target = base / ("工具箱速查_v" + version + ".md")
        text = BUNDLE["manual"]
        if target.exists():
            if target.read_text(encoding="utf-8") != text:
                raise ValueError("手册已存在且内容不同，不覆盖：" + str(target))
        else:
            with target.open("x", encoding="utf-8") as stream:
                stream.write(text)
        print("已导出手册：" + str(target))
        return target
    requested = Path(answer.strip()).expanduser()
    target = (requested if requested.is_absolute() else base / requested).resolve()
    if target.exists():
        message = "文件夹已存在，为保护已有题目未写入任何文件：" + str(target)
        helper = target / "prepare.py"
        if helper.is_file():
            command = [sys.executable, str(helper)]
            display = subprocess.list2cmdline(command) if os.name == "nt" else shlex.join(command)
            message += "\n准备提交请运行：\n" + display
        raise ValueError(message)
    contents = {"c_toolbox.h": all_header(), "prepare.py": BUNDLE["prepare"],
                "main.c": BUNDLE["starter"]}
    target.parent.mkdir(parents=True, exist_ok=True)
    target.mkdir(exist_ok=False)
    created = []
    try:
        for name, content in contents.items():
            path = target / name
            with path.open("x", encoding="utf-8") as stream:
                created.append(path)
                stream.write(content)
    except Exception:
        for path in created:
            path.unlink()
        target.rmdir()
        raise
    print("已创建题目目录：" + str(target))
    print("编辑 main.c；提交时运行同目录 prepare.py。")
    print("旧题的头文件和 prepare.py 保持原版本，不随入口脚本更新。")
    return target

def cli(argv=None):
    for stream in (sys.stdin, sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(encoding="utf-8")
    args = list(sys.argv[1:] if argv is None else argv)
    if args and (args[0] in ("generate", "header") or
                 any(x in args for x in ("--modules", "--list", "--vector", "--deque",
                                         "--heap", "--preamble", "--main", "--header", "--version"))):
        if args[0] == "generate":
            return generate_cli(args[1:])
        if args[0] == "header":
            return generate_cli(["--header"] + args[1:])
        return generate_cli(args)
    parser = argparse.ArgumentParser(description="C Toolbox: initialize a problem folder; Enter exports Markdown.")
    parser.add_argument("--folder", help="noninteractive initialization; empty value exports the manual")
    options = parser.parse_args(args)
    try:
        answer = options.folder if options.folder is not None else input(
            "题目文件夹（留空导出根目录 Markdown 手册）：")
        initialize_folder(answer)
    except (ValueError, OSError) as error:
        print(str(error), file=sys.stderr)
        return 1
    except (KeyboardInterrupt, EOFError):
        print("已取消。", file=sys.stderr)
        return 130
    return 0

if __name__ == "__main__":
    raise SystemExit(cli())
