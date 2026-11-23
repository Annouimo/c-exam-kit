static inline long long sum_ints(const int *a, int n) {
    exam_check_n(n);
    long long total = 0;
    for (int i = 0; i < n; ++i) total += a[i];
    return total;
}
static inline int min_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("min_ints requires nonempty array");
    int value = a[0];
    for (int i = 1; i < n; ++i) if (a[i] < value) value = a[i];
    return value;
}
static inline int max_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("max_ints requires nonempty array");
    int value = a[0];
    for (int i = 1; i < n; ++i) if (a[i] > value) value = a[i];
    return value;
}
static inline int argmin_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("argmin_ints requires nonempty array");
    int pos = 0;
    for (int i = 1; i < n; ++i) if (a[i] < a[pos]) pos = i;
    return pos;
}
static inline int argmax_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("argmax_ints requires nonempty array");
    int pos = 0;
    for (int i = 1; i < n; ++i) if (a[i] > a[pos]) pos = i;
    return pos;
}
static inline double mean_ints(const int *a, int n) {
    if (n <= 0) exam_algo_fail("mean_ints requires nonempty array");
    return (double)sum_ints(a, n) / n;
}
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
static inline double stddev_ints(const int *a, int n) {
    return sqrt(variance_ints(a, n));
}
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
