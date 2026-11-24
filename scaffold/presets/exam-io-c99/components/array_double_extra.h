/** @category 数组 */
/* Floating array comparisons use exact ==; NaN/infinity are not supported. */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int values_doubles_into(double *destination,
                                      const double *items, size_t count,
                                      size_t capacity) {
    if (count > (size_t)INT_MAX || count > capacity ||
        (count && (!destination || !items)))
        exam_algo_fail("values_doubles output capacity too small");
    if (count) memmove(destination, items, count * sizeof(double));
    return (int)count;
}
static inline int exam_cmp_double_desc(const void *pa, const void *pb) {
    double a = *(const double *)pa, b = *(const double *)pb;
    return (a < b) - (a > b);
}
/**
 * @public
 * @requires exam_doubles_finite exam_cmp_double_desc
 */
static inline void sort_doubles_desc(double *a, int n) {
    exam_doubles_finite(a, n);
    if (n > 1) qsort(a, (size_t)n, sizeof(*a), exam_cmp_double_desc);
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int count_doubles(const double *a, int n, double key) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) if (a[i] == key) ++count;
    return count;
}
/**
 * @public
 * @requires lower_bound_doubles
 */
static inline int binary_search_doubles(const double *a, int n, double key) {
    int pos = lower_bound_doubles(a, n, key);
    return pos < n && a[pos] == key ? pos : -1;
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline int argmin_doubles(const double *a, int n) {
    if (n <= 0) exam_algo_fail("argmin_doubles requires nonempty array");
    exam_doubles_finite(a, n);
    int result = 0;
    for (int i = 1; i < n; ++i) if (a[i] < a[result]) result = i;
    return result;
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline int argmax_doubles(const double *a, int n) {
    if (n <= 0) exam_algo_fail("argmax_doubles requires nonempty array");
    exam_doubles_finite(a, n);
    int result = 0;
    for (int i = 1; i < n; ++i) if (a[i] > a[result]) result = i;
    return result;
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline double variance_doubles(const double *a, int n) {
    if (n <= 0) exam_algo_fail("variance_doubles requires nonempty array");
    exam_doubles_finite(a, n);
    long double total = 0;
    for (int i = 0; i < n; ++i) total += (long double)a[i];
    long double mean = total / n, squared = 0;
    for (int i = 0; i < n; ++i) {
        long double delta = (long double)a[i] - mean;
        squared += delta * delta;
    }
    double result = (double)(squared / n);
    if (!isfinite(result)) exam_algo_fail("variance_doubles overflow");
    return result;
}
/**
 * @public
 * @requires variance_doubles
 */
static inline double stddev_doubles(const double *a, int n) {
    return sqrt(variance_doubles(a, n));
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite exam_cmp_double
 */
static inline double median_doubles(const double *a, int n) {
    if (n <= 0) exam_algo_fail("median_doubles requires nonempty array");
    exam_doubles_finite(a, n);
    if ((size_t)n > SIZE_MAX / sizeof(double))
        exam_algo_fail("median_doubles allocation overflow");
    double *temp = (double *)malloc((size_t)n * sizeof(*temp));
    if (!temp) exam_algo_fail("median_doubles allocation failed");
    memcpy(temp, a, (size_t)n * sizeof(*temp));
    qsort(temp, (size_t)n, sizeof(*temp), exam_cmp_double);
    double result = (n & 1) ? temp[n / 2]
        : (double)(((long double)temp[n / 2 - 1] +
                    (long double)temp[n / 2]) / 2);
    free(temp);
    return result;
}
/**
 * @public
 * @requires exam_doubles_finite
 */
static inline int unique_sorted_doubles(double *a, int n) {
    exam_doubles_finite(a, n);
    if (n == 0) return 0;
    int count = 1;
    for (int i = 1; i < n; ++i)
        if (a[i] != a[count - 1]) a[count++] = a[i];
    return count;
}
/**
 * @public
 * @requires exam_doubles_finite
 */
static inline int unique_doubles(double *a, int n) {
    exam_doubles_finite(a, n);
    int count = 0;
    for (int i = 0; i < n; ++i) {
        int j = 0;
        while (j < count && a[j] != a[i]) ++j;
        if (j == count) a[count++] = a[i];
    }
    return count;
}
/**
 * @public
 * @requires values_doubles_into
 */
#define values_doubles(array, ...) \
    values_doubles_into((array), (const double[]){__VA_ARGS__}, \
        sizeof((const double[]){__VA_ARGS__}) / sizeof(double), \
        sizeof(array) / sizeof((array)[0]))
