/** @category 字符串 */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void str_insert_into(char *s, int pos, const char *text,
                                   size_t capacity) {
    size_t n = strlen(s), add = strlen(text);
    if (pos < 0 || (size_t)pos > n || n >= capacity ||
        add > capacity - n - 1)
        exam_algo_fail("str_insert invalid index or capacity");
    /* text must not alias s when insertion shifts the buffer. */
    memmove(s + pos + add, s + pos, n - (size_t)pos + 1);
    memcpy(s + pos, text, add);
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id str_erase
 * @title 删除一段字符串
 * @members str_erase
 * 从下标 pos 起删除 count 个字符，范围必须在字符串内。原地修改。
 * @example
 * char s[] = "abcde";
 * str_erase(s, 1, 2); // ade
 */
static inline void str_erase(char *s, int pos, int count) {
    size_t n = strlen(s);
    if (pos < 0 || count < 0 || (size_t)pos > n ||
        (size_t)count > n - (size_t)pos)
        exam_algo_fail("str_erase invalid range");
    memmove(s + pos, s + pos + count, n - (size_t)pos - (size_t)count + 1);
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id str_replace_char
 * @title 替换字符
 * @members str_replace_char
 * 原地将所有 old_char 替换为 new_char，返回替换次数。
 * @example
 * char s[] = "banana";
 * int n = str_replace_char(s, 'a', 'o'); // 3
 */
static inline int str_replace_char(char *s, char old_char, char new_char) {
    if (!old_char || !new_char) exam_algo_fail("NUL cannot be replaced");
    int count = 0;
    for (size_t i = 0; s[i]; ++i) if (s[i] == old_char) {
        s[i] = new_char; ++count;
    }
    return count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id str_remove_char
 * @title 删除指定字符
 * @members str_remove_char
 * 原地删除所有 key 字符，返回删除次数。
 * @example
 * char s[] = "banana";
 * int n = str_remove_char(s, 'a'); // bnn
 */
static inline int str_remove_char(char *s, char key) {
    if (!key) exam_algo_fail("NUL cannot be removed");
    size_t write = 0;
    int removed = 0;
    for (size_t read = 0; s[read]; ++read) {
        if (s[read] == key) ++removed;
        else s[write++] = s[read];
    }
    s[write] = '\0';
    return removed;
}
/* Non-overlapping replacement; needle and replacement must not alias s. */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int str_replace_all_into(char *s, const char *needle,
                                       const char *replacement,
                                       size_t capacity) {
    if (!s || !needle || !replacement || capacity == 0)
        exam_algo_fail("str_replace_all invalid arguments");
    size_t old_len = strlen(needle), new_len = strlen(replacement);
    if (!old_len) exam_algo_fail("str_replace_all empty pattern");
    char *out = (char *)malloc(capacity);
    if (!out) exam_algo_fail("str_replace_all allocation failed");
    size_t written = 0;
    int count = 0;
    const char *src = s, *match;
    while ((match = strstr(src, needle)) != NULL) {
        size_t prefix = (size_t)(match - src);
        if (prefix > capacity - 1 - written ||
            new_len > capacity - 1 - written - prefix)
            { free(out); exam_algo_fail("str_replace_all exceeds capacity"); }
        memcpy(out + written, src, prefix);
        written += prefix;
        memcpy(out + written, replacement, new_len);
        written += new_len;
        src = match + old_len;
        if (count == INT_MAX) { free(out); exam_algo_fail("replace count overflow"); }
        ++count;
    }
    size_t tail = strlen(src);
    if (tail > capacity - 1 - written) {
        free(out); exam_algo_fail("str_replace_all exceeds capacity");
    }
    memcpy(out+written, src, tail+1);
    memcpy(s, out, written+tail+1);
    free(out);
    return count;
}
/**
 * @public
 * @requires str_insert_into
 * @category 字符串
 */
/**
 * @id str_insert
 * @title 插入字符串
 * @members str_insert str_insert_into
 * 在下标 pos 前插入 text。pos 可以等于原长度；目标容量包含结束符，text 不能指向目标缓冲区。
 * @example
 * char s[20] = "ac";
 * str_insert(s, 1, "b"); // abc
 */
#define str_insert(array, pos, text) \
    str_insert_into((array), (pos), (text), sizeof(array))
/**
 * @public
 * @requires str_replace_all_into
 */
/**
 * @id str_replace_all
 * @title 替换子串
 * @members str_replace_all str_replace_all_into
 * 原地替换所有非重叠匹配，返回替换次数。needle 非空；needle 和 replacement 不可指向目标缓冲区，目标容量须足够。
 * @example
 * char s[30] = "a--b--c";
 * int n = str_replace_all(s, "--", ":"); // a:b:c
 */
#define str_replace_all(array, needle, replacement) \
    str_replace_all_into((array), (needle), (replacement), sizeof(array))
