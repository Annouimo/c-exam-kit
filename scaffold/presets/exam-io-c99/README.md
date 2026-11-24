# C99 lightweight scaffold preset

The C99 preset for [Accoding Companion](https://github.com/Annouimo/accoding-companion), built and distributed as `@annouimo/c-exam-kit-preset`.

## Workflow

中文函数速查由 `components/*.h` 的声明注释生成，随 npm preset 一起发布。
文件顶部用 `@category` 标记分类。公开声明用 `@public`，函数、宏和类型的直接依赖统一写在相邻的 `@requires` 行上。
用 `@title` 和可选的 `@id`、`@members` 创建条目；`@variant int fn1 fn2`（类型键 int / ll / double）声明类型变体，`@example` 后写多行示例，支持 {{type}}、{{fn1}} 等简单占位符，`@example double` 和 `@note double` 覆盖示例或追加说明。目录维护在 `reference-layout.json`。迁移期间输入输出分类严格检查覆盖率，其他分类仍保留旧条目。
本地运行 `node npm/accoding-preset/scripts/build-preset.mjs` 可生成清单和 TUI 数据。

Follow the [Companion setup and usage guide](https://github.com/Annouimo/accoding-companion#readme). Companion keeps a shared preset snapshot and complete IDE header in `.accoding/presets/` for each version used by a workspace. When testing or submitting, Companion assembles the referenced components and their dependencies. The compiler needs C99 and `-lm`.

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
- long long: `fill_lls`, `copy_lls`, `values_lls`, `range_lls`, `sort_lls`, `sort_lls_desc`, `reverse_lls`, `find_lls`, `count_lls`, `binary_search_lls`, `lower_bound_lls`, `upper_bound_lls`, `sum_lls`, `mean_lls`, `variance_lls`, `stddev_lls`, `median_lls`, `min_lls`, `max_lls`, `argmin_lls`, `argmax_lls`, `unique_lls`, `unique_sorted_lls`.
- double: `fill_doubles`, `copy_doubles`, `values_doubles`, `sort_doubles`, `sort_doubles_desc`, `reverse_doubles`, `find_doubles`, `count_doubles`, `binary_search_doubles`, `lower_bound_doubles`, `upper_bound_doubles`, `sum_doubles`, `mean_doubles`, `variance_doubles`, `stddev_doubles`, `median_doubles`, `min_doubles`, `max_doubles`, `argmin_doubles`, `argmax_doubles`, `unique_doubles`, `unique_sorted_doubles`. Floating equality is **exact**; sorting, statistics and deduplication reject NaN and infinity. `range_doubles` is not provided.

`find_*` returns the first matching 0-based index or -1. Binary search requires **ascending** input and returns the first match. `lower_bound`/`upper_bound` return insertion positions. `unique_ints` preserves the first occurrence order but takes O(n²) time. `median_ints` does not alter input and may allocate a temporary array.

## Record sorting and search, indirect sorting and integer answer search

The record type is fixed to `Record` for simplicity. **Define a named struct
tag**, not an anonymous `typedef struct { ... } Record;`. Include `exam.h`
first, then define the fields and define a comparator with signature
`int compare(const Record *, const Record *)` (static is allowed). The comparator returns negative,
zero or positive for less-than, equal or greater-than ordering. Pass the typed comparator explicitly to the C99 heapsort:

~~~c
#include "exam.h"

struct Record {
    int score, penalty;
};

int record_cmp(const Record *a, const Record *b) {
    if (a->score != b->score)
        return (a->score < b->score) - (a->score > b->score);
    return (a->penalty > b->penalty) - (a->penalty < b->penalty);
}

int main(void) {
    Record a[100] = {{95,10},{98,20},{95,5}};
    sort_records(a, 3, record_cmp);
    Record key = {.score = 95, .penalty = 5};
    int index = binary_search_records_by(a, 3, &key, record_cmp);
    printf("%d\n", index);  /* 1 */
    return 0;
}
~~~

`sort_records` sorts the array in place, **not stably**. Exactly one `Record` type is supported; multiple comparison rules are allowed. Use standard
`qsort` directly for more specialized multiple-type situations.

`find_records_by(a, n, &key, compare)` scans the array in its current order.
`binary_search_records_by(a, n, &key, compare)` requires the array to be
ordered according to `compare`. Both return the first matching 0-based index
or `-1`. The comparator has signature `int compare(const Record *, const Record *)`
and receives the array element first and the search key second. Use
`record_cmp` for the same ordering as `sort_records`, or a separate comparator
for a different search key. Initialize every field the comparator reads.
Searching alone only requires the comparator passed at the call site.

`argsort_ints(a,n,order)` does **not modify a**. Instead, it puts the original
zero-based indexes in sorted order into the user-owned `order[]`. Equal
values are ordered by their original indexes, deterministically:

~~~c
int a[] = {8,3,8,2};
int order[4];
argsort_ints(a, 4, order); /* order = {3,1,0,2} */
~~~

The `order` buffer must be a true array with at least n elements. The source
and destination must **not overlap**. For one unsorted original-index lookup,
prefer `find_ints(a,n,key)` (first occurrence or -1), without sorting.

`first_true_int(begin,end,predicate)` uses a half-open `[begin,end)` range,
where the predicate must be monotone from false to true. It returns the first
satisfying integer, or **end if none exists**:

~~~c
bool enough(int x) { return x >= 10; }
int answer = first_true_int(0, 100, enough); /* 10 */
~~~

The callback is `bool predicate(int)`. An empty range returns `end`, and
negative / extreme int bounds are supported without midpoint overflow.

## Strings

Byte-oriented C strings (not Unicode code points): `str_reverse`, `str_lower`, `str_upper`, `str_trim`, `str_is_palindrome`, `str_count_char`, `str_count_substr(s,needle,overlapping)`, `str_longest_char_run`, `str_insert`, `str_erase`, `str_replace_char`, `str_remove_char`, `str_replace_all`, `str_sort_chars`, `str_unique_chars`.

`str_insert` and `str_replace_all` change length, so they use auto-capacity macros. `str_replace_all` replaces non-overlapping matches. Their text/needle/replacement arguments must **not point into the destination string buffer**. For standard operations still use `strlen`, `strcmp`, `strstr` and `snprintf`.

### Fixed-width string lists: split and join

~~~c
char words[100][50];
int n = str_split("apple,,orange,", ",", words);
/* words = {"apple", "", "orange", ""}, n == 4 */
char result[300];
str_join(words, n, ",", result); /* "apple,,orange," */
~~~

- `str_split(text,separator,words)` treats the separator as a **literal nonempty
  string** (so `"::"` works), preserves empty fields, and returns one empty
  field for an empty input string.
- `str_split_whitespace(text,words)` splits on runs of whitespace, omits empty
  fields and returns zero fields for all-whitespace input.
- `str_join(words,n,separator,result)` joins exactly the first n rows, inserts
  separators only **between** fields, and returns no value. For n=0 the output
  is an empty string.

Both 2D input/output macros infer the number of rows and fixed field width
from `char words[ROWS][WIDTH]`. The join output must be a separately declared
real `char result[CAPACITY]` array. They check the number of rows, each field
width, and the joined output capacity; overflow terminates with a message.
They are **byte-oriented**, not Unicode-aware. Input/source and output buffers
must not overlap. For true array parameters/pointers, explicit-capacity
functions `str_split_into`, `str_split_whitespace_into`, and `str_join_into`
are available.


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

**Intentionally excluded for now**: multi-type generic `sort_by`, specialized repeated-query original-index indexes, stable generic record sorting, dynamic StringList/Vector/Matrix containers, ST/RMQ, DFS/grid paths, generalized matrices, heap/deque/map/set, and quadratic-equation special outputs. Multi-key record sorting is covered by `Record` / `record_cmp`.

**Testing:** `python3 tests/test_lightweight_io.py` and `python3 tests/test_lightweight_algorithms.py` compile and run C99 with complete and dependency-selected headers. Windows/MinGW, macOS and Linux are CI targets. CI also checks the upstream standalone CLI's real initialization, dependency selection, assembly, compile and run behavior.
