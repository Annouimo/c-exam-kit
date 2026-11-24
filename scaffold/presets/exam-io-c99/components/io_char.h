/** @category 输入和输出 */
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
 * @category 输入和输出
 * @title 读一个字符
 * @members read_char read_char_raw
 * `read_char()` 跳过空白，`read_char_raw()` 保留空格和换行。两者均有 `try_read_…(&ch)` 版本
 */
static inline char read_char(void) {
    char value;
    if (!try_read_char(&value)) exam_io_fail("unexpected EOF while reading char");
    return value;
}
