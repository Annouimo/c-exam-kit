static inline int exam_cmp_unsigned_char(const void *x, const void *y) {
    unsigned char a = *(const unsigned char *)x;
    unsigned char b = *(const unsigned char *)y;
    return (a > b) - (a < b);
}
static inline void str_sort_chars(char *s) {
    qsort(s, strlen(s), sizeof(char), exam_cmp_unsigned_char);
}
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
