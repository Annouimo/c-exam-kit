/** @category 输入和输出 */
/**
 * @public
 * @requires exam_io_fail exam_io_write print_int
 * @category 输入和输出
 * @title 输出数组
 * @members print_ints print_lls print_doubles
 * `print_ints(a, n, sep, end)`、`print_lls(a, n, sep, end)`、`print_doubles(a, n, digits, sep, end)`。空格分隔、末尾换行可传 `" ", "\n"`
 */
static inline void print_ints(const int *array, int count,
                                   const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid int array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_int(array[i], "");
    }
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_fail exam_io_write print_ll
 */
static inline void print_lls(const long long *array, int count,
                                  const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid long long array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_ll(array[i], "");
    }
    exam_io_write(end);
}
/**
 * @public
 * @requires exam_io_fail exam_io_write print_double
 */
static inline void print_doubles(const double *array, int count, int digits,
                                      const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid double array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_double(array[i], digits, "");
    }
    exam_io_write(end);
}
