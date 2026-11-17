# 常见任务只用对象接口完成 · v0.4.0

目标：对于已经覆盖的任务，读入、处理、组合、输出和释放均有工具箱接口；原生数组和字符指针作为特殊需求的出口。C99 下用明确的函数名区分 String 对象与字面量，不依赖函数重载或 C11 泛型。

## 一条完整的字符串流程

~~~c
String text = read_line();
StringList words = string_split_whitespace(&text);
Counter counts = counter_from_strings(&words);
StringList keys = counter_keys(&counts);
string_list_sort(&keys);
for (size_t i = 0; i < string_list_size(&keys); ++i) {
    const String *word = string_list_get(&keys, i);
    print_string(word, ": ");
    print_ll(counter_get_s(&counts, word), "\n");
}
string_list_free(&keys);
counter_free(&counts);
string_list_free(&words);
string_free(&text);
~~~

装配选择 io、counter。这里不需要 strlen、strcmp、strtok、qsort，也不需要取出字符指针或手动释放列表中的每个字符串。完整程序见 examples/word_report.c。

## StringList 是常用的拥有字符串的列表

split、按空白分词和 read_words 都返回同一种 StringList。列表负责释放其中的字符串：

| 接口 | 语义 |
| --- | --- |
| string_list_new / size / empty / reserve | 创建、查询和预留容量 |
| string_list_push(&list,&text) | 深复制 text，调用者继续拥有原对象 |
| string_list_push_text(&list,"abc") | 复制字面量或原生 C 字符串 |
| string_list_push_take(&list,&text) | 接管 text 的内存，并把原对象清零 |
| string_list_get(&list,i) | 返回借用的 const String 指针 |
| string_list_at(&list,i) | 返回借用的可修改 String 指针 |
| string_list_insert / set | 插入或替换，深复制输入，自动释放被替换内容 |
| string_list_erase(&list,pos,count) | 删除并释放目标字符串 |
| string_list_pop(&list,&out) | 把末项的所有权转移给已初始化的 out；out=NULL 则直接释放 |
| string_list_clone / slice | 独立深复制；slice 使用起点和数量 |
| string_list_find / contains | 按内容查找 String；find 未找到返回 SIZE_MAX |
| string_list_sort / sort_by / reverse | 默认按字典序排序；特殊规则可自定义比较器 |
| string_list_unique_sorted | 删除相邻重复项，通常在排序后调用 |
| string_list_clear / free | 逐项释放；clear 保留列表容量，free 还释放列表存储 |

get/at 越界会明确报错退出；insert/set/erase 越界返回 false。pop 的输出位置不能是列表内部元素。借用指针不需要单独释放；列表扩容、排序或删除后不要继续使用旧指针。

所有权转移示例：

~~~c
StringList list = string_list_new();
String item = read_word();
string_list_push_take(&list, &item); /* item 变成空对象 */
string_free(&item);                 /* 仍安全 */
string_list_free(&list);            /* 内部字符串一起释放 */
~~~

普通类型化 Vector 继续浅复制元素，包括用户显式生成的 Vector<String>。字符串序列默认推荐 StringList；不要把通用 Vector 的 clone/free 当作深复制或逐项释放。

## String 间的操作

原有接口接受字面量；对应的 _s 接口直接接受 String：

~~~c
String text = read_line();
String needle = read_word();
string_remove_repeated_s(&text, &needle);
string_append(&text, "!");       /* 字面量 */
string_append_s(&text, &needle); /* String */
print_string(&text, "\n");
string_free(&needle);
string_free(&text);
~~~

提供 assign_s、append_s、insert_s、find_s、contains_s、count_s、replace_all_s、remove_first_s、remove_all_s、remove_repeated_s、split_s、join_s。比较使用 string_compare / string_equal；与字面量判等用 string_equal_text。

string_size / empty 查询长度；string_at 返回指定字节，越界报错；string_set 修改字节，越界或 NUL 返回 false。string_reverse / string_is_palindrome 分别反转和判断回文，按字节处理，不保证适合 Unicode 文本。string_count_overlapping 补充重叠计数，空目标返回 0；原 string_count 仍不重叠计数。

## 格式化与数值转换

~~~c
String label = string_format("%04d-%02d-%02d", 2026, 11, 17);
string_append_format(&label, " score=%.2f", 98.5);
print_string(&label, "\n");
string_free(&label);
~~~

不需要自己估计缓冲区长度。格式化规则沿用 printf，格式与参数类型必须匹配；不支持利用 %n 产生副作用，也不允许格式化结果中嵌入 NUL。两次格式化的结果长度不一致时会报错。

string_from_int、string_from_ll、string_from_double(value,precision) 负责数值到字符串。选择 io 模块后，string_to_int / ll / double 返回数值，非法格式或范围错误时报错；parse_int_s / ll_s / double_s 则返回 IOStatus，失败时保留输出变量。

## 数值 Vector

所有类型化 Vector 增加 size / empty / at / fill / slice / extend_vector。slice 复制元素但不改变元素本身的所有权规则，extend_vector 支持追加自身。at 越界报错，已有 get 返回 bool 的接口继续可用。

元素类型明确写为 int、long long 或 double 时，自动提供：

| 功能 | 接口后缀 |
| --- | --- |
| 默认排序 | sort_asc / sort_desc |
| 查找与计数 | find / contains / count |
| 升序数组的二分边界 | lower_bound / upper_bound |
| 相邻去重 | unique_sorted |
| 最值与位置 | min / max / argmin / argmax |
| 统计 | sum / mean / variance / stddev / median |

例如用户生成 Scores=double，也会得到 Scores_mean 等函数；用户结构体和未识别的类型别名仍使用通用容器接口。

~~~c
IntVector values = read_ints(5);
print_ll(IntVector_sum(&values), "\n");
print_double(IntVector_mean(&values), 2, "\n");
IntVector_sort_asc(&values);
IntVector_unique_sorted(&values);
print_ints(&values, " ", "\n");
IntVector_free(&values);
~~~

find 返回首次位置或 SIZE_MAX；lower_bound / upper_bound 要求升序，未找到边界时可以返回 size，不能直接据此访问元素。argmin/argmax 并列返回首次位置。

空容器的 sum 为 0；最值、均值、方差、中位数等需要非空输入。variance 是除以 n 的总体方差。median 克隆后排序，不改变原顺序。整数 sum 返回 long long，并在每一步累加前检查溢出；即使最终数学结果可表示，中间和超出范围也会报错。统计量是浮点近似；浮点排序、查找和统计要求有限数，查找与去重使用精确数值相等，不自动应用误差容限。

默认排序 O(n log n)，二分边界 O(log n)，其余线性统计 O(n)；median 需要额外副本且为 O(n log n)。

数值 Vector 含标准差计算，GCC/Clang 编译时链接数学库：

~~~sh
cc -std=c99 -O2 answer.c -o answer -lm
~~~

## String 与映射、集合、计数器组合

Map / Set / Counter 现在自动依赖 String，提供接受 String 键的 _s 接口。原来接受字面量的接口继续可用：

| 对象 | 新接口 |
| --- | --- |
| Map | set_s / get_s / get_or_s / contains_s / remove_s / keys |
| Set | add_s / contains_s / remove_s / from_strings / values |
| Counter | add_s / get_s / remove_s / from_strings / keys / most_common_string |

map_keys、set_values、counter_keys 返回拥有独立字符串副本的 StringList，可直接排序或输出；原容器释放后结果仍可使用。

counter_most_common_string 返回 bool，并将最高频键复制到已初始化的 String 输出对象；无需接收借用的 char 指针。并列仍按字典序最小键选择。

这一轮仍保留字符串键和 long long 值的 Map/Counter；整数键与用户结构体值尚未接入。日期、大整数、矩阵、前缀和、RMQ、排列组合等旧组件也尚未迁入新装配器，不能把旧板子已有功能视为新工具箱已经提供。

说明正文：CC BY 4.0；代码示例：MIT-0。作者 Annouimo，来源 https://github.com/Annouimo/c-exam-kit 。
