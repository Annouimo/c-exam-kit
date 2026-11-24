/** @category 字符串 */
/* Byte-oriented strings: no UTF-8 character awareness. */
/**
 * @public
 * @category 字符串
 * @title 反转、大小写、去首尾空白
 * @members str_reverse str_lower str_upper str_trim
 * `str_reverse(s)`、`str_lower(s)`、`str_upper(s)`、`str_trim(s)` 均原地修改
 */
static inline void str_reverse(char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) {
        char c = s[i]; s[i] = s[n-1-i]; s[n-1-i] = c;
    }
}
/** @public */
static inline void str_lower(char *s) {
    for (size_t i = 0; s[i]; ++i)
        s[i] = (char)tolower((unsigned char)s[i]);
}
/** @public */
static inline void str_upper(char *s) {
    for (size_t i = 0; s[i]; ++i)
        s[i] = (char)toupper((unsigned char)s[i]);
}
/** @public */
static inline void str_trim(char *s) {
    size_t n = strlen(s), left = 0, right = n;
    while (left < n && isspace((unsigned char)s[left])) ++left;
    while (right > left && isspace((unsigned char)s[right-1])) --right;
    memmove(s, s+left, right-left);
    s[right-left] = '\0';
}
/**
 * @public
 * @category 字符串
 * @title 回文、计数、最长连续段
 * @members str_is_palindrome str_count_char str_count_substr str_longest_char_run
 * `str_is_palindrome(s)` 判断回文。`str_count_char(s, ch)` 数字符。`str_count_substr(s, sub, true)` 允许重叠，传 `false` 统计非重叠匹配。`str_longest_char_run(s, ch)` 返回指定字符的最长连续长度
 */
static inline bool str_is_palindrome(const char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) if (s[i] != s[n-1-i]) return false;
    return true;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int str_count_char(const char *s, char key) {
    if (!key) exam_algo_fail("str_count_char does not accept NUL");
    int count = 0;
    for (size_t i = 0; s[i]; ++i) if (s[i] == key) ++count;
    return count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int str_count_substr(const char *s, const char *needle,
                                  bool overlapping) {
    if (!*needle) exam_algo_fail("str_count_substr empty pattern");
    int count = 0;
    size_t advance = overlapping ? 1 : strlen(needle);
    const char *pos = s;
    while ((pos = strstr(pos, needle)) != NULL) {
        if (count == INT_MAX) exam_algo_fail("substring count overflow");
        ++count;
        pos += advance;
    }
    return count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int str_longest_char_run(const char *s, char target) {
    if (!target) exam_algo_fail("NUL cannot be a run target");
    int best = 0, run = 0;
    for (size_t i = 0; s[i]; ++i) {
        run = s[i] == target ? run + 1 : 0;
        if (run > best) best = run;
    }
    return best;
}
