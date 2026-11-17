# 输入输出 · v0.5.0

默认忽略用于分隔数据的空白；题目把空格、空行当作数据时，显式选 raw 接口。

## 默认与精确读取

| 接口 | 行为 |
| --- | --- |
| read_int / read_ll / read_double | 跳过前导空白，读取一个完整数值 token |
| read_word | 跳过前导空白，读取一个非空白串，动态管理容量 |
| read_line | 跳过空行和纯空白行，去掉行首尾空白，保留内部空格 |
| read_char | 跳过前导空白，读取一个非空白字节 |
| read_line_raw | 从当前位置读到行尾，保留空行和首尾空格 |
| read_next_line_raw | 先结束当前 token 所在行，再精确读下一行；下一行为空也照读 |
| read_char_raw | 读取一个原始字节，包括空格和换行 |
| read_rest | 与 read_line_raw 相同，强调“读取当前行剩余内容” |
| read_next_line | 先结束当前 token 所在行，再调用默认 read_line |

“默认忽略空白”不表示删除句子内部的空格。读取 "  hello   world  "，read_line 返回 "hello   world"，read_line_raw 返回完整的 "  hello   world  "。

read_line 从当前位置开始，不强制跳到下一物理行：输入 "42  hello world"，先 read_int 得到 42，再 read_line 得到 "hello world"。输入 "42\n\n  hello world  " 时，同样得到 "hello world"。题目按行组织且当前行其余内容必须为空白时，用 finish_line 或 read_next_line 系列明确跨行。

raw 行函数去掉行结束符，识别 LF、CRLF 和单独 CR，不去掉其他空白。需要读取换行字节本身时使用 read_char_raw；FILE 在 Windows 文本模式下可能已将 CRLF 转为 LF，要比较真实文件字节请自行用 rb 模式打开。

## 最常见的两种题目

通常只关心有效数据：

~~~c
int n = read_int();
String name = read_line(); /* 自动跨过残留换行与空白行 */
IntVector values = read_ints(3);
print_string(&name, "\n");
print_ints(&values, " ", "\n");
string_free(&name);
IntVector_free(&values);
~~~

给出 n，后面恰好 n 行；空行、缩进也是数据：

~~~c
int n = read_int();
if (n < 0) tb_fail("negative line count");
finish_line();
for (int i = 0; i < n; ++i) {
    String text = read_line_raw();
    print_string(&text, "\n");
    string_free(&text);
}
~~~

例如输入 "3\n\n  a  \n \t \n"，三次精确读取分别得到空串、"  a  "、" \t "。不会漏掉第一条空记录。

仅有一次“数字 → 下一整行”转换，也可简写：

~~~c
int n = read_int();
String first = read_next_line_raw(); /* 允许空行，保留两端空白 */
string_free(&first);
~~~

finish_line 不会默默丢弃同一行剩余的文字；遇到非空白返回 IO_EXTRA_DATA（简便接口会报错退出）。此时验证已消耗前导水平空白，首个非空白字节仍留在流中。若要完整保留行尾内容，直接调用 read_rest，不要先调用 finish_line；若明确希望丢弃，则调用 discard_line。已经在行边界时 finish_line 不做任何事，不会再吞掉一行。

## EOF、输入错误和严格解析

read_* 是预期输入存在的简便接口，遇到 EOF、非法格式或超出范围会向 stderr 报错并退出，不会返回假造的零。

try_read_* 返回 IOStatus，失败不修改输出变量。String 输出变量须先用 string_new() 或 {0} 初始化，并负责最终释放。

| 状态 | 含义 |
| --- | --- |
| IO_OK | 成功，枚举值为 0 |
| IO_EOF | 无可读数据；默认行读取可能已跳过末尾空白行 |
| IO_FORMAT | 格式不合法，或文本输入包含 NUL |
| IO_RANGE | 数值超出目标范围；浮点溢出、下溢或非有限值 |
| IO_STREAM_ERROR | 底层流读写失败 |
| IO_EXTRA_DATA | 显式结束当前行时发现剩余文字 |

必须与 IO_OK 比较，不要把返回值直接当 bool：

~~~c
int x;
IOStatus status;
while ((status = try_read_int(&x)) == IO_OK)
    print_int(x, "\n");
if (status != IO_EOF) io_require(status);
~~~

token 数字解析要求整个 token 合法："12abc" 报错，不会像某些 scanf 用法那样先读入 12、留下 abc。非法 token 会被消费，随后可继续读取下一个 token。整数使用十进制；double 使用当前 C locale 的 strtod 语法，支持科学计数法，拒绝 NaN、无穷大和范围错误。

也可先精确读取一行，再验证行内只能有一个数字：

~~~c
String line = read_line_raw();
int n;
io_require(parse_int(string_cstr(&line), &n));
string_free(&line);
~~~

parse_int / parse_ll / parse_double 允许两端空白，但不允许第二个 token。parse_int("12 13", &n) 返回 IO_FORMAT。

## 批量读写

| 输入 | 返回类型 | 对应输出 |
| --- | --- | --- |
| read_ints(n) | IntVector | print_ints(&v, separator, end) |
| read_lls(n) | LongLongVector | print_lls(&v, separator, end) |
| read_doubles(n) | DoubleVector | print_doubles(&v, precision, separator, end) |
| read_words(n) | StringList | print_strings(&words, separator, end) |

批量读取支持 n=0。n 来自有符号输入时，先检查非负再传入；函数参数为 size_t。结果拥有独立内存，分别用对应 Vector_free 或 string_list_free 释放。

print_int(value,end)、print_ll(value,end)、print_double(value,precision,end)、print_text(text,end)、print_string(&text,end) 用于单值输出。end="" 表示不换行，end="\n" 表示换行。容器输出不加尾部分隔符；空容器只输出 end。复杂格式继续用 printf。

## 文件输入与装配

~~~sh
python c_toolbox_v0.5.0.py --modules io --main your_main.c -o answer.c
cc -std=c99 -O2 answer.c -o answer -lm
~~~

io 自动带入 String、IntVector、LongLongVector、DoubleVector。用户额外配置的 Vector 可共存，但不能把这些内置类型改成其他元素类型。

默认接口使用 stdin；要读文件，可创建 IOReader reader = io_reader(file)，使用 io_read_int(&reader,&n)、io_read_line(&reader,&text)、io_read_line_raw、io_read_next_line_raw、io_read_char_raw 等接口，规则相同。reader 不拥有 FILE，也不需释放；调用者负责 fclose。输出简便接口使用 stdout，文件输出继续使用 fprintf/fputs。

同一 FILE 只用一个 IOReader，不要在中间混用 scanf/getchar/string_readline/string_read_token 等输入操作；它们不会更新行边界状态。与 printf 混用输出没有此限制。已有 String 的底层输入函数保持原行为；新默认规则只适用于 io 模块。

文本以字节处理，空白依据 C 的 isspace，不提供 Unicode 空白识别。需要处理 NUL 或其他二进制内容时，使用原始字符/标准文件接口。输入结束或出错不回滚已经消费的字节，只有输出变量保持原值。

说明正文：CC BY 4.0；代码示例：MIT-0。作者 Annouimo，来源 https://github.com/Annouimo/c-exam-kit 。

数值 Vector 的默认排序和统计、StringList 的所有权管理、String 到数值的直接转换，见 [对象接口说明](objects.md)。
