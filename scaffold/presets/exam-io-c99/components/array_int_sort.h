static inline int exam_cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
static inline int exam_cmp_int_desc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x < y) - (x > y);
}
static inline void sort_ints(int *a, int n) {
    exam_check_n(n);
    if (n > 1) qsort(a, (size_t)n, sizeof(*a), exam_cmp_int_asc);
}
static inline void sort_ints_desc(int *a, int n) {
    exam_check_n(n);
    if (n > 1) qsort(a, (size_t)n, sizeof(*a), exam_cmp_int_desc);
}
static inline void bubble_sort_ints(int *a, int n) {
    exam_check_n(n);
    for (int end = n; end > 1; --end) {
        bool changed = false;
        for (int j = 1; j < end; ++j) if (a[j-1] > a[j]) {
            int temp = a[j-1]; a[j-1] = a[j]; a[j] = temp;
            changed = true;
        }
        if (!changed) break;
    }
}
static inline int unique_sorted_ints(int *a, int n) {
    exam_check_n(n);
    if (!n) return 0;
    int write = 1;
    for (int i = 1; i < n; ++i)
        if (a[i] != a[write-1]) a[write++] = a[i];
    return write;
}
static inline int unique_ints(int *a, int n) {
    exam_check_n(n);
    int write = 0;
    for (int i = 0; i < n; ++i) {
        bool found = false;
        for (int j = 0; j < write; ++j)
            if (a[j] == a[i]) { found = true; break; }
        if (!found) a[write++] = a[i];
    }
    return write;
}
