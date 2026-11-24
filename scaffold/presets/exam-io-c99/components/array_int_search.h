/** @category 数组 */
/**
 * @public
 * @requires exam_check_n
 */
/**
 * @id linear_search_ints
 * @title 线性查找（int）
 * @members linear_search_ints
 * @related find_arrays
 * 与 find_ints 行为一致：无需排序，返回首次匹配下标或 -1。
 * @example
 * int a[] = {3, 1, 3};
 * int i = linear_search_ints(a, 3, 3);
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
 */
/**
 * @id find_arrays
 * @title 查找首次出现位置
 * @variant int find_ints
 * @variant ll find_lls
 * @variant double find_doubles
 * 无需排序，返回第一次出现的 0 基下标；不存在返回 -1，O(n)。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * int i = {{fn1}}(a, 4, 3); // 0
 * @note double
 * 浮点相等按精确相等判断。
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
 */
/**
 * @id binary_search_arrays
 * @title 在升序数组中查找
 * @variant int binary_search_ints lower_bound_ints upper_bound_ints
 * @variant ll binary_search_lls lower_bound_lls upper_bound_lls
 * @variant double binary_search_doubles lower_bound_doubles upper_bound_doubles
 * 要求数组升序。binary_search 返回首次匹配下标或 -1；lower_bound 返回首个 >= key 的位置；upper_bound 返回首个 > key 的位置，后两者无满足项时返回 n。O(log n)。
 * @example
 * {{type}} a[] = {1, 3, 3, 5};
 * int i = {{fn1}}(a, 4, 3); // 1
 * int l = {{fn2}}(a, 4, 3); // 1
 * int r = {{fn3}}(a, 4, 3); // 3
 * @note double
 * 使用精确比较；数组需按相同规则升序排列。
 */
static inline int binary_search_ints(const int *a, int n, int key) {
    int i = lower_bound_ints(a, n, key);
    return i < n && a[i] == key ? i : -1;
}
/**
 * @public
 * @requires exam_check_n
 */
/**
 * @id count_arrays
 * @title 统计出现次数
 * @variant int count_ints
 * @variant ll count_lls
 * @variant double count_doubles
 * 无需排序，返回 key 在前 n 项出现的次数。
 * @example
 * {{type}} a[] = {3, 1, 3, 2};
 * int count = {{fn1}}(a, 4, 3); // 2
 * @note double
 * 浮点相等按精确相等判断。
 */
static inline int count_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) if (a[i] == key) ++count;
    return count;
}
