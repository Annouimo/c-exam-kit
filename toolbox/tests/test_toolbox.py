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
STANDALONE = ROOT / "dist/c_toolbox_v0.2.0.py"
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
        for module in ("string", "vector", "map", "set", "counter", "deque", "heap"):
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

    def test_string_common_operations(self):
        self.run_c(tb.generate(["string"], main=r"""
#include <assert.h>
int main(void) {
    String s = string_new();
    assert(!string_pop(&s, NULL));
    string_trim(&s);
    string_assign(&s, " \tAbC \r\n");
    string_trim(&s);
    assert(!strcmp(s.data, "AbC"));
    string_lower(&s); assert(!strcmp(s.data, "abc"));
    string_upper(&s); assert(!strcmp(s.data, "ABC"));
    assert(string_starts_with(&s, "AB") && string_ends_with(&s, "BC"));
    assert(string_contains(&s, "") && !string_contains(&s, "D"));
    assert(!string_starts_with(&s, "ABCD") && !string_ends_with(&s, "XABC"));
    string_assign(&s, s.data + 1); assert(!strcmp(s.data, "BC"));
    string_push(&s, '!'); char ch;
    assert(string_pop(&s, &ch) && ch == '!');
    string_assign(&s, "aabb");
    assert(string_count(&s, "ab") == 1 && string_count(&s, "") == 0);
    string_remove_all(&s, "ab"); assert(!strcmp(s.data, "ab"));
    string_assign(&s, "aabb");
    assert(string_remove_repeated(&s, "ab") == 2 && s.len == 0);
    assert(string_remove_repeated(&s, "") == 0);
    string_assign(&s, "abab");
    assert(string_remove_repeated(&s, s.data + 2) == 2 && s.len == 0);
    string_assign(&s, " \t\r\n"); string_trim(&s); assert(!s.len);
    string_clear(&s); assert(!s.len && !strcmp(s.data, ""));
    assert(string_read_token(&s, stdin) && s.len == 10000);
    assert(string_read_token(&s, stdin) && !strcmp(s.data, "last"));
    assert(!string_read_token(&s, stdin) && !s.len);
    string_free(&s); return 0;
}"""), "\t" + "a" * 10000 + " \nlast")

    def test_vector_extend_resize(self):
        self.run_c(tb.generate(["vector"], main=r"""
#include <assert.h>
int main(void) {
    IntVector v = IntVector_new();
    assert(!IntVector_front(&v, NULL) && !IntVector_back(&v, NULL));
    IntVector_extend(&v, NULL, 0);
    for (int i = 0; i < 8; ++i) IntVector_push(&v, i);
    IntVector_extend(&v, v.data + 2, 6);
    assert(v.len == 14 && v.data[8] == 2 && v.data[13] == 7);
    IntVector_reverse(&v); int x;
    assert(IntVector_front(&v, &x) && x == 7);
    assert(IntVector_back(&v, &x) && x == 0);
    assert(IntVector_set(&v, 0, -1));
    assert(IntVector_get(&v, 0, &x) && x == -1);
    assert(!IntVector_get(&v, v.len, &x) && !IntVector_set(&v, v.len, 0));
    IntVector_resize(&v, 100, 42); assert(v.len == 100 && v.data[99] == 42);
    IntVector_resize(&v, 0, 0); assert(!v.len);
    IntVector_reverse(&v); IntVector_free(&v); return 0;
}"""))

    def test_container_type_dependencies(self):
        self.run_c(tb.generate(["vector", "deque"],
            ["Words=String", "Nested=WordQueue"],
            deques=["WordQueue=String"], main=r"""
#include <assert.h>
int main(void) {
    Words words = Words_new();
    Words_push(&words, string_from("hello"));
    assert(!strcmp(words.data[0].data, "hello"));
    string_free(&words.data[0]); Words_free(&words);
    Nested nested = Nested_new(); Nested_free(&nested);
    WordQueue q = WordQueue_new();
    WordQueue_push_back(&q, string_from("x"));
    String item;
    assert(WordQueue_pop_front(&q, &item));
    string_free(&item); WordQueue_free(&q); return 0;
}"""))
        with self.assertRaises(ValueError):
            tb.generate(["vector", "deque"], ["Same=int"], deques=["Same=int"])
        with self.assertRaises(ValueError):
            tb.generate(["vector"], ["A=B", "B=A"])

    def test_deque_random_against_python(self):
        from collections import deque
        rng = random.Random(2026)
        expected = deque()
        operations = []
        for _ in range(1500):
            action = rng.randrange(4)
            if action < 2 or not expected:
                value = rng.randrange(-99, 100)
                if action == 0:
                    expected.appendleft(value)
                    operations.append("IntDeque_push_front(&q, %d);" % value)
                else:
                    expected.append(value)
                    operations.append("IntDeque_push_back(&q, %d);" % value)
            else:
                if action == 2:
                    value = expected.popleft(); name = "pop_front"
                else:
                    value = expected.pop(); name = "pop_back"
                operations.append("assert(IntDeque_%s(&q, &value) && value == %d);" % (name, value))
            operations.append("assert(q.len == %d);" % len(expected))
        operations += ["IntDeque copy = IntDeque_clone(&q); IntDeque_free(&q);"]
        for value in expected:
            operations.append("assert(IntDeque_pop_front(&copy, &value) && value == %d);" % value)
        self.run_c(tb.generate(["deque"], main="#include <assert.h>\nint main(void) {"
                   "IntDeque q = IntDeque_new(); int value;\n" + "\n".join(operations) +
                   "\nassert(!IntDeque_pop_back(&copy, NULL));"
                   "IntDeque_clear(&copy); IntDeque_free(&copy); return 0; }"))

    def test_heap_random_against_python(self):
        import heapq
        rng = random.Random(55)
        expected = []
        operations = []
        for _ in range(1000):
            if not expected or rng.randrange(3):
                value = rng.randrange(-100, 100)
                heapq.heappush(expected, value)
                operations.append("IntHeap_push(&h, %d);" % value)
            else:
                operations.append("assert(IntHeap_pop(&h, &value) && value == %d);" % heapq.heappop(expected))
            operations.append("assert(h.len == %d);" % len(expected))
            if expected:
                operations.append("assert(IntHeap_peek(&h, &value) && value == %d);" % expected[0])
        for value in sorted(expected):
            operations.append("assert(IntHeap_pop(&h, &value) && value == %d);" % value)
        self.run_c(tb.generate(["heap"], main=r"""
#include <assert.h>
static int compare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
static int reverse(const void *a, const void *b) { return -compare(a, b); }
int main(void) { IntHeap h = IntHeap_new(compare); int value;
""" + "\n".join(operations) + r"""
assert(!IntHeap_pop(&h, NULL)); IntHeap_free(&h);
for (int n = 0; n < 50; ++n) {
    int items[50];
    for (int i = 0; i < n; ++i) items[i] = (i * 13) % 17;
    h = IntHeap_from(items, (size_t)n, reverse);
    IntHeap copy = IntHeap_clone(&h); IntHeap_free(&h);
    int last = INT_MAX;
    while (IntHeap_pop(&copy, &value)) { assert(value <= last); last = value; }
    IntHeap_clear(&copy); IntHeap_free(&copy);
}
return 0; }"""))

    def test_heap_custom_struct(self):
        self.run_c(tb.generate(["heap"], preamble="typedef struct { int id, priority; } Task;",
            heaps=["TaskHeap=Task"], main=r"""
#include <assert.h>
static int compare(const void *a, const void *b) {
    const Task *x = a, *y = b;
    return (x->priority > y->priority) - (x->priority < y->priority);
}
int main(void) {
    TaskHeap h = TaskHeap_new(compare);
    TaskHeap_push(&h, (Task){1, 100}); TaskHeap_push(&h, (Task){2, 10});
    Task task;
    assert(TaskHeap_pop(&h, &task) && task.id == 2);
    TaskHeap_free(&h); return 0;
}"""))

    def test_set_algebra_and_counter(self):
        self.run_c(tb.generate(["set", "counter"], main=r"""
#include <assert.h>
int main(void) {
    Set a = set_new(), b = set_new();
    set_add(&a, "a"); set_add(&a, "b"); set_add(&b, "b"); set_add(&b, "c");
    Set u = set_union(&a, &b), i = set_intersection(&a, &b), d = set_difference(&a, &b);
    assert(set_size(&u) == 3 && set_size(&i) == 1 && set_size(&d) == 1);
    assert(set_contains(&i, "b") && set_contains(&d, "a"));
    assert(set_is_subset(&a, &u) && !set_is_subset(&a, &b));
    set_clear(&a); assert(!set_size(&a)); set_add(&a, "fresh");
    set_free(&a); set_free(&b); set_free(&u); set_free(&i); set_free(&d);
    Map m = map_new(); assert(map_get_or(&m, "x", 7) == 7);
    map_set(&m, "x", 0); assert(map_get_or(&m, "x", 7) == 0 && map_size(&m) == 1);
    size_t cap = m.cap; map_clear(&m); assert(map_size(&m) == 0 && m.cap == cap);
    map_set(&m, "y", 1); map_free(&m);
    Counter c = counter_new(); const char *key; long long count;
    assert(!counter_most_common(&c, &key, &count));
    counter_add(&c, "b", -1); counter_add(&c, "a", -1);
    assert(counter_most_common(&c, &key, &count) && !strcmp(key, "a") && count == -1);
    assert(counter_size(&c) == 2); counter_clear(&c); assert(!counter_size(&c));
    counter_free(&c); return 0;
}"""))

    def test_new_examples(self):
        main = (ROOT / "examples/remove_words.c").read_text(encoding="utf-8")
        self.assertEqual(self.run_c(tb.generate(["string"], main=main), "aabb hello\n2\nab hello\n"), " \n")
        main = (ROOT / "examples/priority_queue.c").read_text(encoding="utf-8")
        self.assertEqual(self.run_c(tb.generate(["heap"], main=main)), "10\n20\n30\n")


if __name__ == "__main__":
    unittest.main(verbosity=2)

