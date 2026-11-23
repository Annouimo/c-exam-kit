static inline bool try_read_char_raw(char *value) {
    if (!value) exam_io_fail("null char destination");
    int c = exam_io_get();
    if (c == EOF) return false;
    *value = (char)c;
    *exam_io_partial_line() = (c != '\n' && c != '\r');
    return true;
}
static inline char read_char_raw(void) {
    char value;
    if (!try_read_char_raw(&value)) exam_io_fail("unexpected EOF while reading char");
    return value;
}
static inline bool try_read_char(char *value) {
    if (!value) exam_io_fail("null char destination");
    int c;
    do { c = exam_io_get(); } while (exam_io_space(c));
    if (c == EOF) return false;
    *value = (char)c;
    *exam_io_partial_line() = 1;
    return true;
}
static inline char read_char(void) {
    char value;
    if (!try_read_char(&value)) exam_io_fail("unexpected EOF while reading char");
    return value;
}
