/**
 * @public
 * @requires exam_io_fail exam_io_write
 * @id print_number
 * @title 输出一个数
 * @tags 输出 数值
 * @summary 输出一个数值及结束字符串。
 * @keywords 输出 printf 精度 打印 小数
 * @variant int print_int
 * @variant ll print_ll
 * @variant double print_double
 * 输出数值后追加 end 字符串（例如换行或空格）。double 版本额外指定小数位数。
 * @example
 * {{fn1}}(42, "\n");
 * @example double
 * print_double(3.14159, 2, "\n");
 */
static inline void print_int(int value, const char *end) {
    if (printf("%d", value) < 0) exam_io_fail("output stream error");
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_fail exam_io_write
 */
static inline void print_ll(long long value, const char *end) {
    if (printf("%lld", value) < 0) exam_io_fail("output stream error");
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_fail exam_io_write
 */
static inline void print_double(double value, int digits, const char *end) {
    if (digits < 0 || digits > 100) exam_io_fail("invalid double precision");
    if (printf("%.*f", digits, value) < 0) exam_io_fail("output stream error");
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_fail exam_io_write
 * @id print_char
 * @title 输出字符
 * @tags 输出 字符串
 * @summary 输出一个字符及结束字符串。
 * @keywords 输出 字符 打印
 * 输出一个字符，再追加 end 字符串。
 * @example
 * print_char('A', "\n");
 */
static inline void print_char(char value, const char *end) {
    if (putchar((unsigned char)value) == EOF) exam_io_fail("output stream error");
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_write
 * @id print_text
 * @title 输出字符串
 * @tags 输出 字符串
 * @summary 输出以零结尾的字符串及结束字符串。
 * @keywords 输出 文本 打印
 * 输出以零结尾的字符串，再追加 end 字符串。
 * @example
 * print_text("hello", "\n");
 */
static inline void print_text(const char *value, const char *end) {
    exam_io_write(value);
    exam_io_write(end);
}
