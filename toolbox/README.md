# C Toolbox · v0.3.0 原型

面向熟悉 Python / JS / C++ 的 C 语言考生。Python 3.9+ 装配器离线生成独立 C99 代码；生成结果不依赖 Python、第三方库或运行时外部文件。此项目与课程脚手架的接口、版本、发布流程独立，当前同仓库开发。

当前提供 String、类型化 Vector / Deque / Heap、字符串键 Map / Set / Counter。尚未实现通用键值类型、Matrix 或完整高级语言标准库；普通数学算法仍可从原脚手架生成后组合使用。

## 输入输出（v0.3.0）

新增 io 模块：默认跳过空白，精确读取用 raw 接口。read_line 跳过空行、去掉行首尾空白并保留内部空格；read_line_raw 保留空行与首尾空白。数字后要精确读取下一行，用 read_next_line_raw。单值、动态文本、Vector 批量读写、数值验证和 EOF 状态使用同一套规则。

[完整输入输出说明](docs/io.md) · [保留空行示例](examples/io_lines.c) · [新版删词题](examples/io_remove_words.c)

## 使用

开发工程中先构建单文件装配器：

~~~sh
python toolbox/build.py
python toolbox/dist/c_toolbox_v0.3.0.py --list
python toolbox/dist/c_toolbox_v0.3.0.py --modules string counter -o answer.c
~~~

分发时只需要 c_toolbox_v0.3.0.py，无第三方 Python 依赖。无参数在终端显示简单的组件选择提示；非交互输入时默认生成全部组件。--modules 明确选择组件，自动加入依赖，不会加入未选中的其他对象。生成结果默认是组件函数，没有 main；粘贴至答案或用 --main 追加自己的完整程序。

可运行的单词计数例子（顺序不保证，按字面空格分割，不是通用分词器）：

~~~sh
python toolbox/dist/c_toolbox_v0.3.0.py --modules string counter --main toolbox/examples/word_count.c -o answer.c
cc -std=c99 -O2 answer.c -o answer
~~~

用户结构体与类型化容器：

~~~sh
python toolbox/dist/c_toolbox_v0.3.0.py --modules vector --vector StudentVector=Student --preamble toolbox/examples/student_types.c --main toolbox/examples/student_sort.c -o students.c
cc -std=c99 -O2 students.c -o students
~~~

--vector 可重复使用；名字须以大写字母开头。不指定时生成 IntVector=int。复杂类型先写 typedef，通过 --preamble 在库类型之后、类型化容器之前插入。普通数组直接使用 .data 与 .len。排序沿用 C 的 qsort 比较器。

## 统一约定

- 动态对象支持 {0} 初始化；String、Vector、Deque、Map、Set、Counter 也有 _new() 构造函数。String 还可用 string_from("abc")。Heap 必须用 Name_new(compare) 或 Name_from(items, count, compare) 设置比较器后再插入；free 后重新使用也要重新构造。
- 拥有内存的对象不能通过普通结构体赋值获得独立副本；使用对应 clone。返回新对象可以按值接收，接收者负责释放。
- String、Map 的键、StringList 中的字符串拥有独立存储。Map 当前只支持字符串键和 long long 值，不保留插入顺序。Set / Counter 共用 Map 底层。
- Vector / Deque / Heap 只浅复制元素，不负责元素内部资源。存 String 或指针时自行逐项释放；这些容器的 clone 也仅复制元素值。避免误以为嵌套容器会自动深复制。
- _free(&object) 释放并清零，可重复调用。clear 保留分配的容量。
- 字符串位置与长度按字节计算，保持普通 C 字符串的零终止规则，不支持内嵌 NUL，不按 Unicode 字符索引。不要直接改写 data 却遗漏更新长度或终止符。
- 编辑使用“0 下标起点 + 数量”，不混用闭区间。插入允许在末尾；erase 不截断越界范围，越界返回 false 且不修改对象。
- 无结果的查询用 bool 或 SIZE_MAX 表示。分配失败与容量计算溢出统一向 stderr 报错并退出；不静默截断。非法 slice、空 split 分隔符也报错退出。
- 扩容可能使 data 指针失效；Map 遍历期间不得修改 Map，遍历得到的键是借用指针。
- 所有函数是 static，便于直接粘贴到单个答案。编译时若开启未使用函数警告，可按需裁剪或使用 -Wno-unused-function。

## 接口速查

| 对象 | 操作 |
| --- | --- |
| String | new / from / clone / cstr / reserve / assign / clear / append / push / pop / insert / erase / slice / find / contains / starts_with / ends_with / count / replace_all / remove_first / remove_all / remove_repeated / trim / lower / upper / readline / read_token / split / join / free |
| StringList | split 返回独立字符串列表；string_list_free 逐项释放 |
| Name 类型 Vector | Name_new / reserve / push / pop / insert / erase / get / set / front / back / extend / resize / reverse / clone / clear / sort / free |
| Map | new / set / get / get_or / contains / size / clear / remove / next / clone / free |
| Set | new / add / contains / remove / size / clear / next / clone / union / intersection / difference / is_subset / free |
| Counter | new / add / get / remove / size / clear / next / most_common / clone / free |
| Name 类型 Deque | Name_new / reserve / push_front / push_back / pop_front / pop_back / front / back / get / clear / clone / free |
| Name 类型 Heap | Name_new(compare) / from / reserve / push / peek / pop / clear / clone / free |
| io | read_int / ll / double / word / line / char、raw 精确读法、try_read 状态、批量读写 |

String 的 find 接收起始下标，未找到返回 SIZE_MAX；空目标匹配起点。replace_all 使用不重叠替换，空目标不修改；支持旧串、替换串引用原字符串。split 接收非空字面分隔串并保留空字段，join 返回新 String。readline 去掉 LF 和末尾 CR，即使空行也返回 true，立即 EOF 返回 false。

Map_get 接收可选输出指针，用 bool 区分缺失与值为零。Map_set / Set_add 返回是否新增，替换已有值返回 false，不表示失败。Map_next / Set_next / Counter_next 的游标用 size_t 初始化为 0。Counter 缺失项读为零，允许零和负计数；add 溢出返回 false，原值保留。

~~~c
String text = string_from("a,b,a");
StringList words = string_split(&text, ",");
Counter counts = counter_new();

for (size_t i = 0; i < words.len; ++i)
    if (!counter_add(&counts, string_cstr(&words.data[i]), 1))
        tb_fail("counter overflow");

printf("%lld\n", counter_get(&counts, "a")); /* 2 */
string_list_free(&words);
string_free(&text);
counter_free(&counts);
~~~

String 编辑和 Vector 中间插入/删除为 O(n)，Vector push 均摊 O(1)；Map / Set / Counter 查询和更新平均 O(1)，哈希碰撞严重时可能 O(n)。String 的查找和替换使用朴素字符串匹配，不承诺线性时间。

## 开发与验证

### v0.2.0 新增用法

构造空字符串优先写 String text = string_new()，不要只写未初始化的 String text。string_read_token 动态读取一个非空白串，不需猜测字符数组容量；会消费结束处的一个空白字符，混用 readline 时需考虑剩余行内容。读取失败会清空原内容，返回 false。

三种删除行为明确区分（空目标均不删除）：

| 接口 | 对 aabb 删除 ab 的结果 |
| --- | --- |
| string_remove_first | ab，返回 true |
| string_remove_all | ab，单轮删除原有的不重叠匹配 |
| string_remove_repeated | 空串，返回删除次数 2 |

remove_repeated 每次从头查找，可能为 O(n²)。替换、插入、赋值、反复删除均允许输入文本引用原 String。count 计数不重叠匹配，空目标返回 0。trim 按 C 的 isspace 去两端空白；lower / upper 是当前 C locale 下的字节转换，不是 Unicode 大小写处理。

删词题可直接写：

~~~c
String text = string_new(), word = string_new();
string_readline(&text, stdin);
int n;
scanf("%d", &n);
for (int i = 0; i < n && string_read_token(&word, stdin); ++i)
    string_remove_repeated(&text, string_cstr(&word));
puts(string_cstr(&text));
string_free(&word);
string_free(&text);
~~~

完整示例是 examples/remove_words.c。Vector 的 extend 支持把自身的元素区间追加至末尾；resize 扩大时填入传入的值，缩小时不会释放元素内部资源。get / set / front / back 越界或为空时返回 false，读取失败不修改输出变量。

选择 deque 默认生成 IntDeque；--deque TaskQueue=Task 可生成用户类型。队列用 push_back / pop_front，栈用 push_back / pop_back，双端操作均摊 O(1)，get 按逻辑位置读取，不能把环形存储的 data 当成连续逻辑数组。

~~~c
IntDeque queue = IntDeque_new();
IntDeque_push_back(&queue, 10);
IntDeque_push_back(&queue, 20);
int value;
while (IntDeque_pop_front(&queue, &value)) printf("%d\n", value);
IntDeque_free(&queue);
~~~

选择 heap 默认生成 IntHeap；--heap TaskHeap=Task 指定类型。比较器与 qsort 形状相同，返回负数表示第一个参数优先。push / pop 为 O(log n)，peek 为 O(1)，from 批量建堆为 O(n)。相等优先级的顺序不保证；需要稳定顺序时把序号纳入比较器。

~~~c
static int compare_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
/* 在 main 中： */
IntHeap pending = IntHeap_new(compare_int);
IntHeap_push(&pending, 30);
IntHeap_push(&pending, 10);
int value;
while (IntHeap_pop(&pending, &value)) printf("%d\n", value);
IntHeap_free(&pending);
~~~

比较器反转即可变成最大堆。完整示例是 examples/priority_queue.c。

Set 的 union / intersection / difference 返回独立新集合，不修改输入，调用者负责释放。is_subset(a,b) 表示 a 是否为 b 的子集。Counter_most_common 接收两个可选输出指针，只返回频次最高的一项，并列时选字典序最小的键；空计数器返回 false，返回的键是借用指针。

类型化参数可重复使用。--modules vector --vector Words=String 自动带入 String，无须重复选择字符串组件；显式配置的嵌套容器按依赖顺序生成，循环依赖会报告错误。容器只浅复制元素，即使自动解析依赖也不会自动深复制或释放元素内部资源。

### 工程检查

| 路径 | 内容 |
| --- | --- |
| src/generator.py | 参数、依赖解析和源码装配逻辑 |
| catalog.json | 组件元数据、依赖与版本 |
| templates/*.c | 可直接阅读和修改的 C 实现 |
| build.py | 将逻辑、配置、模板及许可组装为单文件 Python |
| examples/ | 可执行示例及自定义类型 |
| tests/test_toolbox.py | 脱离工程运行、边界与随机对照测试 |
| dist/ | 本地构建产物，不手工修改 |

~~~sh
python toolbox/tests/test_toolbox.py
python toolbox/build.py --check
~~~

独立 Actions 工作流验证 Linux GCC / Clang、macOS Clang、Windows MinGW，并在 Linux Clang 下启用 AddressSanitizer / UndefinedBehaviorSanitizer。Actions 附件提供单文件装配器、全量组件头文件和示例。不会创建正式 Release。

原型代码由本项目独立实现，未引入第三方库。工程 Python / 测试 / 构建 / 工作流按 MIT；C 模板、生成的 C 代码及示例按 MIT-0；说明文字按 CC BY 4.0。单文件 Python 含 MIT 与 MIT-0 内容，须保留适用许可；生成的 C 答案可删除署名及注释。参见项目根目录 LICENSING.md。

运行装配器与携带静态资料是不同的考试许可事项，沿用项目现有考试使用约定。

