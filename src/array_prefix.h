/** @category 数组 */
/**
 * @public
 * @requires exam_failf
 */
static inline void build_prefix_ints_into(const int *a, int n, long long *prefix,
                                           size_t capacity) {
    if (n < 0)
        exam_failf("algorithm", "build_prefix_ints", "n=%d out of range (expected n >= 0)", n);
    if (!prefix || (n && !a))
        exam_failf("algorithm", "build_prefix_ints", "%s must not be NULL",
                   prefix ? "input" : "destination");
    if ((unsigned long long)n >= capacity)
        exam_failf("algorithm", "build_prefix_ints",
                   "capacity=%zu too small (need %llu elements)", capacity, (unsigned long long)n + 1);
    prefix[0] = 0;
    for (int i = 0; i < n; ++i) prefix[i+1] = prefix[i] + (long long)a[i];
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline long long prefix_range_sum(const long long *prefix, int begin,
                                        int end) {
    if (begin < 0 || end < begin) exam_algo_fail("invalid prefix range");
    return prefix[end] - prefix[begin];
}
/* 0-based half-open [begin,end), result array contains n+1 items. */
/**
 * @public
 * @requires build_prefix_ints_into
 */
/**
 * @id prefix_sum
 * @title 多次区间求和（前缀和）
 * @members build_prefix_ints prefix_range_sum build_prefix_ints_into
 * @related difference_array
 * 先建表，再查询 [l,r)（不含 r）。n 须非负，prefix 指向至少 n+1 项的 long long 数组。n 大于 0 时输入数组也不能为空指针。容量不足会显示实际容量和所需的元素数。建表 O(n)，单次查询 O(1)。
 * @example
 * int a[] = {2, 4, 6, 8};
 * long long prefix[5];
 * build_prefix_ints(a, 4, prefix);
 * long long s = prefix_range_sum(prefix, 1, 3); // 10
 */
#define build_prefix_ints(a, n, prefix) \
    build_prefix_ints_into((a), (n), (prefix), sizeof(prefix)/sizeof((prefix)[0]))
