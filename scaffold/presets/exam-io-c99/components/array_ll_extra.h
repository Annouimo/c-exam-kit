/* Complements array_ll_basic; all queries preserve the original array. */
static inline int values_lls_into(long long *destination,
                                  const long long *items, size_t count,
                                  size_t capacity) {
    if (count > (size_t)INT_MAX || count > capacity ||
        (count && (!destination || !items)))
        exam_algo_fail("values_lls output capacity too small");
    if (count) memmove(destination, items, count * sizeof(long long));
    return (int)count;
}
static inline int range_lls_into(long long *destination,
                                 long long first, long long stop,
                                 long long step, size_t capacity) {
    if (!step) exam_algo_fail("range_lls step must not be zero");
    int n = 0;
    long long current = first;
    while ((step > 0 && current < stop) ||
           (step < 0 && current > stop)) {
        if ((size_t)n >= capacity || n == INT_MAX || !destination)
            exam_algo_fail("range_lls output capacity too small");
        destination[n++] = current;
        if (step > 0 && current > LLONG_MAX - step) break;
        if (step < 0 && current < LLONG_MIN - step) break;
        current += step;
    }
    return n;
}
static inline int exam_cmp_ll_desc(const void *pa, const void *pb) {
    long long a = *(const long long *)pa, b = *(const long long *)pb;
    return (a < b) - (a > b);
}
static inline void sort_lls_desc(long long *a, int n) {
    exam_check_n(n);
    if (n < 2) return;
    /* qsort comparator in descending order, without signed subtraction. */
    qsort(a, (size_t)n, sizeof(*a), exam_cmp_ll_desc);
}
static inline int count_lls(const long long *a, int n, long long key) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) if (a[i] == key) ++count;
    return count;
}
static inline int binary_search_lls(const long long *a, int n,
                                    long long key) {
    int pos = lower_bound_lls(a, n, key);
    return pos < n && a[pos] == key ? pos : -1;
}
static inline int argmin_lls(const long long *a, int n) {
    if (n <= 0) exam_algo_fail("argmin_lls requires nonempty array");
    int result = 0;
    for (int i = 1; i < n; ++i) if (a[i] < a[result]) result = i;
    return result;
}
static inline int argmax_lls(const long long *a, int n) {
    if (n <= 0) exam_algo_fail("argmax_lls requires nonempty array");
    int result = 0;
    for (int i = 1; i < n; ++i) if (a[i] > a[result]) result = i;
    return result;
}
static inline double mean_lls(const long long *a, int n) {
    if (n <= 0) exam_algo_fail("mean_lls requires nonempty array");
    long double total = 0;
    for (int i = 0; i < n; ++i) total += (long double)a[i];
    return (double)(total / n);
}
static inline double variance_lls(const long long *a, int n) {
    if (n <= 0) exam_algo_fail("variance_lls requires nonempty array");
    long double total = 0;
    for (int i = 0; i < n; ++i) total += (long double)a[i];
    long double mean = total / n, squared = 0;
    for (int i = 0; i < n; ++i) {
        long double d = (long double)a[i] - mean;
        squared += d * d;
    }
    return (double)(squared / n);
}
static inline double stddev_lls(const long long *a, int n) {
    return sqrt(variance_lls(a, n));
}
static inline double median_lls(const long long *a, int n) {
    if (n <= 0) exam_algo_fail("median_lls requires nonempty array");
    if ((size_t)n > SIZE_MAX / sizeof(long long))
        exam_algo_fail("median_lls allocation overflow");
    long long *temp = (long long *)malloc((size_t)n * sizeof(*temp));
    if (!temp) exam_algo_fail("median_lls allocation failed");
    memcpy(temp, a, (size_t)n * sizeof(*temp));
    qsort(temp, (size_t)n, sizeof(*temp), exam_cmp_ll);
    double result = (n & 1) ? (double)temp[n / 2]
        : (double)(((long double)temp[n / 2 - 1] +
                    (long double)temp[n / 2]) / 2);
    free(temp);
    return result;
}
static inline int unique_sorted_lls(long long *a, int n) {
    exam_check_n(n);
    if (n == 0) return 0;
    int count = 1;
    for (int i = 1; i < n; ++i)
        if (a[i] != a[count - 1]) a[count++] = a[i];
    return count;
}
static inline int unique_lls(long long *a, int n) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) {
        int j = 0;
        while (j < count && a[j] != a[i]) ++j;
        if (j == count) a[count++] = a[i];
    }
    return count;
}
#define values_lls(array, ...) \
    values_lls_into((array), (const long long[]){__VA_ARGS__}, \
        sizeof((const long long[]){__VA_ARGS__}) / sizeof(long long), \
        sizeof(array) / sizeof((array)[0]))
#define range_lls(array, first, stop, step) \
    range_lls_into((array), (first), (stop), (step), \
                   sizeof(array) / sizeof((array)[0]))
