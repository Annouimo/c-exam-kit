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
            raise ValueError("container must be Name=CType, e.g. IntVector=int")
        if name in used or name in reserved:
            raise ValueError("duplicate/reserved container name: " + name)
        # CType is trusted local C source, but not an entire declaration.
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_ \t*]*", ctype):
            raise ValueError("use a simple C type or a typedef for: " + name)
        used.add(name)
        vectors.append((name, ctype))
    return vectors

def generate(modules, specs=(), preamble="", main="", deques=(), heaps=()):
    ordered = resolve(modules)
    parameters = {"vector": (specs, "IntVector=int"),
                  "deque": (deques, "IntDeque=int"),
                  "heap": (heaps, "IntHeap=int")}
    instances = []
    for kind, (choices, default) in parameters.items():
        if choices and kind not in ordered:
            raise ValueError("--" + kind + " requires the " + kind + " module")
        if kind in ordered:
            instances.extend((kind, name, ctype)
                             for name, ctype in vectors_from(choices or [default]))
    names = [name for _, name, _ in instances]
    if len(set(names)) != len(names):
        raise ValueError("container names must be unique across all modules")
    # Known library element types pull in their definitions automatically.
    library_types = {"String": "string", "StringList": "string", "Map": "map",
                     "MapEntry": "map", "Set": "set", "Counter": "counter"}
    for _, _, ctype in instances:
        for token in re.findall(r"[A-Za-z_][A-Za-z0-9_]*", ctype):
            if token in library_types:
                ordered = resolve(ordered + [library_types[token]])
    parts = ["/* C Toolbox v" + BUNDLE["catalog"]["version"] +
             " | Generated C99 | SPDX-License-Identifier: MIT-0 */",
             BUNDLE["templates"]["core.c"]]
    def template_for(name):
        entry = BUNDLE["catalog"]["modules"][name]
        return "\n".join(BUNDLE["templates"][path] for path in
                         [entry["template"]] + entry.get("extra_templates", []))
    for name in ordered:
        if name == "core" or name in parameters:
            continue
        parts.append(template_for(name))
    if preamble:
        parts.append("/* User type declarations */\n" + preamble)
    # Resolve dependencies between explicitly configured container types as well.
    emitted = set()
    while instances:
        ready = [entry for entry in instances if
                 not ((set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", entry[2])) & set(names)) - emitted)]
        if not ready:
            raise ValueError("cyclic container element types")
        for kind, typename, ctype in ready:
            parts.append(template_for(kind).replace("@TYPE@", ctype).replace("@NAME@", typename))
            emitted.add(typename)
            instances.remove((kind, typename, ctype))
    if main:
        parts.append("/* User program */\n" + main)
    return "\n\n".join(parts) + "\n"

def cli(argv=None):
    parser = argparse.ArgumentParser(description="Offline C99 toolbox assembler")
    parser.add_argument("--version", action="version", version=BUNDLE["catalog"]["version"])
    parser.add_argument("--list", action="store_true", help="list modules")
    parser.add_argument("--modules", nargs="+", help="selected modules (dependencies automatic)")
    parser.add_argument("--vector", action="append", default=[], metavar="Name=CType")
    parser.add_argument("--deque", action="append", default=[], metavar="Name=CType")
    parser.add_argument("--heap", action="append", default=[], metavar="Name=CType")
    parser.add_argument("--preamble", type=Path, help="C type declarations inserted before typed containers")
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
            print("Modules: string vector map set counter deque heap", file=sys.stderr)
            modules = input("Select (space separated; Enter = all): ").split()
        modules = modules or ["string", "vector", "map", "set", "counter", "deque", "heap"]
    try:
        code = generate(modules, args.vector,
                        args.preamble.read_text(encoding="utf-8") if args.preamble else "",
                        args.main.read_text(encoding="utf-8") if args.main else "",
                        args.deque, args.heap)
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

