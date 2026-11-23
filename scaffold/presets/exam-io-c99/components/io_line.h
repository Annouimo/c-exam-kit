/* read_line: read the next COMPLETE line after a partial token/char read.
 * read_rest: read from the current position (including leading spaces).
 * Real empty lines are preserved; EOF before any character returns false. */
static inline bool try_read_line_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null line destination");
    if (*exam_io_partial_line()) exam_io_skip_current_line();
    return exam_io_line_buffer(out, capacity) != 0;
}
static inline void read_line_into(char *out, size_t capacity) {
    if (!try_read_line_into(out, capacity))
        exam_io_fail("unexpected EOF while reading line");
}
static inline bool try_read_rest_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null line destination");
    return exam_io_line_buffer(out, capacity) != 0;
}
static inline void read_rest_into(char *out, size_t capacity) {
    if (!try_read_rest_into(out, capacity))
        exam_io_fail("unexpected EOF while reading rest of line");
}
static inline void skip_line(void) {
    exam_io_skip_current_line();
}
#define try_read_line(array) try_read_line_into((array), sizeof(array))
#define read_line(array) read_line_into((array), sizeof(array))
#define try_read_rest(array) try_read_rest_into((array), sizeof(array))
#define read_rest(array) read_rest_into((array), sizeof(array))
