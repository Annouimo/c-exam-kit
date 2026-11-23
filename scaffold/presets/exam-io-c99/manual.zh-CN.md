# C 考试脚手架函数速查

适用版本 **C Exam Kit @PRESET_VERSION@ · exam-io-c99**。在脚手架生成的 `main.c` 中保留原有的头文件引用，即可调用下列函数。

下标从 **0** 开始，区间 `[l, r)` 包含 `l`、不包含 `r`。写入字符串、前缀和等缓冲区的接口通常从数组本身推算容量。把数组传进自定义函数后，参数会变成指针，这时用相应的 `*_into` 接口并传入容量。

## 1. 读入与输出

- **读一个数**
  `read_int()`、`read_ll()`、`read_double()` 直接返回读到的值

- **一直读到 EOF**
  `try_read_int(&x)`、`try_read_ll(&x)`、`try_read_double(&x)`。读到返回 `true`，正常 EOF 返回 `false`

- **读一个字符**
  `read_char()` 跳过空白，`read_char_raw()` 保留空格和换行。两者均有 `try_read_…(&ch)` 版本

- **读单词或整行**
  `read_word(s)` 读一个以空白分隔的单词，`read_line(s)` 读整行并去掉换行。若刚读过数字或单词，`read_line` 会先跳过当前行的剩余部分

- **读本行剩余内容**
  `read_rest(s)` 保留当前行剩余的空格。想丢弃则用 `skip_line()`

- **读数组**
  `read_ints(a, n)`、`read_lls(a, n)`、`read_doubles(a, n)`

- **输出值**
  `print_int(x, end)`、`print_ll(x, end)`、`print_double(x, digits, end)`、`print_char(ch, end)`、`print_text(s, end)`

- **输出数组**
  `print_ints(a, n, sep, end)`、`print_lls(a, n, sep, end)`、`print_doubles(a, n, digits, sep, end)`。空格分隔、末尾换行可传 `" ", "\n"`

`read_word`、`read_line`、`read_rest` 也有 `try_read_…(s)` 版本。用脚手架读取同一份输入时请统一使用这组读入接口，输出可以照常用 `printf`。

```c
int x;
while (try_read_int(&x)) {
    print_int(x, "\n");
}
```

## 2. 数组

以下以 `int` 数组为例。常用操作也有 `_lls` 和 `_doubles` 版本。

- **填充、复制、生成**
  `fill_ints(a, n, v)`、`copy_ints(dst, src, n)`。`values_ints(a, 3, 1, 4)` 写入列表，`range_ints(a, first, stop, step)` 写入等差序列，二者返回写入个数，`stop` 不包含在内

- **排序、反转、交换**
  `sort_ints(a, n)` 升序，`sort_ints_desc(a, n)` 降序，`reverse_ints(a, n)` 反转，`swap_ints(a, i, j)` 交换，均原地修改

- **找位置、数次数**
  `find_ints(a, n, x)` 返回第一次出现的下标，找不到返回 `-1`。`count_ints(a, n, x)` 返回出现次数

- **在升序数组中查找**
  `binary_search_ints(a, n, x)` 找第一次出现，找不到返回 `-1`。`lower_bound_ints(a, n, x)` 找首个 `>= x`，`upper_bound_ints(a, n, x)` 找首个 `> x`，两者找不到均返回 `n`

- **去重**
  `n = unique_sorted_ints(a, n)` 用于已排序数组。`n = unique_ints(a, n)` 保留首次出现的顺序，复杂度 `O(n²)`。结果在原数组的前 `n` 项

- **统计量**
  `sum_ints(a, n)` 返回 `long long`。`mean_ints`、`variance_ints`、`stddev_ints`、`median_ints` 返回 `double`，方差除以 `n`。`min_ints`、`max_ints` 返回值，`argmin_ints`、`argmax_ints` 返回首次出现的下标

- **连续段**
  `longest_equal_run_ints(a, n)` 和 `longest_plus_one_run_ints(a, n)` 返回最长相等段和逐项加一段的长度。`max_subarray_sum_ints(a, n)` 返回最大非空连续子段和。`max_forward_diff_ints(a, n)` 求 `i < j` 时 `a[i] - a[j]` 的最大值

- **频数、排序后的下标**
  `frequency_ints(a, n, freq, max_value)` 统计 `0..max_value`，`freq` 至少有 `max_value + 1` 项。`argsort_ints(a, n, order)` 把升序所对应的原下标写入 `order`，相等项保持原顺序

- **结构体排序**
  定义 `struct Record` 和 `int record_cmp(const Record *a, const Record *b)`，调用 `sort_records(a, n)` 原地排序。若 `a` 应排在 `b` 前面，比较函数返回负数

- **结构体查找**
  `find_records_by(a, n, &key, compare)` 按当前顺序扫描。`binary_search_records_by(a, n, &key, compare)` 要求数组已按 `compare` 排序。
  两者返回首个匹配的下标，找不到返回 `-1`。比较函数接收数组元素和查找键，类型均为 `const Record *`。初始化比较函数会读到的所有字段。排序与查找规则相同时，可以直接传入 `record_cmp`。

在 Companion 生成的 `main.c` 中保留已有的 `#include` 行，再写下面的程序。

```c
#include "exam.h"

struct Record { int id; };

int record_cmp(const Record *a, const Record *b) {
    return (a->id > b->id) - (a->id < b->id);
}

int main(void) {
    Record a[] = {{30}, {10}, {30}};
    Record key = {30};
    print_int(find_records_by(a, 3, &key, record_cmp), "\n");  // 0
    sort_records(a, 3);
    print_int(binary_search_records_by(a, 3, &key, record_cmp), "\n");  // 1
    return 0;
}
```

**多次区间求和**用前缀和。查询闭区间 `[l, r]` 时，右端传 `r + 1`。

```c
int a[] = {2, 4, 6, 8};
long long prefix[5];
build_prefix_ints(a, 4, prefix);
long long sum = prefix_range_sum(prefix, 1, 3);  // 4 + 6
```

**多次区间加值**用差分。设 `long long diff[n + 1], out[n]`，先 `build_diff_ints(a, n, diff)`，每次用 `diff_range_add(diff, n, l, r, delta)` 更新 `[l, r)`，最后用 `diff_to_lls(diff, n, out)` 还原。

## 3. 字符串

按字节处理 C 字符串。原地改写的参数应是可写的字符数组。

- **反转、大小写、去首尾空白**
  `str_reverse(s)`、`str_lower(s)`、`str_upper(s)`、`str_trim(s)` 均原地修改

- **回文、计数、最长连续段**
  `str_is_palindrome(s)` 判断回文。`str_count_char(s, ch)` 数字符。`str_count_substr(s, sub, true)` 允许重叠，传 `false` 统计非重叠匹配。`str_longest_char_run(s, ch)` 返回指定字符的最长连续长度

- **插入、删除、替换**
  `str_insert(s, pos, text)` 在下标 `pos` 前插入。`str_erase(s, pos, count)` 从 `pos` 删除 `count` 个字符，删除范围须在串内。`str_remove_char(s, ch)` 删除所有指定字符。`str_replace_char(s, old, new_ch)` 替换字符。`str_replace_all(s, old, replacement)` 替换非重叠子串并返回次数

- **排序、去重**
  `str_sort_chars(s)` 按字节值升序。`str_unique_chars(s)` 保留首次出现的顺序并返回新长度

- **拆分、拼接**
  `str_split(s, ",", words)` 按字面分隔符拆分并保留空字段。`str_split_whitespace(s, words)` 按空白拆分并跳过空字段。两者返回字段数。用 `str_join(words, n, "-", out)` 写入拼接结果，`words` 应是 `char words[行数][每行容量]` 这样的二维数组

## 4. 数学、数位与二分

- **公约数、公倍数、幂**
  `gcd(a, b)`、`lcm(a, b)` 返回非负值。`int_pow(x, e)`、`mod_pow(x, e, m)` 的指数取非负整数，模数取正整数

- **阶乘、质数**
  `factorial_basic(n)` 支持 `0..20`。`is_prime(x)` 判断单个数。`sieve_primes(flags, limit)` 一次标记 `0..limit`，`flags` 用容量至少为 `limit + 1` 的 `unsigned char` 数组

- **进制、数位**
  `base_to_int(s, base)` 返回 `long long`，`int_to_base(x, base, out)` 写入字符串，进制均为 `2..36`。`int_digit_count(x, base)` 和 `int_digit_sum(x, base)` 返回位数、数位和。`int_digit_at(x, base, pos)` 从最低位按 `0` 开始取位

- **拆分与合并数位**
  `int_to_digits(x, base, a)` 按高位到低位写入并返回位数。`digits_to_int(a, n, base)` 按同一顺序合并。`int_reverse_digits(x)` 反转十进制数位

- **浮点数、多项式**
  `nearly_equal(a, b, abs_tol, rel_tol)` 按绝对和相对误差判断。`round_digits(x, digits)` 四舍五入。`poly_eval_lls(coef, n, x)` 计算多项式，`coef[0]` 是常数项

- **二分答案**
  `first_true_int(l, r, check)` 找 `[l, r)` 中第一个满足 `bool check(int x)` 的整数。条件必须从假单调变真，全不满足时返回 `r`

- **二分求根**
  `bisect_root(f, l, r, tol, max_iter)` 返回近似根，`f` 是 `double f(double x)`。取连续函数的异号区间，端点恰好为根也可以

- **位运算**
  `bit_get(x, i)`、`bit_set(x, i)`、`bit_clear(x, i)`、`bit_toggle(x, i)`。最低位编号为 `0`，后三者返回修改后的值。`bit_count(x)` 统计置位数，`is_power_of_two(x)` 判断是否为二的幂

## 5. 大整数、枚举及其他类型

- **大整数输入输出**
  `BigInt a;` 后用 `read_bigint(&a)`、`try_read_bigint(&a)`、`bigint_from_str(&a, s)` 或 `bigint_from_ll(&a, x)` 赋值。`bigint_print(&a, "\n")` 输出。`bigint_to_str(&a, out)` 写入字符串。默认最多 2048 个十进制数位

- **大整数计算**
  `bigint_add(&c, &a, &b)`、`bigint_sub(&c, &a, &b)`、`bigint_mul(&c, &a, &b)` 均把结果写入第一个参数。`bigint_divmod(&q, &r, &a, &b)` 求商和余数，商向零截断。`bigint_compare(&a, &b)` 返回 `-1`、`0` 或 `1`。`bigint_factorial(&c, n)` 求阶乘

- **枚举组合、排列**
  `Combination it = combination_new(n, r)`，用 `while (combination_next(&it))` 逐个取，当前结果在 `it.indices[0..r-1]`。排列对应 `Permutation`、`permutation_new`、`permutation_next`。枚举的是 `0..n-1` 的下标，默认 `n <= 64`

- **复数**
  `Complex z = complex_new(real, imag)`。`complex_add(a, b)`、`complex_sub(a, b)`、`complex_mul(a, b)`、`complex_div(a, b)` 返回复数。`complex_abs(z)` 返回模，`complex_print(z, digits, end)` 输出

- **日期**
  `Date d = date_new(y, m, day)`。`date_add_days(d, k)` 返回移过 `k` 天的日期，`date_diff(a, b)` 返回 `b - a` 的天数。另有 `date_is_leap(y)`、`date_days_in_month(y, m)`、`date_day_of_year(d)`、`date_print(d, end)`

- **时刻**
  `Time t = time_new(h, m, s)`。`time_add_seconds(t, k)` 按 24 小时循环，`time_diff(a, b)` 直接返回 `b - a` 的秒数。另有 `time_to_seconds(t)`、`time_from_seconds(s)`、`time_print(t, end)`

手册正文 © 2026 Annouimo，采用 CC BY 4.0。示例代码采用 MIT-0。
项目来源 https://github.com/Annouimo/c-exam-kit
