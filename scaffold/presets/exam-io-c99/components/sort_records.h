/* The user defines:
 *   struct Record { ... };
 *   int record_cmp(const Record *a, const Record *b);
 * after including exam.h. sort_records uses qsort; stability is NOT guaranteed.
 */
typedef struct Record Record;
int record_cmp(const Record *a, const Record *b);

static inline int exam_record_qsort_cmp(const void *a, const void *b) {
    return record_cmp((const Record *)a, (const Record *)b);
}
static inline void exam_sort_records(void *records, int n, size_t element_size) {
    exam_check_n(n);
    if (n > 1) {
        if (!records || !element_size)
            exam_algo_fail("invalid record array");
        qsort(records, (size_t)n, element_size, exam_record_qsort_cmp);
    }
}
#define sort_records(array, n) \
    exam_sort_records((array), (n), sizeof((array)[0]))
