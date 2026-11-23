static inline void read_lls_into(long long *array, int count, size_t capacity) {
    if (count < 0 || (size_t)count > capacity)
        exam_io_fail("long long array length exceeds capacity");
    if (count && !array) exam_io_fail("null long long array");
    for (int i = 0; i < count; ++i) array[i] = read_ll();
}
#define read_lls(array, count) \
    read_lls_into((array), (count), sizeof(array) / sizeof((array)[0]))
