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
 * @title 读本行剩余内容
 * @members read_rest skip_line
 * `read_rest(s)` 保留当前行剩余的空格。想丢弃则用 `skip_line()`
 */
#define read_rest(array) read_rest_into((array), sizeof(array))
