/** @category 数组 */
/** @requires Record exam_check_n */
static inline int exam_find_records_by(const Record *records, int n,
    size_t element_size, const Record *key,
    int (*compare)(const Record *, const Record *)) {
    exam_check_n(n);
    const unsigned char *bytes = (const unsigned char *)records;
    for (int i = 0; i < n; ++i) {
        const Record *record = (const Record *)(bytes + (size_t)i * element_size);
        if (compare(record, key) == 0) return i;
    }
    return -1;
}

/** @requires Record exam_check_n */
static inline int exam_binary_search_records_by(const Record *records, int n,
    size_t element_size, const Record *key,
    int (*compare)(const Record *, const Record *)) {
    exam_check_n(n);
    const unsigned char *bytes = (const unsigned char *)records;
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right - left) / 2;
        const Record *record = (const Record *)(bytes + (size_t)mid * element_size);
        if (compare(record, key) < 0) left = mid + 1;
        else right = mid;
    }
    if (left == n) return -1;
    const Record *record = (const Record *)(bytes + (size_t)left * element_size);
    return compare(record, key) == 0 ? left : -1;
}

/**
 * @public
 * @requires exam_find_records_by
 * @category 数组
 * @title 结构体查找
 * @members find_records_by binary_search_records_by
 * `find_records_by(a, n, &key, compare)` 按当前顺序扫描。`binary_search_records_by(a, n, &key, compare)` 要求数组已按 `compare` 排序。 两者返回首个匹配的下标，找不到返回 `-1`。比较函数接收数组元素和查找键，类型均为 `const Record *`。初始化比较函数会读到的所有字段。排序与查找规则相同时，可以直接传入 `record_cmp`。
 * @example struct Record { int id; };
 * @example
 * @example int record_cmp(const Record *a, const Record *b) {
 * @example     return (a->id > b->id) - (a->id < b->id);
 * @example }
 * @example
 * @example int main(void) {
 * @example     Record a[] = {{30}, {10}, {30}};
 * @example     Record key = {30};
 * @example     print_int(find_records_by(a, 3, &key, record_cmp), "\n");  // 0
 * @example     sort_records(a, 3);
 * @example     print_int(binary_search_records_by(a, 3, &key, record_cmp), "\n");  // 1
 * @example     return 0;
 * @example }
 */
#define find_records_by(array, n, key, compare) \
    exam_find_records_by((array), (n), sizeof((array)[0]), (key), (compare))

/**
 * @public
 * @requires exam_binary_search_records_by
 */
#define binary_search_records_by(array, n, key, compare) \
    exam_binary_search_records_by((array), (n), sizeof((array)[0]), (key), (compare))
