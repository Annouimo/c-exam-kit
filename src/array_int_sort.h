
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
 * @id sort_arrays
 * @title 数组排序
 * @tags 数组 排序
 * @summary 原地对数组进行升序或降序排序。
 * @keywords 升序 降序 排列
 * @variant int sort_ints sort_ints_desc
 * @variant ll sort_lls sort_lls_desc
 * @variant double sort_doubles sort_doubles_desc
 * 原地排序前 n 项。第一个接口升序，第二个接口降序。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * {{fn1}}(a, 4);
 * {{fn2}}(a, 4);
 * @note double
 * 不接受 NaN 和无穷大。
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
 * @id bubble_sort_ints
 * @title 冒泡排序
 * @tags 数组 整数 排序
 * @summary 使用冒泡算法对整数数组原地升序排序。
 * @keywords 冒泡 升序
 * @members bubble_sort_ints
 * @related sort_arrays
 * 对 int 数组原地升序冒泡排序，时间 O(n²)。普通排序优先使用 sort_ints。
 * @example
 * int a[] = {3, 1, 2};
 * bubble_sort_ints(a, 3);
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
 * @id unique_arrays
 * @title 数组去重
 * @tags 数组 修改 统计
 * @summary 原地移除重复元素并返回新长度。
 * @keywords 去重 重复 不同元素
 * @variant int unique_ints unique_sorted_ints
 * @variant ll unique_lls unique_sorted_lls
 * @variant double unique_doubles unique_sorted_doubles
 * 原地去重并返回新长度，只使用数组前新长度项。第一种保持首次出现顺序（O(n²)）；第二种要求已经排序（O(n)）。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * int n = {{fn1}}(a, 4);
 * // n = 3
 * @note double
 * 按精确相等去重，不接受 NaN 和无穷大。
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
