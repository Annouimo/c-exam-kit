/** @category 字符串 */
/* Byte-oriented strings: no UTF-8 character awareness. */
/**
 * @public
 * @category 字符串
 */
/**
 * @id str_reverse
 * @title 反转字符串
 * @members str_reverse
 * 原地反转字符串中的字节，不包含结尾的空字符。
 * @example
 * char s[] = "abc";
 * str_reverse(s); // cba
 */
static inline void str_reverse(char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) {
        char c = s[i]; s[i] = s[n-1-i]; s[n-1-i] = c;
    }
}
/** @public */
/**
 * @id str_case
 * @title 大小写转换
 * @members str_lower str_upper
 * 原地转换英文字母大小写。
 * @example
 * char s[] = "AbC";
 * str_lower(s); // abc
 * str_upper(s); // ABC
 */
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
/**
 * @id str_trim
 * @title 去除首尾空白
 * @members str_trim
 * 原地删除首尾空白，保留中间空白。
 * @example
 * char s[] = "  a b  ";
 * str_trim(s); // a b
 */
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
 */
/**
 * @id str_is_palindrome
 * @title 判断回文
 * @members str_is_palindrome
 * 按字节比较首尾，返回是否为回文；区分大小写。
 * @example
 * bool ok = str_is_palindrome("abba");
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
/**
 * @id str_count_char
 * @title 统计字符
 * @members str_count_char
 * 返回指定字符出现的次数。
 * @example
 * int n = str_count_char("banana", 'a'); // 3
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
/**
 * @id str_count_substr
 * @title 统计子串
 * @members str_count_substr
 * needle 是待匹配子串。overlapping 为 true 时允许重叠匹配，否则只统计不重叠匹配。
 * @example
 * int a = str_count_substr("aaaa", "aa", true); // 3
 * int b = str_count_substr("aaaa", "aa", false); // 2
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
/**
 * @id str_longest_char_run
 * @title 最长连续字符
 * @members str_longest_char_run
 * 返回指定字符连续出现的最大长度。
 * @example
 * int n = str_longest_char_run("aaabba", 'a'); // 3
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
