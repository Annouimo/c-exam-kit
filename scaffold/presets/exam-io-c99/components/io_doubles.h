/** @category 输入和输出 */
/**
 * @public
 * @requires exam_io_fail read_double
 */
static inline void read_doubles_into(double *array, int count, size_t capacity) {
    if (count < 0 || (size_t)count > capacity)
        exam_io_fail("double array length exceeds capacity");
    if (count && !array) exam_io_fail("null double array");
    for (int i = 0; i < count; ++i) array[i] = read_double();
}
/**
 * @public
 * @requires read_doubles_into
 */
#define read_doubles(array, count) \
    read_doubles_into((array), (count), sizeof(array) / sizeof((array)[0]))
