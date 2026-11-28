/**
 * @public
 * @requires exam_check_n
 * @id linear_search_ints
 * @title 线性查找（int）
 * @tags 数组 整数 查找
 * @summary 在线性扫描中找到整数第一次出现的位置。
 * @keywords 线性搜索 首次匹配
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
 * @id find_arrays
 * @title 查找首次出现位置
 * @tags 数组 查找
 * @summary 在无需排序的数组中查找目标值首次出现的位置。
 * @return 首次匹配的 0 基下标；不存在时返回 -1。
 * @complexity O(n)
 * @keywords 搜索 首次匹配 定位
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
 * @id lower_bound_arrays
 * @title 查找下界（首个 ≥ 目标）
 * @tags 数组 查找 二分
 * @summary 在升序数组中找出第一个不小于目标值的位置。
 * @param a 升序数组。
 * @param n 参与查找的元素数量。
 * @param key 目标值。
 * @return 首个不小于 key 的下标；不存在时返回 n。
 * @pre 数组按升序排列。
 * @complexity O(log n)
 * @keywords 下界 插入位置 大于等于
 * @variant int lower_bound_ints
 * @variant ll lower_bound_lls
 * @variant double lower_bound_doubles
 * @related binary_search_arrays upper_bound_arrays
 * 升序数组中，返回首个 >= key 的位置；没有满足项时返回 n。
 * @example
 * {{type}} a[] = {1, 3, 3, 5};
 * int i = {{fn1}}(a, 4, 3); // 1
 * @note double
 * 使用精确比较；数组需按相同规则升序排列。
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
 * @id upper_bound_arrays
 * @title 查找上界（首个 > 目标）
 * @tags 数组 查找 二分
 * @summary 在升序数组中找出第一个大于目标值的位置。
 * @param a 升序数组。
 * @param n 参与查找的元素数量。
 * @param key 目标值。
 * @return 首个大于 key 的下标；不存在时返回 n。
 * @pre 数组按升序排列。
 * @complexity O(log n)
 * @keywords 上界 插入位置 严格大于
 * @variant int upper_bound_ints
 * @variant ll upper_bound_lls
 * @variant double upper_bound_doubles
 * @related binary_search_arrays lower_bound_arrays
 * 升序数组中，返回首个 > key 的位置；没有满足项时返回 n。
 * @example
 * {{type}} a[] = {1, 3, 3, 5};
 * int i = {{fn1}}(a, 4, 3); // 3
 * @note double
 * 使用精确比较；数组需按相同规则升序排列。
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
 * @id binary_search_arrays
 * @title 二分查找
 * @tags 数组 查找 二分
 * @summary 在升序数组中查找目标值首次出现的位置。
 * @param a 待查找的数组。
 * @param n 参与查找的元素数量。
 * @param key 目标值。
 * @return 首次匹配的 0 基下标；找不到返回 -1。
 * @pre 数组按升序排列。
 * @complexity O(log n)
 * @keywords 有序数组 搜索 首次匹配
 * @variant int binary_search_ints
 * @variant ll binary_search_lls
 * @variant double binary_search_doubles
 * @related lower_bound_arrays upper_bound_arrays
 * 要求数组升序；返回首次匹配下标或 -1。
 * @example
 * {{type}} a[] = {1, 3, 3, 5};
 * int i = {{fn1}}(a, 4, 3); // 1
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
 * @id count_arrays
 * @title 统计出现次数
 * @tags 数组 统计 查找
 * @summary 统计目标值在数组前 n 项出现的次数。
 * @return 匹配的元素数量。
 * @keywords 计数 出现次数
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
