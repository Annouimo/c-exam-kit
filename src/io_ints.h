/**
 * @public
 * @requires exam_io_fail read_int
 */
static inline void read_ints_into(int *array, int count, size_t capacity) {
    if (count < 0 || (size_t)count > capacity)
        exam_io_fail("int array length exceeds capacity");
    if (count && !array) exam_io_fail("null int array");
    for (int i = 0; i < count; ++i) array[i] = read_int();
}
/**
 * @public
 * @requires read_ints_into
 * @id read_arrays
 * @title 读数组
 * @tags 输入 数组
 * @summary 连续读取 n 个数值到已有数组。
 * @pre 普通接口传入实际数组，指针使用显式容量接口。
 * @keywords 数组 输入 容量 批量读入
 * @variant int read_ints read_ints_into
 * @variant ll read_lls read_lls_into
 * @variant double read_doubles read_doubles_into
 * 连续读取 n 个元素，写入已有数组。n 是读取数量，不是容量。普通接口要求实际数组；指针用显式容量的第二个接口，容量单位为元素。
 * @example
 * {{type}} a[100];
 * int n = read_int();
 * {{fn1}}(a, n);
 */
#define read_ints(array, count) \
    read_ints_into((array), (count), sizeof(array) / sizeof((array)[0]))
