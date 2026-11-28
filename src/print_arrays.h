/**
 * @public
 * @requires exam_io_fail exam_io_write print_int
 * @id print_arrays
 * @title 输出数组
 * @tags 输出 数组
 * @summary 按指定分隔符和结束字符串输出数组。
 * @keywords 数组 输出 分隔符 精度 打印 空格
 * @variant int print_ints
 * @variant ll print_lls
 * @variant double print_doubles
 * 输出前 n 个元素，sep 只放在元素之间，end 在最后输出。double 版本额外指定小数位数。
 * @example
 * {{type}} a[] = {1, 2, 3};
 * {{fn1}}(a, 3, " ", "\n");
 * @example double
 * double a[] = {1.5, 2.5};
 * print_doubles(a, 2, 2, " ", "\n");
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
