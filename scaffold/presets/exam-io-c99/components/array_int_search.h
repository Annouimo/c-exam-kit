/** @category 数组 */
/**
 * @public
 * @requires exam_check_n
 */
static inline int linear_search_ints(const int *a, int n, int key) {
    exam_check_n(n);
    for (int i = 0; i < n; ++i) if (a[i] == key) return i;
    return -1;
}
/**
 * @public
 * @requires linear_search_ints
 * @category 数组
 * @title 找位置、数次数
 * @members find_ints find_lls find_doubles count_ints count_lls count_doubles
 * `find_ints(a, n, x)` 返回第一次出现的下标，找不到返回 `-1`。`count_ints(a, n, x)` 返回出现次数
 */
static inline int find_ints(const int *a, int n, int key) {
    return linear_search_ints(a, n, key);
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int lower_bound_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right-left)/2;
        if (a[mid] < key) left = mid+1;
        else right = mid;
    }
    return left;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int upper_bound_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right-left)/2;
        if (a[mid] <= key) left = mid+1;
        else right = mid;
    }
    return left;
}
/**
 * @public
 * @requires lower_bound_ints
 * @category 数组
 * @title 在升序数组中查找
 * @members binary_search_ints binary_search_lls binary_search_doubles lower_bound_ints lower_bound_lls lower_bound_doubles upper_bound_ints upper_bound_lls upper_bound_doubles
 * `binary_search_ints(a, n, x)` 找第一次出现，找不到返回 `-1`。`lower_bound_ints(a, n, x)` 找首个 `>= x`，`upper_bound_ints(a, n, x)` 找首个 `> x`，两者找不到均返回 `n`
 */
static inline int binary_search_ints(const int *a, int n, int key) {
    int i = lower_bound_ints(a, n, key);
    return i < n && a[i] == key ? i : -1;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int count_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) if (a[i] == key) ++count;
    return count;
}
