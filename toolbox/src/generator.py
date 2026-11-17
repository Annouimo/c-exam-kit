# SPDX-License-Identifier: MIT
"""Runtime logic. Build script injects BUNDLE (catalog and C templates)."""
import argparse
import json
from pathlib import Path
import re
import sys

def resolve(modules):
    result = []
    visiting = set()
    def visit(name):
        if name not in BUNDLE["catalog"]["modules"]:
            raise ValueError("unknown module: " + name)
        if name in visiting:
            raise ValueError("dependency cycle: " + name)
        if name in result:
            return
        visiting.add(name)
        for dependency in BUNDLE["catalog"]["modules"][name]["requires"]:
            visit(dependency)
        visiting.remove(name)
        result.append(name)
    for name in modules:
        visit(name)
    return result

def vectors_from(specs):
    vectors = []
    used = set()
    reserved = {"String", "StringList", "Map", "MapEntry", "Set", "Counter",
                "int", "char", "long", "short", "float", "double", "void",
                "struct", "union", "enum", "bool", "size_t", "FILE"}
    for spec in specs:
        name, separator, ctype = spec.partition("=")
        name, ctype = name.strip(), ctype.strip()
        if not separator or not re.fullmatch(r"[A-Z][A-Za-z0-9_]*", name):
            raise ValueError("vector must be Name=CType, e.g. IntVector=int")
        if name in used or name in reserved:
            raise ValueError("duplicate/reserved vector name: " + name)
        # CType is trusted local C source, but not an entire declaration.
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_ \t*]*", ctype):
            raise ValueError("use a simple C type or a typedef for: " + name)
        used.add(name)
        vectors.append((name, ctype))
    return vectors

def generate(modules, specs=(), preamble="", main=""):
    ordered = resolve(modules)
    vectors = vectors_from(specs or ["IntVector=int"]) if "vector" in ordered else []
    if specs and "vector" not in ordered:
        raise ValueError("--vector requires the vector module")
    parts = ["/* C Toolbox v" + BUNDLE["catalog"]["version"] +
             " | Generated C99 | SPDX-License-Identifier: MIT-0 */",
             BUNDLE["templates"]["core.c"]]
    if preamble:
        parts.append("/* User type declarations */\n" + preamble)
    for name in ordered:
        if name == "core":
            continue
        entry = BUNDLE["catalog"]["modules"][name]
        template = BUNDLE["templates"][entry["template"]]
        if name == "vector":
            for typename, ctype in vectors:
                parts.append(template.replace("@TYPE@", ctype).replace("@NAME@", typename))
        else:
            parts.append(template)
    if main:
        parts.append("/* User program */\n" + main)
    return "\n\n".join(parts) + "\n"

def cli(argv=None):
    parser = argparse.ArgumentParser(description="Offline C99 toolbox assembler")
    parser.add_argument("--version", action="version", version=BUNDLE["catalog"]["version"])
    parser.add_argument("--list", action="store_true", help="list modules")
    parser.add_argument("--modules", nargs="+", help="selected modules (dependencies automatic)")
    parser.add_argument("--vector", action="append", default=[], metavar="Name=CType")
    parser.add_argument("--preamble", type=Path, help="C type declarations inserted before components")
    parser.add_argument("--main", type=Path, help="append your complete C main program")
    parser.add_argument("-o", "--output", type=Path, help="write UTF-8 C source; default stdout")
    args = parser.parse_args(argv)
    if args.list:
        for name, entry in BUNDLE["catalog"]["modules"].items():
            if name != "core":
                print(name + ": " + entry["description"])
        return 0
    modules = args.modules
    if modules is None:
        if sys.stdin.isatty():
            print("Modules: string vector map set counter", file=sys.stderr)
            modules = input("Select (space separated; Enter = all): ").split()
        modules = modules or ["string", "vector", "map", "set", "counter"]
    try:
        code = generate(modules, args.vector,
                        args.preamble.read_text(encoding="utf-8") if args.preamble else "",
                        args.main.read_text(encoding="utf-8") if args.main else "")
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(code, encoding="utf-8")
        else:
            sys.stdout.write(code)
    except (ValueError, OSError) as error:
        parser.error(str(error))
    return 0

if __name__ == "__main__":
    raise SystemExit(cli())
