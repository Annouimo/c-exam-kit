# Lightweight IO preset for C99

**Status: feature branch; review before merging.** This is an IO-only preset for the
[standalone CLI manifest v1](https://github.com/Annouimo/better-accoding-for-buaaers/tree/feat/standalone-cli-foundation/scaffold).
No algorithms from the older scaffold or C Toolbox are migrated. Legacy
c-exam-kit files, build scripts, documentation and releases remain unchanged.

## Use with the new CLI

From a checkout of better-accoding-for-buaaers (standalone CLI feature branch):

~~~sh
bun src/standalone/cli.ts scaffold init ./A --preset /path/to/c-exam-kit/scaffold/presets/exam-io-c99/manifest.json
bun src/standalone/cli.ts scaffold prepare ./A/main.c
~~~

Initialization snapshots the manifest and source components in each problem
folder, writes a complete exam.h and a blank main.c. Prepare selects symbols
found in user code and their declared dependencies, producing a standalone
.accoding/submit.c without changing the original code. No runtime library
or Python is necessary for the generated C program.

### Numeric input and EOF

~~~c
#include "exam.h"
int main(void) {
    int a, b;
    while (try_read_int(&a)) {
        b = read_int();
        print_int(a + b, "\n");
    }
    return 0;
}
~~~

try_read_* returns false **only at clean EOF before a new value**. Bad numeric
text (such as 12abc), truncated records, out-of-range values and stream errors
terminate with a nonzero exit status and an error printed to stderr. Ordinary
read_* functions also fail on unexpected EOF. Numeric tokens are parsed
without allocating memory for every value.

### Arrays

~~~c
int n = read_int();
int a[1000];
read_ints(a, n);
print_ints(a, n, " ", "\n");
~~~

read_ints, read_lls and read_doubles infer the real array's element capacity,
reject negative counts and counts exceeding capacity, and read exactly n
values. Array printing takes the number of **used elements** explicitly.

### Words and lines

~~~c
char name[80], line[1024];
read_word(name); /* next non-whitespace word */
read_line(line); /* next complete line after a partial token read */
read_rest(line); /* current line remainder, including leading whitespace */
~~~

- read_word skips leading whitespace.
- read_line, after a token or character has been read from the current line,
  discards the remaining contents (including the line terminator), then reads
  the next **complete** line. Otherwise, it reads the current complete line.
- read_rest always reads from the current position through the end of that line.
- read_line and read_rest preserve genuinely blank lines and leading/trailing
  spaces. Line terminators LF, CRLF and CR are removed.
- try_read_line(s) returns true for a real empty line; false only when no
  line remains at all.
- skip_line() discards the rest of the current line.

## Public API

| Category | API |
| --- | --- |
| Single values | read_int, read_ll, read_double, read_char, read_char_raw |
| EOF-aware | try_read_int, try_read_ll, try_read_double, try_read_char, try_read_char_raw |
| Text | read_word, try_read_word, read_line, try_read_line, read_rest, try_read_rest, skip_line |
| Array input | read_ints, read_lls, read_doubles |
| Scalar output | print_int(x,end), print_ll(x,end), print_double(x,digits,end), print_char(x,end), print_text(s,end) |
| Array output | print_ints(a,n,sep,end), print_lls(a,n,sep,end), print_doubles(a,n,digits,sep,end) |

Word, line and array input convenience names are **lowercase function-like
macros**. They automatically forward sizeof(array) or element capacity to a
checked helper. They are safe to use only on a **real array in the current
scope**, not a pointer, dynamically allocated memory, or an array parameter.
Explicit-capacity functions such as read_word_into(s,capacity),
read_line_into(s,capacity), and read_ints_into(a,n,capacity) are available
when necessary.

All input helpers use stdin and shared line-state tracking. Do not mix them
with scanf, fgets, getchar or other consumers of stdin. printf is compatible
with the output helpers. The preset uses only C99 and standard library
headers. print_double uses fixed-point formatting (0–100 digits), retaining
standard C negative-zero output semantics.

## Test and integration notes

~~~sh
python3 tests/test_lightweight_io.py
~~~

The included regression suite compiles C99 programs with all modules and
with selected dependency subsets, and tests EOF, blank lines, CRLF, mixed
text/numbers, array capacities, output formatting and malformed input.
The manifest follows the external CLI's schema v1; **the CLI implementation
remains in better-accoding-for-buaaers**, not in this repository. Run the
standalone CLI's own tests there as part of final integration review.

Known limits: no dynamic String/Vector/Matrix, no automatic growing buffers,
no other algorithm templates, no per-token allocation, numeric tokens longer
than 127 bytes rejected, and no generalized multi-stream abstraction.
