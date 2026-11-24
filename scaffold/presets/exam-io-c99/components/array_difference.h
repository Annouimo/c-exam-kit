/** @category 数组 */
/* A difference array of long long values, length at least n+1. */
/**
 * @public
 * @requires exam_algo_fail
 * @title 多次区间加值
 * @members build_diff_ints build_diff_ints_into diff_range_add diff_to_lls diff_to_lls_into
 * 先用 build_diff_ints 建差分表（n + 1 项），再用 diff_range_add 更新 [l, r)。最后用 diff_to_lls 还原到长度为 n 的数组。
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
 */
#define build_diff_ints(a,n,difference) \
    build_diff_ints_into((a),(n),(difference),sizeof(difference)/sizeof((difference)[0]))
/**
 * @public
 * @requires diff_to_lls_into
 */
#define diff_to_lls(difference,n,out) \
    diff_to_lls_into((difference),(n),(out),sizeof(out)/sizeof((out)[0]))
