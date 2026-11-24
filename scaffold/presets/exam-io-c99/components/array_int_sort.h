/** @category 数组 */
static inline int exam_cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}
static inline int exam_cmp_int_desc(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x < y) - (x > y);
}
/**
 * @public
 * @requires exam_check_n exam_cmp_int_asc
 * @category 数组
 * @title 排序、反转、交换
 * @members sort_ints sort_lls sort_doubles sort_ints_desc sort_lls_desc sort_doubles_desc reverse_ints reverse_lls reverse_doubles swap_ints
 * `sort_ints(a, n)` 升序，`sort_ints_desc(a, n)` 降序，`reverse_ints(a, n)` 反转，`swap_ints(a, i, j)` 交换，均原地修改
 */
static inline void sort_ints(int *a, int n) {
    exam_check_n(n);
    if (n > 1) qsort(a, (size_t)n, sizeof(*a), exam_cmp_int_asc);
}
/**
 * @public
 * @requires exam_check_n exam_cmp_int_desc
 */
static inline void sort_ints_desc(int *a, int n) {
    exam_check_n(n);
    if (n > 1) qsort(a, (size_t)n, sizeof(*a), exam_cmp_int_desc);
}
/**
 * @public
 * @requires exam_check_n
 */
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
/**
 * @public
 * @requires exam_check_n
 */
static inline int unique_sorted_ints(int *a, int n) {
    exam_check_n(n);
    if (!n) return 0;
    int write = 1;
    for (int i = 1; i < n; ++i)
        if (a[i] != a[write-1]) a[write++] = a[i];
    return write;
}
/**
 * @public
 * @requires exam_check_n
 */
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
