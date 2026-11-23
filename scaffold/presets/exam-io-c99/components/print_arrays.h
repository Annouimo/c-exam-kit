static inline void print_ints(const int *array, int count,
                                   const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid int array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_int(array[i], "");
    }
    exam_io_write(end);
}
static inline void print_lls(const long long *array, int count,
                                  const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid long long array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_ll(array[i], "");
    }
    exam_io_write(end);
}
static inline void print_doubles(const double *array, int count, int digits,
                                      const char *separator, const char *end) {
    if (count < 0 || (count && !array)) exam_io_fail("invalid double array");
    for (int i = 0; i < count; ++i) {
        if (i) exam_io_write(separator);
        print_double(array[i], digits, "");
    }
    exam_io_write(end);
}
