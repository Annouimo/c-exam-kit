/* Byte-oriented strings: no UTF-8 character awareness. */
static inline void str_reverse(char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) {
        char c = s[i]; s[i] = s[n-1-i]; s[n-1-i] = c;
    }
}
static inline void str_lower(char *s) {
    for (size_t i = 0; s[i]; ++i)
        s[i] = (char)tolower((unsigned char)s[i]);
}
static inline void str_upper(char *s) {
    for (size_t i = 0; s[i]; ++i)
        s[i] = (char)toupper((unsigned char)s[i]);
}
static inline void str_trim(char *s) {
    size_t n = strlen(s), left = 0, right = n;
    while (left < n && isspace((unsigned char)s[left])) ++left;
    while (right > left && isspace((unsigned char)s[right-1])) --right;
    memmove(s, s+left, right-left);
    s[right-left] = '\0';
}
static inline bool str_is_palindrome(const char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; ++i) if (s[i] != s[n-1-i]) return false;
    return true;
}
static inline int str_count_char(const char *s, char key) {
    if (!key) exam_algo_fail("str_count_char does not accept NUL");
    int count = 0;
    for (size_t i = 0; s[i]; ++i) if (s[i] == key) ++count;
    return count;
}
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
static inline int str_longest_char_run(const char *s, char target) {
    if (!target) exam_algo_fail("NUL cannot be a run target");
    int best = 0, run = 0;
    for (size_t i = 0; s[i]; ++i) {
        run = s[i] == target ? run + 1 : 0;
        if (run > best) best = run;
    }
    return best;
}
