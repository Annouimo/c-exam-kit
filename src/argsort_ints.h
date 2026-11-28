
/* Stable indirect sorting: order[i] is an ORIGINAL, zero-based index.
 * The input array is never modified. Equal values use original index order.
 */
typedef struct {
    int value;
    int index;
} ExamIntIndexPair;
/**
 * @requires ExamIntIndexPair
 */
static inline int exam_int_index_compare(const void *left, const void *right) {
    const ExamIntIndexPair *a = (const ExamIntIndexPair *)left;
    const ExamIntIndexPair *b = (const ExamIntIndexPair *)right;
    if (a->value != b->value)
        return (a->value > b->value) - (a->value < b->value);
    return (a->index > b->index) - (a->index < b->index);
}
/**
 * @public
 * @requires exam_algo_fail ExamIntIndexPair exam_int_index_compare
 */
static inline void argsort_ints_into(const int *a, int n, int *order,
                                     size_t capacity) {
    if (n < 0 || (size_t)n > capacity)
        exam_algo_fail("argsort_ints output capacity too small");
    if (n == 0) return;
    if (!a || !order) exam_algo_fail("null argsort_ints array");
    if ((size_t)n > SIZE_MAX / sizeof(ExamIntIndexPair))
        exam_algo_fail("argsort_ints allocation size overflow");
    ExamIntIndexPair *pairs = (ExamIntIndexPair *)malloc(
        (size_t)n * sizeof(*pairs));
    if (!pairs) exam_algo_fail("argsort_ints allocation failed");
    for (int i = 0; i < n; ++i) {
        pairs[i].value = a[i];
        pairs[i].index = i;
    }
    qsort(pairs, (size_t)n, sizeof(*pairs), exam_int_index_compare);
    for (int i = 0; i < n; ++i) order[i] = pairs[i].index;
    free(pairs);
}
/**
 * @public
 * @requires argsort_ints_into
 * @id argsort_ints
 * @title 间接排序（原下标）
 * @tags 数组 排序
 * @summary 不修改原数组，生成按值升序排列的原下标。
 * @pre order 至少 n 项，不能与 a 重叠。
 * @keywords 间接排序 原下标 排名
 * @members argsort_ints argsort_ints_into
 * @related sort_arrays
 * 不修改 a，把升序排列对应的原下标写入 order；相等值按原下标排序。order 至少 n 项，不能与 a 重叠。
 * @example
 * int a[] = {8, 3, 8, 2}, order[4];
 * argsort_ints(a, 4, order); // 3, 1, 0, 2
 */
#define argsort_ints(a, n, order) \
    argsort_ints_into((a), (n), (order), sizeof(order) / sizeof((order)[0]))
