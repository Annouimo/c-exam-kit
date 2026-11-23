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

#define find_records_by(array, n, key, compare) \
    exam_find_records_by((array), (n), sizeof((array)[0]), (key), (compare))

#define binary_search_records_by(array, n, key, compare) \
    exam_binary_search_records_by((array), (n), sizeof((array)[0]), (key), (compare))
