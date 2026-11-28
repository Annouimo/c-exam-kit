
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
 * @id reverse_arrays
 * @title 反转数组
 * @tags 数组 修改
 * @summary 原地反转数组的前 n 项。
 * @keywords 倒序 翻转
 * @variant int reverse_ints
 * @variant ll reverse_lls
 * @variant double reverse_doubles
 * 原地反转前 n 项。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * {{fn1}}(a, 4);
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
 * @id swap_ints
 * @title 交换两项
 * @tags 数组 整数 修改
 * @summary 交换两个指定下标的整数元素。
 * @keywords 交换 位置
 * @members swap_ints
 * 交换下标 i、j 对应的两个 int 元素。调用者保证下标有效。
 * @example
 * int a[] = {1, 2, 3};
 * swap_ints(a, 0, 2);
 */
static inline void swap_ints(int *array, int i, int j) {
    if (i < 0 || j < 0) exam_algo_fail("negative array index");
    int temp = array[i]; array[i] = array[j]; array[j] = temp;
}
/**
 * @public
 * @requires fill_ints_into
 * @id fill_arrays
 * @title 填充数组
 * @tags 数组 构造 修改
 * @summary 将数组前 n 项设为指定值。
 * @keywords 初始化 填满 赋值
 * @variant int fill_ints fill_ints_into
 * @variant ll fill_lls fill_lls_into
 * @variant double fill_doubles fill_doubles_into
 * 将前 n 项设为 value；不会改变数组容量。普通接口要求实际数组，指针用显式容量接口。
 * @example
 * {{type}} a[10];
 * {{fn1}}(a, 10, 5);
 */
#define fill_ints(array, n, value) \
    fill_ints_into((array), (n), (value), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires copy_ints_into
 * @id copy_arrays
 * @title 复制数组
 * @tags 数组 构造 修改
 * @summary 将源数组的前 n 项复制到目标数组。
 * @keywords 拷贝 复制
 * @variant int copy_ints copy_ints_into
 * @variant ll copy_lls copy_lls_into
 * @variant double copy_doubles copy_doubles_into
 * 从 src 复制 n 项到 dst，目标容量至少 n。两个数组不应重叠；指针目标用显式容量接口。
 * @example
 * {{type}} src[] = {1, 2, 3}, dst[3];
 * {{fn1}}(dst, src, 3);
 */
#define copy_ints(array, source, n) \
    copy_ints_into((array), (source), (n), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires values_ints_into
 * @id values_arrays
 * @title 从给定数据构造数组
 * @tags 数组 构造
 * @summary 把给定的数据依次写入数组。
 * @keywords 初始化 参数列表
 * @variant int values_ints values_ints_into
 * @variant ll values_lls values_lls_into
 * @variant double values_doubles values_doubles_into
 * 将参数列表写入数组，返回写入元素数。普通接口要求实际数组；显式容量接口接收源数组、元素数、目标容量。
 * @example
 * {{type}} a[10];
 * int n = {{fn1}}(a, 4, 1, 3);
 */
#define values_ints(array, ...) \
    values_ints_into((array), (const int[]){__VA_ARGS__}, \
    sizeof((const int[]){__VA_ARGS__})/sizeof(int), sizeof(array)/sizeof((array)[0]))
/**
 * @public
 * @requires range_ints_into
 * @id range_arrays
 * @title 生成整数等差序列
 * @tags 数组 整数 构造
 * @summary 生成指定起点、终点和步长的整数序列。
 * @keywords 等差数列 步长 序列 range
 * @variant int range_ints range_ints_into
 * @variant ll range_lls range_lls_into
 * 从 first 开始，每次增加 step，遇到或越过 stop 时停止（不含 stop）。step 不为 0，可为负。返回写入项数，容量须足够。
 * @example
 * {{type}} a[10];
 * int n = {{fn1}}(a, 1, 8, 2); // 1, 3, 5, 7
 */
#define range_ints(array, first, stop, step) \
    range_ints_into((array), (first), (stop), (step), sizeof(array)/sizeof((array)[0]))
