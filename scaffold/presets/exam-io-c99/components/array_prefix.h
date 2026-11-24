/** @category 数组 */
/**
 * @public
 * @requires exam_algo_fail
 * @title 多次区间求和
 * @members build_prefix_ints build_prefix_ints_into prefix_range_sum
 * 先建前缀和，再用 prefix_range_sum(prefix, l, r) 查询 [l, r)。prefix 需要 n + 1 项。
 * @example int a[] = {2, 4, 6, 8};
 * @example long long prefix[5];
 * @example build_prefix_ints(a, 4, prefix);
 * @example long long sum = prefix_range_sum(prefix, 1, 3); // 4 + 6
 */
static inline void build_prefix_ints_into(const int *a, int n, long long *prefix,
                                           size_t capacity) {
    if (n < 0 || (size_t)n >= capacity || !prefix)
        exam_algo_fail("prefix array capacity must be at least n+1");
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
#define build_prefix_ints(a, n, prefix) \
    build_prefix_ints_into((a), (n), (prefix), sizeof(prefix)/sizeof((prefix)[0]))
