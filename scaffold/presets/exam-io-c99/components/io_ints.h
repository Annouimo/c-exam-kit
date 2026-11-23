static inline void read_ints_into(int *array, int count, size_t capacity) {
    if (count < 0 || (size_t)count > capacity)
        exam_io_fail("int array length exceeds capacity");
    if (count && !array) exam_io_fail("null int array");
    for (int i = 0; i < count; ++i) array[i] = read_int();
}
#define read_ints(array, count) \
    read_ints_into((array), (count), sizeof(array) / sizeof((array)[0]))
