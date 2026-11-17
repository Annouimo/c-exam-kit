/* SPDX-License-Identifier: MIT-0 */
static void string_clear(String *s) {
    s->len = 0;
    if (s->data) s->data[0] = '\0';
}
static void string_assign(String *s, const char *text) {
    String copy = string_from(text); /* permits text inside s.data */
    string_free(s);
    *s = copy;
}
static void string_push(String *s, char ch) {
    if (!ch) tb_fail("NUL is not a text character");
    string_reserve(s, tb_add_size(s->len, 1));
    s->data[s->len++] = ch;
    s->data[s->len] = '\0';
}
static bool string_pop(String *s, char *out) {
    if (!s->len) return false;
    char ch = s->data[--s->len];
    s->data[s->len] = '\0';
    if (out) *out = ch;
    return true;
}
static bool string_contains(const String *s, const char *text) {
    return string_find(s, text, 0) != SIZE_MAX;
}
static bool string_starts_with(const String *s, const char *text) {
    size_t n = strlen(text);
    return n <= s->len && !memcmp(string_cstr(s), text, n);
}
static bool string_ends_with(const String *s, const char *text) {
    size_t n = strlen(text);
    return n <= s->len && !memcmp(string_cstr(s) + s->len - n, text, n);
}
/* Non-overlapping occurrences; empty needle counts as zero. */
static size_t string_count(const String *s, const char *needle) {
    size_t width = strlen(needle), start = 0, count = 0, pos;
    if (!width) return 0;
    while ((pos = string_find(s, needle, start)) != SIZE_MAX) {
        ++count;
        start = pos + width;
    }
    return count;
}
static bool string_remove_first(String *s, const char *needle) {
    if (!*needle) return false;
    size_t pos = string_find(s, needle, 0);
    return pos != SIZE_MAX && string_erase(s, pos, strlen(needle));
}
/* Single pass over original matches; does not revisit new boundary matches. */
static void string_remove_all(String *s, const char *needle) {
    string_replace_all(s, needle, "");
}
/* Restarts from the beginning after every deletion. Returns deletion count. */
static size_t string_remove_repeated(String *s, const char *needle) {
    if (!*needle) return 0;
    String copy = string_from(needle); /* needle may alias s */
    size_t count = 0;
    while (string_remove_first(s, copy.data)) ++count;
    string_free(&copy);
    return count;
}
static void string_trim(String *s) {
    size_t first = 0, end = s->len;
    const char *text = string_cstr(s);
    while (first < end && isspace((unsigned char)text[first])) ++first;
    while (end > first && isspace((unsigned char)text[end - 1])) --end;
    (void)string_erase(s, end, s->len - end);
    (void)string_erase(s, 0, first);
}
/* Byte-wise case conversion in the current C locale, not Unicode case folding. */
static void string_lower(String *s) {
    for (size_t i = 0; i < s->len; ++i)
        s->data[i] = (char)tolower((unsigned char)s->data[i]);
}
static void string_upper(String *s) {
    for (size_t i = 0; i < s->len; ++i)
        s->data[i] = (char)toupper((unsigned char)s->data[i]);
}
/* Whitespace-delimited token of arbitrary length; consumes one ending space. */
static bool string_read_token(String *s, FILE *input) {
    string_clear(s);
    int ch;
    do { ch = fgetc(input); } while (ch != EOF && isspace((unsigned char)ch));
    if (ch == EOF) return false;
    do {
        string_push(s, (char)ch);
        ch = fgetc(input);
    } while (ch != EOF && !isspace((unsigned char)ch));
    return true;
}
