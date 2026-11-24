/** @category 输入和输出 */
/**
 * @public
 * @requires exam_io_fail exam_io_write
 * @category 输入和输出
 * @title 输出值
 * @members print_int print_ll print_double print_char print_text
 * `print_int(x, end)`、`print_ll(x, end)`、`print_double(x, digits, end)`、`print_char(ch, end)`、`print_text(s, end)`
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
 */
static inline void print_char(char value, const char *end) {
    if (putchar((unsigned char)value) == EOF) exam_io_fail("output stream error");
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_write
 */
static inline void print_text(const char *value, const char *end) {
    exam_io_write(value);
    exam_io_write(end);
}
