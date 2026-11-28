
/* A difference array of long long values, length at least n+1. */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void build_diff_ints_into(const int *a, int n,
                                        long long *difference, size_t capacity) {
    if (n < 0 || (size_t)n >= capacity)
        exam_algo_fail("difference array capacity must be at least n+1");
    for (int i = 0; i < n; ++i)
        difference[i] = (long long)a[i] - (i ? (long long)a[i-1] : 0);
    difference[n] = 0;
}
/**
 * @public
 * @requires exam_algo_fail exam_add_checked_ll exam_sub_checked_ll
 */
static inline void diff_range_add(long long *difference, int n,
                                  int begin, int end, long long delta) {
    if (n < 0 || begin < 0 || begin > end || end > n)
        exam_algo_fail("invalid difference range");
    if (begin == end) return;
    difference[begin] = exam_add_checked_ll(difference[begin], delta);
    difference[end] = exam_sub_checked_ll(difference[end], delta);
}
/**
 * @public
 * @requires exam_algo_fail exam_add_checked_ll
 */
static inline void diff_to_lls_into(const long long *difference, int n,
                                    long long *out, size_t capacity) {
    if (n < 0 || (size_t)n > capacity)
        exam_algo_fail("difference output array too small");
    long long current = 0;
    for (int i = 0; i < n; ++i) {
        current = exam_add_checked_ll(current, difference[i]);
        out[i] = current;
    }
}
/**
 * @public
 * @requires build_diff_ints_into
 * @id difference_array
 * @title 多次区间加值（差分）
 * @tags 数组 区间 修改
 * @summary 使用差分表对多个区间加值并还原数组。
 * @pre 差分表至少 n+1 项，输出数组至少 n 项。
 * @complexity 单次区间更新 O(1)。
 * @keywords 差分 区间加值 批量更新
 * @members build_diff_ints diff_range_add diff_to_lls build_diff_ints_into diff_to_lls_into
 * @related prefix_sum
 * 差分表至少 n+1 项 long long。先建表，再对 [l,r) 加 delta，最后还原到 n 项 long long 数组。区间更新 O(1)。
 * @example
 * int a[] = {2, 4, 6, 8};
 * long long diff[5], out[4];
 * build_diff_ints(a, 4, diff);
 * diff_range_add(diff, 4, 1, 3, 10);
 * diff_to_lls(diff, 4, out); // 2, 14, 16, 8
 */
#define build_diff_ints(a,n,difference) \
    build_diff_ints_into((a),(n),(difference),sizeof(difference)/sizeof((difference)[0]))
/**
 * @public
 * @requires diff_to_lls_into
 */
#define diff_to_lls(difference,n,out) \
    diff_to_lls_into((difference),(n),(out),sizeof(out)/sizeof((out)[0]))
