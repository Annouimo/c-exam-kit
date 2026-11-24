/** @category 输入和输出 */
/* For actual char arrays only: sizeof(pointer) is NOT the buffer capacity. */
/**
 * @public
 * @requires exam_io_fail exam_io_token
 */
static inline bool try_read_word_into(char *out, size_t capacity) {
    if (!out) exam_io_fail("null word destination");
    return exam_io_token(out, capacity) != 0;
}
/**
 * @public
 * @requires try_read_word_into exam_io_fail
 */
static inline void read_word_into(char *out, size_t capacity) {
    if (!try_read_word_into(out, capacity))
        exam_io_fail("unexpected EOF while reading word");
}
/**
 * @public
 * @requires try_read_word_into
 */
#define try_read_word(array) try_read_word_into((array), sizeof(array))
/**
 * @public
 * @requires read_word_into
 * @category 输入和输出
 * @title 读单词或整行
 * @members read_word read_line
 * `read_word(s)` 读一个以空白分隔的单词，`read_line(s)` 读整行并去掉换行。若刚读过数字或单词，`read_line` 会先跳过当前行的剩余部分
 */
#define read_word(array) read_word_into((array), sizeof(array))
