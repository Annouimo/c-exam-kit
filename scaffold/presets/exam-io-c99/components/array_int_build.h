/** @category 数组 */
/* Destination must be a true array for the lowercase convenience macros. */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void fill_ints_into(int *array, int n, int value, size_t capacity) {
    if (n < 0 || (size_t)n > capacity || (n && !array))
        exam_algo_fail("fill_ints exceeds array capacity");
    for (int i = 0; i < n; ++i) array[i] = value;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void copy_ints_into(int *destination, const int *source,
                                  int n, size_t capacity) {
    if (n < 0 || (size_t)n > capacity || (n && (!destination || !source)))
        exam_algo_fail("copy_ints exceeds destination capacity");
    if (n) memmove(destination, source, (size_t)n * sizeof(int));
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int values_ints_into(int *destination, const int *values,
                                   size_t count, size_t capacity) {
    if (count > capacity || count > INT_MAX)
        exam_algo_fail("values_ints exceeds destination capacity");
    if (count) memmove(destination, values, count * sizeof(int));
    return (int)count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int range_ints_into(int *destination, int first, int stop,
                                  int step, size_t capacity) {
    if (!step) exam_algo_fail("range_ints step must not be zero");
    int count = 0;
    long long current = first;
    while ((step > 0 && current < stop) ||
           (step < 0 && current > stop)) {
        if ((size_t)count >= capacity || count == INT_MAX)
            exam_algo_fail("range_ints exceeds array capacity");
        destination[count++] = (int)current;
        current += step;
    }
    return count;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline void reverse_ints(int *array, int n) {
    exam_check_n(n);
    for (int i = 0; i < n / 2; ++i) {
        int temp = array[i];
        array[i] = array[n-1-i];
        array[n-1-i] = temp;
    }
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void swap_ints(int *array, int i, int j) {
    if (i < 0 || j < 0) exam_algo_fail("negative array index");
    int temp = array[i]; array[i] = array[j]; array[j] = temp;
}
/**
 * @public
 * @requires fill_ints_into
 * @category 数组
 * @title 填充、复制、生成
 * @members fill_ints fill_lls fill_doubles copy_ints copy_lls copy_doubles values_ints values_lls values_doubles range_ints range_lls
 * `fill_ints(a, n, v)`、`copy_ints(dst, src, n)`。`values_ints(a, 3, 1, 4)` 写入列表，`range_ints(a, first, stop, step)` 写入等差序列，二者返回写入个数，`stop` 不包含在内
 */
#define fill_ints(array, n, value) \
    fill_ints_into((array), (n), (value), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires copy_ints_into
 */
#define copy_ints(array, source, n) \
    copy_ints_into((array), (source), (n), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires values_ints_into
 */
#define values_ints(array, ...) \
    values_ints_into((array), (const int[]){__VA_ARGS__}, \
    sizeof((const int[]){__VA_ARGS__})/sizeof(int), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires range_ints_into
 */
#define range_ints(array, first, stop, step) \
    range_ints_into((array), (first), (stop), (step), sizeof(array)/sizeof((array)[0]))
