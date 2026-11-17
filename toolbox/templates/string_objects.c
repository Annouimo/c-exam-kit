/* SPDX-License-Identifier: MIT-0 */
static size_t string_size(const String *s) { return s->len; }
static bool string_empty(const String *s) { return s->len == 0; }
static char string_at(const String *s, size_t pos) {
    if (pos >= s->len) tb_fail("string index out of range");
    return s->data[pos];
}
static bool string_set(String *s, size_t pos, char ch) {
    if (pos >= s->len || !ch) return false;
    s->data[pos] = ch; return true;
}
static int string_compare(const String *a, const String *b) {
    return strcmp(string_cstr(a), string_cstr(b));
}
static bool string_equal(const String *a, const String *b) { return string_compare(a, b) == 0; }
static bool string_equal_text(const String *a, const char *b) { return !strcmp(string_cstr(a), b); }
static void string_reverse(String *s) {
    for (size_t i = 0; i < s->len / 2; ++i) {
        char ch = s->data[i]; s->data[i] = s->data[s->len - i - 1];
        s->data[s->len - i - 1] = ch;
    }
}
static bool string_is_palindrome(const String *s) {
    for (size_t i = 0; i < s->len / 2; ++i)
        if (s->data[i] != s->data[s->len - i - 1]) return false;
    return true;
}
static size_t string_count_overlapping(const String *s, const char *needle) {
    if (!*needle) return 0;
    size_t start = 0, count = 0, pos;
    while ((pos = string_find(s, needle, start)) != SIZE_MAX) {
        ++count; start = pos + 1;
    }
    return count;
}
/* _s variants accept String operands directly; original forms accept literals. */
static void string_assign_s(String *s, const String *value) { string_assign(s, string_cstr(value)); }
static void string_append_s(String *s, const String *value) { string_append(s, string_cstr(value)); }
static bool string_insert_s(String *s, size_t pos, const String *value) {
    return string_insert(s, pos, string_cstr(value));
}
static size_t string_find_s(const String *s, const String *needle, size_t start) {
    return string_find(s, string_cstr(needle), start);
}
static bool string_contains_s(const String *s, const String *needle) {
    return string_contains(s, string_cstr(needle));
}
static size_t string_count_s(const String *s, const String *needle) {
    return string_count(s, string_cstr(needle));
}
static void string_replace_all_s(String *s, const String *old, const String *replacement) {
    string_replace_all(s, string_cstr(old), string_cstr(replacement));
}
static bool string_remove_first_s(String *s, const String *needle) {
    return string_remove_first(s, string_cstr(needle));
}
static void string_remove_all_s(String *s, const String *needle) {
    string_remove_all(s, string_cstr(needle));
}
static size_t string_remove_repeated_s(String *s, const String *needle) {
    return string_remove_repeated(s, string_cstr(needle));
}
static StringList string_split_s(const String *s, const String *delimiter) {
    return string_split(s, string_cstr(delimiter));
}
static String string_join_s(const StringList *list, const String *separator) {
    return string_join(list, string_cstr(separator));
}
