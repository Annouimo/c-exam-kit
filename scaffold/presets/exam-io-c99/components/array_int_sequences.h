/** @category 数组 */
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id max_subarray_sum_ints
 * @title 最大连续子段和
 * @members max_subarray_sum_ints
 * 返回最大非空连续子段和，n > 0。
 * @example
 * int a[] = {-2, 3, 4, -1};
 * long long s = max_subarray_sum_ints(a, 4); // 7
 */
static inline long long max_subarray_sum_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("max_subarray_sum_ints requires nonempty array");
    long long best = a[0], running = a[0];
    for (int i = 1; i < n; ++i) {
        running = running > 0 ? running + a[i] : a[i];
        if (running > best) best = running;
    }
    return best;
}
/**
 * @public
 * @requires exam_check_n
 * @category 数组
 */
/**
 * @id longest_runs
 * @title 最长连续段
 * @members longest_equal_run_ints longest_plus_one_run_ints
 * 分别返回最长相等连续段和逐项增加 1 的连续段长度。
 * @example
 * int a[] = {1, 1, 1, 2, 3};
 * int eq = longest_equal_run_ints(a, 5); // 3
 * int inc = longest_plus_one_run_ints(a, 5); // 3
 */
static inline int longest_equal_run_ints(const int *a, int n) {
    exam_check_n(n);
    if (n == 0) return 0;
    int best = 1, run = 1;
    for (int i = 1; i < n; ++i) {
        run = a[i] == a[i-1] ? run+1 : 1;
        if (run > best) best = run;
    }
    return best;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int longest_plus_one_run_ints(const int *a, int n) {
    exam_check_n(n);
    if (n == 0) return 0;
    int best = 1, run = 1;
    for (int i = 1; i < n; ++i) {
        run = (long long)a[i] == (long long)a[i-1] + 1 ? run+1 : 1;
        if (run > best) best = run;
    }
    return best;
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id max_forward_diff_ints
 * @title 前项减后项的最大差
 * @members max_forward_diff_ints
 * 求 i < j 时 a[i] - a[j] 的最大值，至少两项。
 * @example
 * int a[] = {5, 2, 4};
 * long long d = max_forward_diff_ints(a, 3); // 3
 */
static inline long long max_forward_diff_ints(const int *a, int n) {
    if (n < 2) exam_algo_fail("max_forward_diff_ints requires at least 2 values");
    int max_before = a[0];
    long long best = (long long)a[0] - a[1];
    for (int i = 1; i < n; ++i) {
        long long diff = (long long)max_before - a[i];
        if (diff > best) best = diff;
        if (a[i] > max_before) max_before = a[i];
    }
    return best;
}
