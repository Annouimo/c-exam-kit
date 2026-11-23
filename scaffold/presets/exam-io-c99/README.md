# C99 lightweight scaffold preset

**Review branch only**. Designed for the [standalone CLI manifest v1](https://github.com/Annouimo/better-accoding-for-buaaers/tree/feat/standalone-cli-foundation/scaffold). The directory name `exam-io-c99` originated in the IO implementation stage; it now contains independent algorithm components as well. No old templates or dynamic Vector/String/Matrix implementation are copied.

## Workflow

~~~sh
# From better-accoding-for-buaaers, standalone CLI feature branch:
bun src/standalone/cli.ts scaffold init ./A --preset /path/to/c-exam-kit/scaffold/presets/exam-io-c99/manifest.json
bun src/standalone/cli.ts scaffold prepare ./A/main.c
~~~

Initialization creates `main.c`, a complete local `exam.h` and an immutable per-problem preset snapshot in `.accoding/scaffold/`. Prepare uses referenced public symbols and component dependencies to emit `.accoding/submit.c`, then checks that it compiles. The compiler needs C99 and `-lm`; no installed runtime library is required.

## Design conventions

- **Plain C values and arrays**: array functions use an `int` length and 0-based indices. Composite values use small structs. Queries do not modify arguments; mutation is in place.
- **Naming**: verb + plural type (`sort_ints`, `sum_lls`), `str_...` for byte strings, type + verb (`bigint_add`, `date_add_days`) for composites. No backwards-compatibility aliases.
- **Output buffers**: lowercase function-like macros `read_ints(a,n)`, `fill_ints(a,n,v)`, `values_ints(a,...)`, `range_ints(a,first,stop,step)`, `read_line(s)`, `str_insert(s,pos,text)` infer capacity using `sizeof(array)`. **Pass real arrays declared in the current scope**, never pointers, malloc buffers, or function array parameters. Use the corresponding `*_into(..., capacity)` function for pointers.
- **Error behavior**: invalid parameters, overflow and capacity failure terminate nonzero with a message on stderr. `try_read_*` returns false only at clean EOF.
- **Ranges**: where used, ranges are half-open `[begin,end)`; binary insertion positions are `[0,n]`.
- **No dynamic container API**: a few algorithms may allocate temporary working memory internally; users do not manage it.

## IO

~~~c
#include "exam.h"
int main(void) {
    int a,b;
    while (try_read_int(&a)) {
        b=read_int();
        print_int(a+b,"\n");
    }
    return 0;
}
~~~

Single values: `read_int`, `read_ll`, `read_double`, `read_char` (skips whitespace), `read_char_raw`. Each has a `try_read_*` variant taking a pointer and returning bool.

Text: `read_word`, `read_line`, `read_rest`, `skip_line`, with `try_read_*` variants. `read_line` reads the next **whole** line after a partial token read (discarding any remaining part of the token's line), but preserves real blank lines. `read_rest` always reads the remainder of the current line, including leading spaces. CRLF/LF/CR handled.

Arrays: `read_ints`, `read_lls`, `read_doubles`; `print_ints(a,n,sep,end)`, `print_lls`, `print_doubles(a,n,digits,sep,end)`. Scalar outputs `print_int(x,end)`, `print_ll`, `print_double(x,digits,end)`, `print_char`, `print_text`. Standard `printf` is compatible. Do not interleave input helpers with `scanf` or `fgets` on stdin.

## Arrays

~~~c
int a[100];
int n = values_ints(a, 4, 1, 4, 2); /* n = 4 */
sort_ints(a,n);
int pos = lower_bound_ints(a,n,4); /* first >= 4 */
long long total = sum_ints(a,n);
~~~

- int construction/mutation: `fill_ints`, `copy_ints`, `values_ints`, `range_ints`, `reverse_ints`, `swap_ints`, `sort_ints`, `sort_ints_desc`, `bubble_sort_ints`, `unique_ints`, `unique_sorted_ints`.
- int queries: `linear_search_ints`, `find_ints`, `binary_search_ints`, `lower_bound_ints`, `upper_bound_ints`, `count_ints`; `sum_ints`, `mean_ints`, `variance_ints`, `stddev_ints`, `median_ints`, `min_ints`, `max_ints`, `argmin_ints`, `argmax_ints`.
- sequence: `longest_equal_run_ints`, `longest_plus_one_run_ints`, `max_subarray_sum_ints`, `max_forward_diff_ints`.
- interval: `build_prefix_ints(a,n,prefix)`, `prefix_range_sum(prefix,begin,end)`; `build_diff_ints(a,n,diff)`, `diff_range_add(diff,n,begin,end,delta)`, `diff_to_lls(diff,n,out)`. Prefix/difference buffers require **at least n+1** `long long` elements; end is exclusive.
- frequencies: `frequency_ints(a,n,freq,max_value)` fills counters for **nonnegative** values `0..max_value`.
- long long: `fill_lls`, `copy_lls`, `sort_lls`, `reverse_lls`, `find_lls`, `lower_bound_lls`, `upper_bound_lls`, `sum_lls`, `min_lls`, `max_lls`.
- double: `fill_doubles`, `copy_doubles`, `sort_doubles`, `reverse_doubles`, `find_doubles`, `lower_bound_doubles`, `upper_bound_doubles`, `sum_doubles`, `mean_doubles`, `min_doubles`, `max_doubles`. Sorting rejects NaN and infinities; finding compares using C's exact `==`.

`find_*` returns the first matching 0-based index or -1. Binary search requires **ascending** input and returns the first match. `lower_bound`/`upper_bound` return insertion positions. `unique_ints` preserves the first occurrence order but takes O(n²) time. `median_ints` does not alter input and may allocate a temporary array.

## Strings

Byte-oriented C strings (not Unicode code points): `str_reverse`, `str_lower`, `str_upper`, `str_trim`, `str_is_palindrome`, `str_count_char`, `str_count_substr(s,needle,overlapping)`, `str_longest_char_run`, `str_insert`, `str_erase`, `str_replace_char`, `str_remove_char`, `str_replace_all`, `str_sort_chars`, `str_unique_chars`.

`str_insert` and `str_replace_all` change length, so they use auto-capacity macros. `str_replace_all` replaces non-overlapping matches. Their text/needle/replacement arguments must **not point into the destination string buffer**. For standard operations still use `strlen`, `strcmp`, `strstr` and `snprintf`.

## Math, bits, dates, complex

- Math: `gcd`/`lcm` (nonnegative unsigned results), `factorial_basic` (0..20), `int_pow` (signed integer), `mod_pow`, `is_prime`, `sieve_primes(flags,limit)`, `poly_eval_lls(coeff,count,x)` (constant coefficient first), `nearly_equal`, `round_digits`, `bisect_root(f,left,right,tolerance,iterations)`.
- Base and digits: `base_to_int(text,base)`, `int_to_base(value,base,out)` (bases 2..36), `int_digit_count`, `int_digit_sum`, `int_digit_at` (least-significant position 0), `int_to_digits` (most-significant first), `digits_to_int`, `int_reverse_digits`.
- Bits: `bit_get/set/clear/toggle`, `bit_count`, `is_power_of_two`, `bit_lowest_set_index`, `bit_highest_set_index`, `bit_rotate_left` on unsigned 64-bit values; bit indices 0..63.
- Complex: `Complex` is a two-double **value** type with `complex_new/add/sub/mul/div/abs/print`. No dynamic memory or special expression parser.
- Dates: `Date` with `date_new/valid/is_leap/days_in_month/day_of_year/to_serial/from_serial/add_days/diff/compare/print` (Gregorian years 1..9999).
- Time: `Time` with `time_new/valid/to_seconds/from_seconds/diff/add_seconds/next_second/next_minute/print`, wrapping within 24 h; `diff(a,b)` means `b-a`.

## BigInt and enumeration

~~~c
BigInt a,b,sum;
bigint_from_str(&a,"12345678901234567890");
bigint_from_str(&b,"-15");
bigint_add(&sum,&a,&b);
bigint_print(&sum,"\n");
~~~

Fixed-size decimal `BigInt`, signed `-1/0/+1`, **no owned pointers or free operation**. Default max length: `EXAM_BIGINT_CAP=2048` decimal digits, overridable before the header is included. API: `bigint_zero`, `bigint_from_str`, `bigint_from_ll`, `bigint_to_str`, `bigint_print`, `bigint_compare`, `bigint_add`, `bigint_sub`, `bigint_mul`, `bigint_divmod(&q,&r,&a,&b)`, `bigint_mul_small`, `bigint_div_small`, `bigint_factorial`, `read_bigint`, `try_read_bigint`. Division truncates toward zero; remainder has dividend's sign. Output arguments may alias input operands except the two distinct outputs of `bigint_divmod`.

~~~c
Combination c = combination_new(5,3);
while(combination_next(&c)) {
    /* indices in c.indices[0..c.r-1] */
}
Permutation p = permutation_new(5,3);
while(permutation_next(&p)) {
    /* indices in p.indices[0..p.r-1] */
}
~~~

Both enumerate ordered 0-based indices, with no allocation; `Combination` is unordered choice, `Permutation` is ordered choice. A zero-length selection produces exactly one empty result. Default maximum n: `EXAM_ENUM_CAP=64`, overridable before including the header.

## Not included / next design discussion

**Intentionally excluded for now**: `sort_by`, `argsort_ints`, multi-key record sorting, original-index search, stable generic sorting, string-list/split-join storage interfaces, generic predicate-based integer answer bisection, dynamic container abstractions, ST/RMQ, DFS/grid paths, generalized matrices, heap/deque/map/set and quadratic-equation special outputs.

**Testing:** `python3 tests/test_lightweight_io.py` and `python3 tests/test_lightweight_algorithms.py` compile and run C99 with complete and dependency-selected headers. Windows/MinGW, macOS and Linux are CI targets. Final integration should also use the upstream standalone CLI to initialize and assemble an example.
