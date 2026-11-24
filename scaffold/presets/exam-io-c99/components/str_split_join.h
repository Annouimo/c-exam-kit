/** @category 字符串 */
/* Text split/join for *actual* rectangular char arrays:
 *   char words[100][50];
 *   int n = str_split("a,,b,", ",", words); // four fields
 *   char text[300];
 *   str_join(words, n, ",", text);
 * out_rows is a char[rows][width] buffer laid out contiguously.
 * Delimiter split preserves empty fields and never changes input text.
 */
/** @requires exam_algo_fail */
static inline void exam_split_store(char *out_rows, size_t capacity_rows,
                                    size_t width, int row,
                                    const char *begin, size_t length) {
    if (row < 0 || (size_t)row >= capacity_rows || width == 0 ||
        length >= width || !out_rows)
        exam_algo_fail("str_split output capacity too small");
    char *destination = out_rows + (size_t)row * width;
    memcpy(destination, begin, length);
    destination[length] = '\0';
}
/**
 * @public
 * @requires exam_algo_fail exam_split_store
 */
static inline int str_split_into(const char *text, const char *delimiter,
                                 char *out_rows, size_t rows, size_t width) {
    if (!text || !delimiter || !delimiter[0])
        exam_algo_fail("str_split requires a nonempty delimiter");
    size_t delimiter_length = strlen(delimiter);
    int count = 0;
    const char *begin = text;
    const char *match;
    while ((match = strstr(begin, delimiter)) != NULL) {
        if (count == INT_MAX) exam_algo_fail("too many split fields");
        exam_split_store(out_rows, rows, width, count++,
                         begin, (size_t)(match - begin));
        begin = match + delimiter_length;
    }
    if (count == INT_MAX) exam_algo_fail("too many split fields");
    exam_split_store(out_rows, rows, width, count++, begin, strlen(begin));
    return count;
}
/**
 * @public
 * @requires exam_algo_fail exam_split_store
 */
static inline int str_split_whitespace_into(const char *text,
                                             char *out_rows,
                                             size_t rows, size_t width) {
    if (!text) exam_algo_fail("null str_split_whitespace input");
    int count = 0;
    const char *p = text;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) ++p;
        if (!*p) break;
        const char *begin = p;
        while (*p && !isspace((unsigned char)*p)) ++p;
        if (count == INT_MAX) exam_algo_fail("too many split fields");
        exam_split_store(out_rows, rows, width, count++,
                         begin, (size_t)(p - begin));
    }
    return count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void str_join_into(const char *rows, int n,
                                 size_t row_capacity, size_t width,
                                 const char *delimiter,
                                 char *output, size_t capacity) {
    if (n < 0 || (size_t)n > row_capacity || !delimiter || !output ||
        capacity == 0 || width == 0 || (n > 0 && !rows))
        exam_algo_fail("invalid str_join arguments");
    size_t separator_length = strlen(delimiter);
    size_t written = 0;
    for (int i = 0; i < n; ++i) {
        if (i) {
            if (separator_length > capacity - written - 1)
                exam_algo_fail("str_join output capacity too small");
            memcpy(output + written, delimiter, separator_length);
            written += separator_length;
        }
        const char *word = rows + (size_t)i * width;
        const char *end = (const char *)memchr(word, '\0', width);
        if (!end) exam_algo_fail("str_join input row is not NUL-terminated");
        size_t length = (size_t)(end - word);
        if (length > capacity - written - 1)
            exam_algo_fail("str_join output capacity too small");
        memcpy(output + written, word, length);
        written += length;
    }
    output[written] = '\0';
}
/**
 * @public
 * @requires str_split_into
 * @category 字符串
 * @title 拆分、拼接
 * @members str_split str_split_whitespace str_join
 * `str_split(s, ",", words)` 按字面分隔符拆分并保留空字段。`str_split_whitespace(s, words)` 按空白拆分并跳过空字段。两者返回字段数。用 `str_join(words, n, "-", out)` 写入拼接结果，`words` 应是 `char words[行数][每行容量]` 这样的二维数组
 */
#define str_split(text, delimiter, rows) \
    str_split_into((text), (delimiter), (char *)(rows), \
                   sizeof(rows) / sizeof((rows)[0]), sizeof((rows)[0]))
/**
 * @public
 * @requires str_split_whitespace_into
 */
#define str_split_whitespace(text, rows) \
    str_split_whitespace_into((text), (char *)(rows), \
                              sizeof(rows) / sizeof((rows)[0]), \
                              sizeof((rows)[0]))
/**
 * @public
 * @requires str_join_into
 */
#define str_join(rows, n, delimiter, output) \
    str_join_into((const char *)(rows), (n), \
                  sizeof(rows) / sizeof((rows)[0]), sizeof((rows)[0]), \
                  (delimiter), (output), sizeof(output))
