static inline void build_prefix_ints_into(const int *a, int n, long long *prefix,
                                           size_t capacity) {
    if (n < 0 || (size_t)n >= capacity || !prefix)
        exam_algo_fail("prefix array capacity must be at least n+1");
    prefix[0] = 0;
    for (int i = 0; i < n; ++i) prefix[i+1] = prefix[i] + (long long)a[i];
}
static inline long long prefix_range_sum(const long long *prefix, int begin,
                                        int end) {
    if (begin < 0 || end < begin) exam_algo_fail("invalid prefix range");
    return prefix[end] - prefix[begin];
}
/* 0-based half-open [begin,end), result array contains n+1 items. */
#define build_prefix_ints(a, n, prefix) \
    build_prefix_ints_into((a), (n), (prefix), sizeof(prefix)/sizeof((prefix)[0]))
