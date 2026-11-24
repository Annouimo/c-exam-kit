/** @category 数组 */
/**
 * @public
 * @requires exam_check_n
 * @category 数组
 * @title 统计量
 * @members sum_ints sum_lls sum_doubles mean_ints mean_lls mean_doubles variance_ints variance_lls variance_doubles stddev_ints stddev_lls stddev_doubles median_ints median_lls median_doubles min_ints min_lls min_doubles max_ints max_lls max_doubles argmin_ints argmin_lls argmin_doubles argmax_ints argmax_lls argmax_doubles
 * `sum_ints(a, n)` 返回 `long long`。`mean_ints`、`variance_ints`、`stddev_ints`、`median_ints` 返回 `double`，方差除以 `n`。`min_ints`、`max_ints` 返回值，`argmin_ints`、`argmax_ints` 返回首次出现的下标
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
