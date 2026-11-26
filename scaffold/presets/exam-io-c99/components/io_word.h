/** @category 输入和输出 */
/* For actual char arrays only: sizeof(pointer) is NOT the buffer capacity. */
/** @requires exam_failf exam_io_token */
static inline bool exam_read_word_into(const char *api, char *out, size_t capacity) {
    if (!out) exam_failf("io", api, "destination must not be NULL");
    return exam_io_token(api, out, capacity) != 0;
}
/**
 * @public
 * @requires exam_read_word_into
 */
static inline bool try_read_word_into(char *out, size_t capacity) {
    return exam_read_word_into("try_read_word", out, capacity);
}
/**
 * @public
 * @requires exam_read_word_into exam_failf
 */
static inline void read_word_into(char *out, size_t capacity) {
    if (!exam_read_word_into("read_word", out, capacity))
        exam_failf("io", "read_word", "unexpected EOF");
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
 */
/**
 * @id read_word
 * @title 读一个单词
 * @keywords 字符串 单词 scanf EOF
 * @members read_word try_read_word read_word_into try_read_word_into
 * 跳过前导空白，读取一个不含空白的单词。try 版本在正常 EOF 返回 false。普通宏要求实际字符数组；指针使用 *_into，容量包含结束符。单词过长时，诊断会显示缓冲区容量和可容纳的内容字节数。
 * @example
 * char word[100];
 * read_word(word);
 */
#define read_word(array) read_word_into((array), sizeof(array))
