/** @category 输入和输出 */
/* read_line: read the next COMPLETE line after a partial token/char read.
 * read_rest: read from the current position (including leading spaces).
 * Real empty lines are preserved; EOF before any character returns false. */
/**
 * @public
 * @requires exam_io_fail exam_io_partial_line exam_io_skip_current_line exam_io_line_buffer
 */
static inline bool try_read_line_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null line destination");
    if (*exam_io_partial_line()) exam_io_skip_current_line();
    return exam_io_line_buffer(out, capacity) != 0;
}
/**
 * @public
 * @requires try_read_line_into exam_io_fail
 */
static inline void read_line_into(char *out, size_t capacity) {
    if (!try_read_line_into(out, capacity))
        exam_io_fail("unexpected EOF while reading line");
}
/**
 * @public
 * @requires exam_io_fail exam_io_line_buffer
 */
static inline bool try_read_rest_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null line destination");
    return exam_io_line_buffer(out, capacity) != 0;
}
/**
 * @public
 * @requires try_read_rest_into exam_io_fail
 */
static inline void read_rest_into(char *out, size_t capacity) {
    if (!try_read_rest_into(out, capacity))
        exam_io_fail("unexpected EOF while reading rest of line");
}
/**
 * @public
 * @requires exam_io_skip_current_line
 */
/**
 * @id skip_line
 * @title 跳过本行
 * @keywords 忽略 换行
 * 丢弃当前行剩余输入，直到行结束或 EOF。
 * @example
 * skip_line();
 */
static inline void skip_line(void) {
    exam_io_skip_current_line();
}
/**
 * @public
 * @requires try_read_line_into
 */
#define try_read_line(array) try_read_line_into((array), sizeof(array))
/**
 * @public
 * @requires read_line_into
 */
/**
 * @id read_line
 * @title 读一整行
 * @keywords 字符串 整行 fgets EOF
 * @members read_line try_read_line read_line_into try_read_line_into
 * 读取下一整行，不保留行结束符。若前面已经读取本行的一部分 token，先丢弃本行剩余内容。保留真正的空行。try 版本正常 EOF 返回 false；指针使用 *_into。
 * @example
 * char line[100];
 * read_line(line);
 */
#define read_line(array) read_line_into((array), sizeof(array))
/**
 * @public
 * @requires try_read_rest_into
 */
#define try_read_rest(array) try_read_rest_into((array), sizeof(array))
/**
 * @public
 * @requires read_rest_into
 * @category 输入和输出
 */
/**
 * @id read_rest
 * @title 读本行剩余内容
 * @keywords 字符串 换行 空格 EOF
 * @members read_rest try_read_rest read_rest_into try_read_rest_into
 * 读取当前行剩余内容，保留前导空白，不保留行结束符。try 版本正常 EOF 返回 false；指针使用 *_into。
 * @example
 * int n = read_int();
 * char rest[100];
 * read_rest(rest);
 */
#define read_rest(array) read_rest_into((array), sizeof(array))
