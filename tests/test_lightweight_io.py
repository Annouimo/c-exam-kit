"""Compile-and-run tests for the standalone manifest-v1 lightweight IO preset."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PRESET = ROOT / "scaffold" / "presets" / "exam-io-c99"
MANIFEST = json.loads((PRESET / "manifest.json").read_text(encoding="utf-8"))
COMPONENTS = {item["id"]: item for item in MANIFEST["components"]}
CC = os.environ.get("CC", "cc")


def header(selected=None):
    """Model dependency ordering; the upstream CLI owns identifier parsing."""
    if selected is None:
        selected = set(COMPONENTS)
    else:
        result = set()
        def select(name):
            if name in result:
                return
            for dep in COMPONENTS[name]["requires"]:
                select(dep)
            result.add(name)
        for name in selected:
            select(name)
        selected = result
    content = ["#ifndef EXAM_TEST_H", "#define EXAM_TEST_H"]
    content.extend("#include <{}>".format(h) for h in MANIFEST["systemHeaders"])
    content.extend((PRESET / item["source"]).read_text(encoding="utf-8")
                   for item in MANIFEST["components"] if item["id"] in selected)
    content.append("#endif")
    return "\n".join(content) + "\n"


class IOPresetTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="exam-io-")
        self.addCleanup(self.tmp.cleanup)
        self.folder = Path(self.tmp.name)

    def run_c(self, main, stdin="", selected=None, expect_failure=False):
        (self.folder / "exam.h").write_text(header(selected), encoding="utf-8")
        (self.folder / "main.c").write_text(
            '#include "exam.h"\n' + main + "\n", encoding="utf-8")
        executable = self.folder / ("program.exe" if os.name == "nt" else "program")
        command = [CC, "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                   str(self.folder / "main.c"), "-o", str(executable), "-lm"]
        compilation = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(compilation.returncode, 0, compilation.stderr)
        finished = subprocess.run([str(executable)], input=stdin, text=True,
                                  capture_output=True, timeout=12)
        if expect_failure:
            self.assertNotEqual(finished.returncode, 0)
            self.assertIn("exam io:", finished.stderr)
        else:
            self.assertEqual(finished.returncode, 0, finished.stderr)
        return finished.stdout

    def test_manifest_and_dependencies(self):
        self.assertEqual(MANIFEST["schemaVersion"], 1)
        self.assertEqual(MANIFEST["language"], "c")
        self.assertEqual(MANIFEST["standard"], "c99")
        self.assertEqual(MANIFEST["headerFile"], "exam.h")
        symbols = set()
        for item in MANIFEST["components"]:
            self.assertTrue((PRESET / item["source"]).is_file())
            self.assertTrue(set(item["requires"]) <= set(COMPONENTS))
            for sym in item["symbols"]:
                self.assertNotIn(sym, symbols)
                symbols.add(sym)
            self.assertNotIn("#include", (PRESET / item["source"]).read_text())
        selected = header({"io_ints", "print_scalar"})
        self.assertIn("read_ints_into", selected)
        self.assertIn("try_read_int", selected)
        self.assertNotIn("read_doubles_into", selected)
        self.assertNotIn("try_read_double", selected)
        self.assertNotIn("read_word_into", selected)

    def test_eof_and_t_groups(self):
        main = r"""
int main(void) {
    int a, b;
    while (try_read_int(&a)) {
        b = read_int();
        print_int(a + b, "\n");
    }
    return 0;
}"""
        self.assertEqual(self.run_c(main, "1 2\n3 4\n",
                                    {"io_int", "print_scalar"}), "3\n7\n")
        self.run_c(main, "1\n", {"io_int", "print_scalar"}, expect_failure=True)
        self.assertEqual(self.run_c(
            r'int main(void){for(int t=read_int();t>0;--t) print_int(read_int(),"\n");}',
            "2\n11\n22\n", {"io_int", "print_scalar"}), "11\n22\n")

    def test_line_after_token_and_real_blank_line(self):
        main = r"""
int main(void) {
    int n = read_int();
    char s[80];
    read_line(s);
    print_text(s, "|");
    read_line(s);
    print_text(s, "|");
    read_line(s);
    print_text(s, "|");
    print_int(n, "\n");
    return 0;
}"""
        self.assertEqual(self.run_c(main, "3\nHello\r\n\r\nWorld\n",
                                    {"io_int", "io_line", "print_scalar"}),
                         "Hello||World|3\n")
        self.assertEqual(self.run_c(main, "3 ignored\nHello\n\nWorld\n",
                                    {"io_int", "io_line", "print_scalar"}),
                         "Hello||World|3\n")

    def test_rest_and_words(self):
        main = r"""
int main(void) {
    int n = read_int();
    char rest[80], word[80], line[80];
    read_rest(rest);
    read_word(word);
    read_line(line);
    print_int(n, "|");
    print_text(rest, "|");
    print_text(word, "|");
    print_text(line, "\n");
    return 0;
}"""
        self.assertEqual(self.run_c(main, "5 remaining text\nword\nLine here\n",
                                    {"io_int", "io_line", "io_word", "print_scalar"}),
                         "5| remaining text|word|Line here\n")

    def test_arrays_and_chars(self):
        main = r"""
int main(void) {
    int a[4], n = read_int();
    long long large[2];
    double d[2];
    read_ints(a, n);
    read_lls(large, 2);
    read_doubles(d, 2);
    print_ints(a, n, ",", "|");
    print_lls(large, 2, ",", "|");
    print_doubles(d, 2, 1, ",", "\n");
    char c = read_char();
    char raw = read_char_raw();
    print_char(c, "|");
    print_int(raw == ' ', "\n");
    return 0;
}"""
        self.assertEqual(self.run_c(main, "3 1 2 3 10 20 1.25 2.5\nX Y\n",
                                    {"io_ints", "io_lls", "io_doubles",
                                     "io_char", "print_arrays"}),
                         "1,2,3|10,20|1.2,2.5\nX|1\n")
        self.run_c(main, "5", {"io_ints", "io_lls", "io_doubles",
                               "io_char", "print_arrays"}, expect_failure=True)

    def test_eof_line_and_error_behavior(self):
        main = r"""
int main(void) {
    char s[40];
    int count = 0;
    while (try_read_line(s)) {
        print_text(s, "|");
        ++count;
    }
    print_int(count, "\n");
    return 0;
}"""
        self.assertEqual(self.run_c(main, "A\n\nB",
                                    {"io_line", "print_scalar"}), "A||B|3\n")
        bad = r'int main(void){int x=read_int();print_int(x,"\n");}'
        for data in ("12abc\n", "2147483648\n", "-2147483649\n", ""):
            self.run_c(bad, data, {"io_int", "print_scalar"}, expect_failure=True)
        overlong = r'int main(void){char s[4];read_word(s);}'
        self.run_c(overlong, "abcdefgh", {"io_word"}, expect_failure=True)
        line_long = r'int main(void){char s[3];read_line(s);}'
        self.run_c(line_long, "hello\n", {"io_line"}, expect_failure=True)


if __name__ == "__main__":
    if not shutil.which(CC):
        raise SystemExit("C compiler not found: " + CC)
    unittest.main()
