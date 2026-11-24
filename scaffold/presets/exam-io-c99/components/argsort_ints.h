/** @category 数组 */
/* Stable indirect sorting: order[i] is an ORIGINAL, zero-based index.
 * The input array is never modified. Equal values use original index order.
 */
typedef struct {
    int value;
    int index;
} ExamIntIndexPair;
/** @requires ExamIntIndexPair */
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
 */
#define argsort_ints(a, n, order) \
    argsort_ints_into((a), (n), (order), sizeof(order) / sizeof((order)[0]))
