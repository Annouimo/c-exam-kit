/* For actual char arrays only: sizeof(pointer) is NOT the buffer capacity. */
static inline bool try_read_word_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null word destination");
    return exam_io_token(out, capacity) != 0;
}
static inline void read_word_into(char *out, size_t capacity) {
    if (!try_read_word_into(out, capacity))
        exam_io_fail("unexpected EOF while reading word");
}
#define try_read_word(array) try_read_word_into((array), sizeof(array))
#define read_word(array) read_word_into((array), sizeof(array))
