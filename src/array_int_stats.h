/**
 * @public
 * @requires exam_check_n
 * @id sum_mean_arrays
 * @title 求和与平均值
 * @tags 数组 统计
 * @summary 计算数组元素之和或算术平均数。
 * @keywords 总和 求和 平均数
 * @variant int sum_ints mean_ints
 * @variant ll sum_lls mean_lls
 * @variant double sum_doubles mean_doubles
 * 求前 n 项之和与算术平均数。平均值要求 n > 0。整数求和返回 long long，浮点求和返回 double；平均值返回 double。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * {{fn1}}(a, 4);
 * double avg = {{fn2}}(a, 4);
 * @note double
 * 统计不接受 NaN 和无穷大。
 */
static inline long long sum_ints(const int *a, int n) {
    exam_check_n(n);
    long long total = 0;
    for (int i = 0; i < n; ++i) total += a[i];
    return total;
}
/**
 * @public
 * @requires exam_algo_fail
 * @id extrema_arrays
 * @title 最大值与最小值
 * @tags 数组 统计
 * @summary 计算数组最大值、最小值及其首次出现下标。
 * @keywords 最值 最大 最小 下标
 * @variant int min_ints max_ints argmin_ints argmax_ints
 * @variant ll min_lls max_lls argmin_lls argmax_lls
 * @variant double min_doubles max_doubles argmin_doubles argmax_doubles
 * n 必须大于 0。min/max 返回值，argmin/argmax 返回首次出现的下标。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * {{type}} lo = {{fn1}}(a, 4), hi = {{fn2}}(a, 4);
 * int i = {{fn3}}(a, 4), j = {{fn4}}(a, 4);
 * @note double
 * 统计不接受 NaN 和无穷大。
 */
static inline int min_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("min_ints requires nonempty array");
    int value = a[0];
    for (int i = 1; i < n; ++i) if (a[i] < value) value = a[i];
    return value;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int max_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("max_ints requires nonempty array");
    int value = a[0];
    for (int i = 1; i < n; ++i) if (a[i] > value) value = a[i];
    return value;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int argmin_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("argmin_ints requires nonempty array");
    int pos = 0;
    for (int i = 1; i < n; ++i) if (a[i] < a[pos]) pos = i;
    return pos;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int argmax_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("argmax_ints requires nonempty array");
    int pos = 0;
    for (int i = 1; i < n; ++i) if (a[i] > a[pos]) pos = i;
    return pos;
}
/**
 * @public
 * @requires exam_algo_fail sum_ints
 */
static inline double mean_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("mean_ints requires nonempty array");
    return (double)sum_ints(a, n) / n;
}
/**
 * @public
 * @requires exam_algo_fail sum_ints
 * @id variance_arrays
 * @title 方差与标准差
 * @tags 数组 统计
 * @summary 计算总体方差或标准差。
 * @pre n > 0。
 * @return 总体方差或标准差，类型为 double。
 * @keywords 方差 标准差 离散程度
 * @variant int variance_ints stddev_ints
 * @variant ll variance_lls stddev_lls
 * @variant double variance_doubles stddev_doubles
 * 计算总体方差（除以 n）和标准差，n > 0，返回 double。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * double v = {{fn1}}(a, 4);
 * double s = {{fn2}}(a, 4);
 */
static inline double variance_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("variance_ints requires nonempty array");
    long double mean = (long double)sum_ints(a, n)/n;
    long double sum = 0;
    for (int i = 0; i < n; ++i) {
        long double d = (long double)a[i] - mean;
        sum += d*d;
    }
    return (double)(sum/n);
}
/**
 * @public
 * @requires variance_ints
 */
static inline double stddev_ints(const int *a, int n) {
    return sqrt(variance_ints(a, n));
}
/**
 * @public
 * @requires exam_algo_fail exam_cmp_int_asc
 * @id median_arrays
 * @title 中位数
 * @tags 数组 统计
 * @summary 计算数组中位数，不改变原数组。
 * @pre n > 0。
 * @keywords 中间值 中位数
 * @variant int median_ints
 * @variant ll median_lls
 * @variant double median_doubles
 * 返回中位数，偶数项取中间两项平均；n > 0，不改变原数组。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * double m = {{fn1}}(a, 4);
 */
static inline double median_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("median_ints requires nonempty array");
    if ((size_t)n > SIZE_MAX / sizeof(int))
        exam_algo_fail("median_ints capacity overflow");
    int *copy = (int *)malloc((size_t)n*sizeof(int));
    if (!copy) exam_algo_fail("median_ints allocation failed");
    memcpy(copy, a, (size_t)n*sizeof(int));
    qsort(copy, (size_t)n, sizeof(int), exam_cmp_int_asc);
    double result;
    if (n & 1) result = copy[n/2];
    else result = ((double)copy[n/2-1] + (double)copy[n/2])/2.0;
    free(copy);
    return result;
}
