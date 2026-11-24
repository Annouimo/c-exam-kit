/** @category 字符串 */
static inline int exam_cmp_unsigned_char(const void *x, const void *y) {
    unsigned char a = *(const unsigned char *)x;
    unsigned char b = *(const unsigned char *)y;
    return (a > b) - (a < b);
}
/**
 * @public
 * @requires exam_cmp_unsigned_char
 * @category 字符串
 */
/**
 * @id str_sort_chars
 * @title 字符排序
 * @members str_sort_chars
 * 原地按无符号字节值升序排列。
 * @example
 * char s[] = "cab";
 * str_sort_chars(s); // abc
 */
static inline void str_sort_chars(char *s) {
    qsort(s, strlen(s), sizeof(char), exam_cmp_unsigned_char);
}
/** @public */
/**
 * @id str_unique_chars
 * @title 字符去重
 * @members str_unique_chars
 * 原地去除重复字符，保留首次出现顺序，返回新长度。
 * @example
 * char s[] = "banana";
 * int n = str_unique_chars(s); // ban
 */
static inline int str_unique_chars(char *s) {
    bool seen[256] = {false};
    size_t write = 0, length = strlen(s);
    for (size_t i = 0; i < length; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (!seen[c]) { seen[c] = true; s[write++] = s[i]; }
    }
    s[write] = '\0';
    return (int)write;
}
