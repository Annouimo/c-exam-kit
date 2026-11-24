/** @category 数组 */
/** @requires Record */
static inline void exam_swap_records(Record *records, size_t a, size_t b, size_t size) {
    unsigned char *bytes = (unsigned char *)records;
    for (size_t i = 0; i < size; ++i) {
        unsigned char t = bytes[(size_t)a * size + i];
        bytes[(size_t)a * size + i] = bytes[(size_t)b * size + i];
        bytes[(size_t)b * size + i] = t;
    }
}
/** @requires Record exam_swap_records */
static inline void exam_sift_records(Record *records, size_t root, size_t n, size_t size,
                                     int (*compare)(const Record *, const Record *)) {
    unsigned char *bytes = (unsigned char *)records;
    while (root < n / 2) {
        size_t child = root * 2 + 1;
        if (child + 1 < n &&
            compare((const Record *)(bytes + (size_t)child * size),
                    (const Record *)(bytes + (size_t)(child + 1) * size)) < 0)
            ++child;
        if (compare((const Record *)(bytes + (size_t)root * size),
                    (const Record *)(bytes + (size_t)child * size)) >= 0) break;
        exam_swap_records(records, root, child, size);
        root = child;
    }
}
/** @requires Record exam_check_n exam_algo_fail exam_sift_records exam_swap_records */
static inline void exam_sort_records(Record *records, int n, size_t element_size,
                                     int (*compare)(const Record *, const Record *)) {
    exam_check_n(n);
    if (n < 2) return;
    if (!records || !element_size || !compare)
        exam_algo_fail("invalid record array or comparator");
    size_t count = (size_t)n;
    for (size_t i = count / 2; i > 0; --i)
        exam_sift_records(records, i - 1, count, element_size, compare);
    for (size_t i = count - 1; i > 0; --i) {
        exam_swap_records(records, 0, i, element_size);
        exam_sift_records(records, 0, i, element_size, compare);
    }
}
/**
 * @public
 * @requires exam_sort_records
 * @title 结构体排序
 * @related record_type find_records_by
 * 定义命名结构体 struct Record，再传入比较函数。负数表示 a 排在 b 前面，正数表示排在后面，0 表示相等。原地排序，不保证稳定，最坏 O(n log n)。
 * @example
 * #include "exam.h"
 * struct Record { int id; };
 * static int cmp_id(const Record *a, const Record *b) {
 *     return (a->id > b->id) - (a->id < b->id);
 * }
 * int main(void) {
 *     Record a[] = {{3}, {1}, {2}};
 *     sort_records(a, 3, cmp_id);
 *     return 0;
 * }
 */
#define sort_records(array, n, compare) \
    exam_sort_records((array), (n), sizeof((array)[0]), (compare))
