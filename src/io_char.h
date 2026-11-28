/**
 * @public
 * @requires exam_io_fail exam_io_get exam_io_partial_line
 */
static inline bool try_read_char_raw(char *value) {
    if (!value) exam_io_fail("null char destination");
    int c = exam_io_get();
    if (c == EOF) return false;
    *value = (char)c;
    *exam_io_partial_line() = (c != '\n' && c != '\r');
    return true;
}
/**
 * @public
 * @requires try_read_char_raw exam_io_fail
 */
static inline char read_char_raw(void) {
    char value;
    if (!try_read_char_raw(&value)) exam_io_fail("unexpected EOF while reading char");
    return value;
}
/**
 * @public
 * @requires exam_io_fail exam_io_get exam_io_space exam_io_partial_line
 * @id try_read_char
 * @title 尝试读取字符
 * @tags 输入 字符串
 * @summary 尝试读取字符，支持保留或跳过空白。
 * @return 正常 EOF 返回 false，读到字符返回 true。
 * @keywords EOF 字符 getchar
 * @members try_read_char try_read_char_raw
 * 正常 EOF 返回 false；`try_read_char` 跳过空白，`try_read_char_raw` 保留空白。
 * @example
 * char ch;
 * while (try_read_char_raw(&ch)) {
 *     print_char(ch, "");
 * }
 */
static inline bool try_read_char(char *value) {
    if (!value) exam_io_fail("null char destination");
    int c;
    do { c = exam_io_get(); } while (exam_io_space(c));
    if (c == EOF) return false;
    *value = (char)c;
    *exam_io_partial_line() = 1;
    return true;
}
/**
 * @public
 * @requires try_read_char exam_io_fail
 * @id read_char
 * @title 读一个字符
 * @tags 输入 字符串
 * @summary 读取一个字符，可选择是否跳过空白。
 * @error 提前 EOF 时结束程序。
 * @keywords 字符 空格 换行 getchar
 * @members read_char read_char_raw
 * `read_char()` 跳过空白；`read_char_raw()` 保留空格和换行。提前遇到 EOF 时结束程序。
 * @example
 * char ch = read_char();
 */
static inline char read_char(void) {
    char value;
    if (!try_read_char(&value)) exam_io_fail("unexpected EOF while reading char");
    return value;
}
