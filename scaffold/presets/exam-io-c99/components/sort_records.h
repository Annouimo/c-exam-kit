/** @category 数组 */
/* The user defines:
 *   struct Record { ... };
 *   int record_cmp(const Record *a, const Record *b);
 * after including exam.h. sort_records uses qsort; stability is NOT guaranteed.
 */
/** @requires Record */
int record_cmp(const Record *a, const Record *b);

/** @requires record_cmp Record */
static inline int exam_record_qsort_cmp(const void *a, const void *b) {
    return record_cmp((const Record *)a, (const Record *)b);
}
/** @requires exam_check_n exam_algo_fail exam_record_qsort_cmp */
static inline void exam_sort_records(void *records, int n, size_t element_size) {
    exam_check_n(n);
    if (n > 1) {
        if (!records || !element_size)
            exam_algo_fail("invalid record array");
        qsort(records, (size_t)n, element_size, exam_record_qsort_cmp);
    }
}
/**
 * @public
 * @requires exam_sort_records
 * @category 数组
 * @title 结构体排序
 * @members sort_records
 * 定义 `struct Record` 和 `int record_cmp(const Record *a, const Record *b)`，调用 `sort_records(a, n)` 原地排序。若 `a` 应排在 `b` 前面，比较函数返回负数
 */
#define sort_records(array, n) \
    exam_sort_records((array), (n), sizeof((array)[0]))
