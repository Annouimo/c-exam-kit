# SPDX-License-Identifier: MIT
import contextlib
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import types
import unittest
from unittest.mock import Mock

import test_toolbox as core

tb = core.tb

def load_prepare(folder):
    namespace = {"__name__": "prepare_test", "__file__": str(folder / "prepare.py")}
    exec(compile(tb.BUNDLE["prepare"], namespace["__file__"], "exec"), namespace)
    return namespace

class WorkflowTests(unittest.TestCase):
    run_c = core.ToolboxTests.run_c

    def test_single_file_installer_and_manual(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            installer = root / "toolbox.py"
            installer.write_bytes(core.STANDALONE.read_bytes())
            other = root / "elsewhere"; other.mkdir()
            result = subprocess.run([sys.executable, str(installer)], input="练习/A\n",
                cwd=other, capture_output=True, text=True, encoding="utf-8")
            self.assertEqual(result.returncode, 0, result.stderr)
            folder = root / "练习/A"
            self.assertEqual({p.name for p in folder.iterdir()}, {"main.c", "c_toolbox.h", "prepare.py"})
            self.assertIn('#include "c_toolbox.h"', (folder / "main.c").read_text())
            original = (folder / "main.c").read_bytes()
            result = subprocess.run([sys.executable, str(folder / "prepare.py"), "--no-copy"],
                cwd=other, capture_output=True, text=True, encoding="utf-8")
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(original, (folder / "main.c").read_bytes())
            self.assertTrue((folder / ".toolbox/submit.c").exists())
            result = subprocess.run([sys.executable, str(installer)], input="\n",
                cwd=other, capture_output=True, text=True, encoding="utf-8")
            self.assertEqual(result.returncode, 0, result.stderr)
            manual = root / ("工具箱速查_v" + tb.BUNDLE["catalog"]["version"] + ".md")
            self.assertEqual(manual.read_text(encoding="utf-8"), tb.BUNDLE["manual"])
            self.assertFalse((folder / manual.name).exists())
            again = subprocess.run([sys.executable, str(installer), "--folder", "练习/A"],
                capture_output=True, text=True, encoding="utf-8")
            self.assertNotEqual(again.returncode, 0)
            self.assertEqual(original, (folder / "main.c").read_bytes())
            self.assertIn("已存在", again.stderr)
            self.assertIn("准备提交请运行", again.stderr)
            self.assertIn("prepare.py", again.stderr)

    def test_scan_and_select(self):
        with tempfile.TemporaryDirectory() as temp:
            folder = Path(temp); p = load_prepare(folder)
            with self.assertRaises(ValueError):
                p["choose_source"](folder)
            (folder / "b.c").write_text("int main(void){return 0;}")
            nested = folder / "nested"; nested.mkdir()
            (nested / "x.c").write_text("irrelevant")
            (folder / "old_export.c").write_text(p["EXPORT_MARKER"] + " */\n")
            self.assertEqual(p["choose_source"](folder).name, "b.c")
            (folder / "a.c").write_text("int main(void){return 0;}")
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(p["choose_source"](folder, ask=lambda _: "2").name, "b.c")
                with self.assertRaises(ValueError):
                    p["choose_source"](folder, ask=lambda _: "99")
                with self.assertRaises(KeyboardInterrupt):
                    p["choose_source"](folder, ask=lambda _: "q")

    def test_expansion_comments_conditions_and_repeated_includes(self):
        with tempfile.TemporaryDirectory() as temp:
            folder = Path(temp); p = load_prepare(folder)
            (folder / "c_toolbox.h").write_text(tb.all_header(), encoding="utf-8")
            sub = folder / "sub"; sub.mkdir()
            (sub / "constant.h").write_text("#ifndef CONSTANT_H\n#define CONSTANT_H\n#define VALUE 7\n#endif\n")
            (folder / "helper.h").write_text('#ifndef HELPER_H\n#define HELPER_H\n#include "sub/constant.h"\nstatic int helper(void){return VALUE;}\n#endif\n')
            source = folder / "main.c"
            source.write_text(r'''#include <stdio.h> /* start a comment
that closes on a following line */
#include "c_toolbox.h"
#include "c_toolbox.h"
#if 0
#include "helper.h"
#endif
/* include-looking data must be ignored:
#include "missing.h"
*/ #include \
 "helper.h"
int main(void) {
    const char *text = "#include \"not_a_header.h\"";
    IntVector values = IntVector_values(helper(), 3);
    print_ints(&values, " ", "\n");
    IntVector_free(&values);
    return text[0] != '#';
}
''')
            code, dependencies = p["expand_source"](source)
            self.assertEqual(len(dependencies), 4)
            self.assertIn("#include <stdio.h>", code)
            self.assertNotIn('#include "helper.h"', code)
            self.assertNotIn('#include "missing.h"', code)
            self.assertIn('not_a_header.h', code)
            self.assertEqual(self.run_c(code), "7 3\n")
            self.assertEqual(source.read_text().count('#include "helper.h"'), 1)
            # Compile the original include-based source too.
            out = folder / "answer.c"
            out.write_text('#include "helper.h"\n')
            original = subprocess.run(p["compiler_command"]() +
                ["-std=c99", "main.c", "-o", "original.exe", "-lm"],
                cwd=folder, capture_output=True, text=True)
            self.assertEqual(original.returncode, 0, original.stderr)

    def test_expand_failures_and_source_protection(self):
        with tempfile.TemporaryDirectory() as temp:
            folder = Path(temp); p = load_prepare(folder)
            source = folder / "main.c"
            source.write_text('#include "missing.h"\nint main(void){return 0;}\n')
            with self.assertRaises(ValueError):
                p["expand_source"](source)
            source.write_text('#define H "a.h"\n#include H\n')
            with self.assertRaises(ValueError):
                p["expand_source"](source)
            source.write_text('#include "a.h"\n')
            (folder / "a.h").write_text('#include "main.c"\n')
            with self.assertRaises(ValueError):
                p["expand_source"](source)
            source.write_text('int main(void){return 0;}\n')
            with self.assertRaises(ValueError):
                p["prepare"](folder, output=source)
            self.assertEqual(source.read_text(), 'int main(void){return 0;}\n')

    def test_copy_only_after_success_and_fallback(self):
        with tempfile.TemporaryDirectory() as temp:
            folder = Path(temp); p = load_prepare(folder)
            source = folder / "main.c"
            source.write_text("int main(void){return 0;}\n")
            copied = Mock(); p["copy_clipboard"] = copied
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(p["prepare"](folder), 0)
            code = (folder / ".toolbox/submit.c").read_text()
            copied.assert_called_once_with(code)
            copied.reset_mock()
            source.write_text("int main(void){return 7;}\n")
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(p["prepare"](folder, run=True), 7)
            copied.assert_not_called()
            source.write_text("int main(void){syntax error}\n")
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(ValueError):
                    p["prepare"](folder)
            copied.assert_not_called()
            source.write_text("int main(void){return 0;}\n")
            p["copy_clipboard"] = Mock(side_effect=OSError("no clipboard"))
            with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
                self.assertEqual(p["prepare"](folder), 2)
            self.assertTrue((folder / ".toolbox/submit.c").exists())
            p["compiler_command"] = Mock(side_effect=ValueError("no compiler"))
            p["copy_clipboard"] = copied
            with contextlib.redirect_stdout(io.StringIO()), self.assertRaises(ValueError):
                p["prepare"](folder)
            copied.assert_not_called()

    def test_clipboard_utf8_and_windows_dispatch(self):
        p = load_prepare(Path("."))
        text = "中文\nint main(void){}"
        p["os"] = types.SimpleNamespace(name="posix", environ={})
        p["sys"] = types.SimpleNamespace(platform="darwin")
        p["shutil"] = types.SimpleNamespace(which=lambda _: "/usr/bin/pbcopy")
        runner = Mock(return_value=types.SimpleNamespace(returncode=0))
        p["subprocess"] = types.SimpleNamespace(run=runner, DEVNULL=-3, PIPE=-1)
        p["copy_clipboard"](text)
        self.assertEqual(runner.call_args.kwargs["input"], text.encode("utf-8"))
        p["os"] = types.SimpleNamespace(name="nt")
        backend = Mock(); p["windows_clipboard"] = backend
        p["copy_clipboard"](text); backend.assert_called_once_with(text)

    def test_vector_functional_and_range(self):
        self.run_c(tb.generate(["io"], main=r"""
#include <assert.h>
static bool positive(int x) { return x > 0; }
static int square(int x) { return x*x; }
static int add(int a, int b) { return a+b; }
int main(void) {
    int calls = 0;
    IntVector v = IntVector_values(++calls, 0, -2, 3);
    assert(calls == 1);
    IntVector selected = IntVector_filter(&v, positive);
    IntVector mapped = IntVector_map(&selected, square);
    assert(IntVector_reduce(&mapped, 0, add) == 10);
    assert(IntVector_any(&v, positive) && !IntVector_all(&v, positive));
    assert(IntVector_count_if(&v, positive) == 2 && IntVector_find_if(&v, positive) == 0);
    IntVector_map_inplace(&v, square); assert(IntVector_at(&v, 2) == 4);
    IntVector_free(&v); IntVector_free(&selected); IntVector_free(&mapped);
    v = IntVector_range(5, -2, -2);
    assert(IntVector_size(&v) == 4 && IntVector_at(&v, 3) == -1); IntVector_free(&v);
    v = IntVector_range(INT_MAX-1, INT_MAX, 2);
    assert(IntVector_size(&v) == 1); IntVector_free(&v);
    v = IntVector_range(INT_MIN+1, INT_MIN, -2);
    assert(IntVector_size(&v) == 1); IntVector_free(&v);
    assert(IntVector_all(&v, positive) && !IntVector_any(&v, positive));
    assert(IntVector_reduce(&v, 17, add) == 17);
    assert(IntVector_find_if(&v, positive) == SIZE_MAX);
    LongLongVector w = LongLongVector_range(LLONG_MAX-1, LLONG_MAX, 2);
    assert(LongLongVector_size(&w) == 1); LongLongVector_free(&w);
    return 0;
}"""))
        self.run_c(tb.generate(["vector"], main="int main(void){(void)IntVector_range(0,3,0);return 0;}"),
                   exit_code=1, error_contains="step must not be zero")

    def test_generic_struct_mapping_and_header_mode(self):
        self.run_c(tb.generate(["vector"], ["Rows=Row"], "typedef struct { int x; } Row;", r"""
#include <assert.h>
static bool keep(Row r) { return r.x > 0; }
static Row twice(Row r) { r.x *= 2; return r; }
int main(void) {
    Rows a = Rows_values(((Row){2}), ((Row){-1}));
    Rows b = Rows_filter(&a, keep), c = Rows_map(&b, twice);
    assert(Rows_at(&c, 0).x == 4);
    Rows_free(&a); Rows_free(&b); Rows_free(&c); return 0;
}"""))
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp); header = root / "custom.h"
            subprocess.run([sys.executable, str(core.STANDALONE), "header",
                            "--modules", "vector", "-o", str(header)], check=True)
            self.run_c('#include "' + header.as_posix() + '"\n#include "' + header.as_posix() +
                       '"\nint main(void){return 0;}\n')
        self.assertLess(len(tb.BUNDLE["manual"].encode("utf-8")), 24000)
        self.assertNotIn("@VERSION@", tb.BUNDLE["manual"])

if __name__ == "__main__":
    unittest.main(verbosity=2)
