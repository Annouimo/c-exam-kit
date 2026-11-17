# SPDX-License-Identifier: MIT
import importlib.util
import os
from pathlib import Path
import random
import shlex
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
subprocess.run([sys.executable, str(ROOT / "build.py")], check=True)
STANDALONE = ROOT / "dist/c_toolbox_v0.1.0.py"
spec = importlib.util.spec_from_file_location("toolbox", STANDALONE)
tb = importlib.util.module_from_spec(spec)
spec.loader.exec_module(tb)

class ToolboxTests(unittest.TestCase):
    def run_c(self, code, input_text="", flags=()):
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            source = folder / "answer.c"
            binary = folder / ("answer.exe" if os.name == "nt" else "answer")
            source.write_text(code, encoding="utf-8")
            cmd = shlex.split(os.environ.get("CC", "cc"))
            cmd += ["-std=c99", "-Wall", "-Wextra", "-Werror", "-Wno-unused-function"]
            cmd += shlex.split(os.environ.get("TOOLBOX_CFLAGS", ""))
            cmd += list(flags) + [str(source), "-o", str(binary)]
            compiled = subprocess.run(cmd, capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            completed = subprocess.run([str(binary)], input=input_text, capture_output=True, text=True)
            self.assertEqual(completed.returncode, 0, completed.stderr)
            return completed.stdout

    def test_standalone_without_sources(self):
        with tempfile.TemporaryDirectory() as directory:
            standalone = Path(directory) / "toolbox.py"
            standalone.write_bytes(STANDALONE.read_bytes())
            output = Path(directory) / "answer.c"
            subprocess.run([sys.executable, str(standalone), "--modules", "counter",
                            "-o", str(output)], cwd=directory, check=True)
            code = output.read_text(encoding="utf-8")
            self.assertIn("typedef struct", code)
            self.assertNotIn("StringList", code)
            self.run_c(code + "\nint main(void) { return 0; }\n")
        subprocess.run([sys.executable, str(ROOT / "build.py"), "--check"], check=True)

    def test_invalid_configuration(self):
        for modules, specs in [(["missing"], []), (["map"], ["IntVector=int"]),
                                (["vector"], ["String=int"]),
                                (["vector"], ["Bad=int;"]),
                                (["vector"], ["X=int", "X=double"])]:
            with self.assertRaises(ValueError):
                tb.generate(modules, specs)

    def test_each_module_compiles(self):
        for module in ("string", "vector", "map", "set", "counter"):
            with self.subTest(module=module):
                self.run_c(tb.generate([module], main="int main(void) { return 0; }"))

    def test_string_boundaries_and_ownership(self):
        self.run_c(tb.generate(["string"], main=r"""
#include <assert.h>
int main(void) {
    String s = {0};
    assert(!strcmp(string_cstr(&s), ""));
    assert(string_erase(&s, 0, 0));
    string_append(&s, "abc");
    assert(string_insert(&s, 1, s.data));
    assert(!strcmp(s.data, "aabcbc"));
    string_append(&s, s.data + 3);
    assert(!strcmp(s.data, "aabcbccbc"));
    assert(!string_insert(&s, s.len + 1, "x"));
    assert(!string_erase(&s, 0, SIZE_MAX));
    String copy = string_clone(&s);
    string_free(&s);
    assert(!strcmp(copy.data, "aabcbccbc"));
    string_replace_all(&copy, "bc", "_");
    assert(!strcmp(copy.data, "aa__c_"));
    string_replace_all(&copy, "", "ignored");
    String alias = string_from("abcabc");
    string_replace_all(&alias, alias.data + 3, alias.data + 4);
    assert(!strcmp(alias.data, "bcbc"));
    string_free(&alias);
    String input = string_from(",a,,b,");
    StringList parts = string_split(&input, ",");
    string_free(&input);
    assert(parts.len == 5);
    String joined = string_join(&parts, "|");
    assert(!strcmp(joined.data, "|a||b|"));
    assert(string_find(&joined, "missing", 0) == SIZE_MAX);
    assert(string_find(&joined, "", joined.len) == joined.len);
    assert(string_find(&joined, "", joined.len + 1) == SIZE_MAX);
    string_free(&joined); string_list_free(&parts); string_free(&copy);
    string_free(&copy);
    String line = {0};
    assert(string_readline(&line, stdin) && !strcmp(line.data, ""));
    assert(string_readline(&line, stdin) && !strcmp(line.data, "hello"));
    assert(string_readline(&line, stdin) && !strcmp(line.data, "tail"));
    assert(!string_readline(&line, stdin));
    string_free(&line);
    return 0;
}
"""), "\nhello\r\ntail")

    def test_string_random_edits(self):
        rng = random.Random(26)
        expected = ""
        operations = []
        for _ in range(250):
            if not expected or rng.randrange(2):
                pos = rng.randrange(len(expected) + 1)
                text = "".join(rng.choices("abc", k=rng.randrange(8)))
                expected = expected[:pos] + text + expected[pos:]
                operations.append('assert(string_insert(&s, %d, "%s"));' % (pos, text))
            else:
                pos = rng.randrange(len(expected) + 1)
                count = rng.randrange(len(expected) - pos + 1)
                expected = expected[:pos] + expected[pos + count:]
                operations.append("assert(string_erase(&s, %d, %d));" % (pos, count))
            operations.append('assert(s.len == %d && !strcmp(string_cstr(&s), "%s"));' %
                              (len(expected), expected))
        self.run_c(tb.generate(["string"], main="#include <assert.h>\nint main(void) {"
                              "String s = {0};\n" + "\n".join(operations) +
                              "\nstring_free(&s); return 0; }"))

    def test_vector_struct_and_growth(self):
        self.run_c(tb.generate(["vector"],
            ["IntVector=int", "StudentVector=Student"],
            "typedef struct { int id; double score; } Student;", r"""
#include <assert.h>
static int compare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
int main(void) {
    IntVector v = IntVector_new();
    int out;
    assert(!IntVector_pop(&v, &out));
    assert(IntVector_erase(&v, 0, 0));
    for (int i = 999; i >= 0; --i) IntVector_push(&v, i);
    IntVector_sort(&v, compare);
    for (int i = 0; i < 1000; ++i) assert(v.data[i] == i);
    assert(IntVector_insert(&v, 0, -1));
    assert(IntVector_erase(&v, 1, 1000));
    assert(v.len == 1 && v.data[0] == -1);
    assert(!IntVector_insert(&v, 2, 0));
    assert(!IntVector_erase(&v, 0, SIZE_MAX));
    IntVector copy = IntVector_clone(&v);
    IntVector_free(&v);
    assert(IntVector_pop(&copy, &out) && out == -1);
    IntVector_clear(&copy); IntVector_free(&copy);
    StudentVector students = StudentVector_new();
    StudentVector_push(&students, (Student){42, 98.5});
    assert(students.data[0].score == 98.5);
    StudentVector_free(&students);
    return 0;
}
"""))

    def test_map_random_against_python(self):
        rng = random.Random(99)
        expected = {}
        operations = []
        for _ in range(2000):
            key = "key%d" % rng.randrange(200)
            if rng.randrange(3):
                value = rng.randrange(-1000, 1000)
                expected[key] = value
                operations.append('(void)map_set(&m, "%s", %d);' % (key, value))
            else:
                existed = key in expected
                expected.pop(key, None)
                operations.append('assert(map_remove(&m, "%s") == %s);' %
                                  (key, "true" if existed else "false"))
            operations.append("assert(m.len == %d);" % len(expected))
            if key in expected:
                operations.append('assert(map_get(&m, "%s", &value) && value == %d);' %
                                  (key, expected[key]))
            else:
                operations.append('assert(!map_contains(&m, "%s"));' % key)
        main = """#include <assert.h>
int main(void) { Map m = map_new(); long long value;
""" + "\n".join(operations) + """
Map copy = map_clone(&m);
map_free(&m);
size_t cursor = 0, count = 0;
const char *key;
while (map_next(&copy, &cursor, &key, &value)) {
    assert(map_contains(&copy, key)); ++count;
}
assert(count == copy.len);
map_free(&copy); map_free(&copy);
return 0; }
"""
        self.run_c(tb.generate(["map"], main=main))

    def test_key_copy_set_counter_overflow(self):
        self.run_c(tb.generate(["set", "counter"], main=r"""
#include <assert.h>
int main(void) {
    Set s = set_new();
    char key[] = "hello";
    assert(set_add(&s, key));
    key[0] = 'x';
    assert(set_contains(&s, "hello"));
    assert(!set_add(&s, "hello") && set_size(&s) == 1);
    Set copy = set_clone(&s);
    set_free(&s);
    size_t cursor = 0;
    const char *item;
    assert(set_next(&copy, &cursor, &item) && !strcmp(item, "hello"));
    assert(set_remove(&copy, "hello")); set_free(&copy);
    Counter c = counter_new();
    assert(counter_get(&c, "x") == 0);
    assert(counter_add(&c, "x", LLONG_MAX));
    assert(!counter_add(&c, "x", 1) && counter_get(&c, "x") == LLONG_MAX);
    assert(counter_add(&c, "y", LLONG_MIN));
    assert(!counter_add(&c, "y", -1) && counter_get(&c, "y") == LLONG_MIN);
    assert(counter_add(&c, "z", -2));
    Counter other = counter_clone(&c);
    counter_free(&c);
    assert(counter_get(&other, "z") == -2);
    cursor = 0; long long value;
    assert(counter_next(&other, &cursor, &item, &value));
    assert(counter_remove(&other, "z"));
    counter_free(&other);
    return 0;
}
"""))

    def test_examples(self):
        main = (ROOT / "examples/word_count.c").read_text(encoding="utf-8")
        output = self.run_c(tb.generate(["string", "counter"], main=main), "a b a\n")
        self.assertEqual(dict(line.split() for line in output.splitlines()), {"a": "2", "b": "1"})
        main = (ROOT / "examples/student_sort.c").read_text(encoding="utf-8")
        preamble = (ROOT / "examples/student_types.c").read_text(encoding="utf-8")
        self.assertEqual(self.run_c(tb.generate(["vector"], ["StudentVector=Student"],
                                                preamble, main)), "2 95\n1 80\n")

if __name__ == "__main__":
    unittest.main(verbosity=2)
