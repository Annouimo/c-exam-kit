/* SPDX-License-Identifier: MIT-0 */
/* Owns data. Zero initialization is valid. Positions and lengths count bytes. */
typedef struct { char *data; size_t len, cap; } String;
static const char *string_cstr(const String *s) { return s->data ? s->data : ""; }
static void string_reserve(String *s, size_t length) {
    size_t needed = tb_add_size(length, 1);
    if (needed <= s->cap) return;
    size_t cap = tb_capacity(s->cap, needed);
    s->data = tb_resize(s->data, cap, 1);
    s->cap = cap;
    s->data[s->len] = '\0';
}
static String string_from(const char *text) {
    String s = {0};
    size_t n = strlen(text);
    string_reserve(&s, n);
    memcpy(s.data, text, n + 1);
    s.len = n;
    return s;
}
static String string_clone(const String *s) { return string_from(string_cstr(s)); }
static void string_free(String *s) { free(s->data); *s = (String){0}; }
/* text may refer to s.data; snapshot before reserve/memmove. */
static bool string_insert(String *s, size_t pos, const char *text) {
    if (pos > s->len) return false;
    size_t n = strlen(text);
    if (!n) return true;
    size_t length = tb_add_size(s->len, n);
    char *copy = tb_resize(NULL, n, 1);
    memcpy(copy, text, n);
    string_reserve(s, length);
    memmove(s->data + pos + n, s->data + pos, s->len - pos + 1);
    memcpy(s->data + pos, copy, n);
    s->len = length;
    free(copy);
    return true;
}
static void string_append(String *s, const char *text) {
    (void)string_insert(s, s->len, text);
}
static bool string_erase(String *s, size_t pos, size_t count) {
    if (pos > s->len || count > s->len - pos) return false;
    if (count) memmove(s->data + pos, s->data + pos + count, s->len - pos - count + 1);
    s->len -= count;
    return true;
}
/* Invalid slice is a programming error; empty slices are valid. */
static String string_slice(const String *s, size_t pos, size_t count) {
    if (pos > s->len || count > s->len - pos) tb_fail("invalid string slice");
    String result = {0};
    string_reserve(&result, count);
    memcpy(result.data, string_cstr(s) + pos, count);
    result.data[count] = '\0';
    result.len = count;
    return result;
}
static size_t string_find(const String *s, const char *needle, size_t start) {
    if (start > s->len) return SIZE_MAX;
    const char *base = string_cstr(s);
    const char *p = strstr(base + start, needle);
    return p ? (size_t)(p - base) : SIZE_MAX;
}
static void string_replace_all(String *s, const char *old, const char *replacement) {
    if (!*old) return;
    String result = {0};
    size_t start = 0, width = strlen(old), pos;
    while ((pos = string_find(s, old, start)) != SIZE_MAX) {
        String part = string_slice(s, start, pos - start);
        string_append(&result, string_cstr(&part));
        string_free(&part);
        string_append(&result, replacement);
        start = pos + width;
    }
    string_append(&result, string_cstr(s) + start);
    string_free(s);
    *s = result;
}
/* Reads an entire line, removes LF and optional CR; false only at immediate EOF. */
static bool string_readline(String *s, FILE *input) {
    int ch;
    bool read_any = false;
    s->len = 0;
    string_reserve(s, 0);
    s->data[0] = '\0';
    while ((ch = fgetc(input)) != EOF && ch != '\n') {
        read_any = true;
        string_reserve(s, tb_add_size(s->len, 1));
        s->data[s->len++] = (char)ch;
    }
    if (s->len && s->data[s->len - 1] == '\r') --s->len;
    s->data[s->len] = '\0';
    return ch != EOF || read_any;
}
typedef struct { String *data; size_t len, cap; } StringList;
/* Literal, nonempty delimiter; preserves empty fields; result owns all strings. */
static StringList string_split(const String *s, const char *delimiter) {
    if (!*delimiter) tb_fail("empty split delimiter");
    StringList list = {0};
    size_t start = 0, width = strlen(delimiter);
    for (;;) {
        size_t pos = string_find(s, delimiter, start);
        size_t end = pos == SIZE_MAX ? s->len : pos;
        if (list.len == list.cap) {
            list.cap = tb_capacity(list.cap, tb_add_size(list.len, 1));
            list.data = tb_resize(list.data, list.cap, sizeof *list.data);
        }
        list.data[list.len++] = string_slice(s, start, end - start);
        if (pos == SIZE_MAX) break;
        start = pos + width;
    }
    return list;
}
static String string_join(const StringList *list, const char *separator) {
    String result = {0};
    for (size_t i = 0; i < list->len; ++i) {
        if (i) string_append(&result, separator);
        string_append(&result, string_cstr(&list->data[i]));
    }
    return result;
}
static void string_list_free(StringList *list) {
    for (size_t i = 0; i < list->len; ++i) string_free(&list->data[i]);
    free(list->data);
    *list = (StringList){0};
}
