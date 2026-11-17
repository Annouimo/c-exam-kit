/* SPDX-License-Identifier: MIT-0 */
/* Canonical owning string sequence. Copy by default; _take explicitly moves. */
static StringList string_list_new(void) { return (StringList){0}; }
static size_t string_list_size(const StringList *list) { return list->len; }
static bool string_list_empty(const StringList *list) { return list->len == 0; }
static void string_list_reserve(StringList *list, size_t needed) {
    if (needed <= list->cap) return;
    size_t cap = tb_capacity(list->cap, needed);
    list->data = tb_resize(list->data, cap, sizeof *list->data);
    list->cap = cap;
}
static const String *string_list_get(const StringList *list, size_t pos) {
    if (pos >= list->len) tb_fail("string list index out of range");
    return &list->data[pos];
}
static String *string_list_at(StringList *list, size_t pos) {
    if (pos >= list->len) tb_fail("string list index out of range");
    return &list->data[pos];
}
static void string_list_push_take(StringList *list, String *value) {
    String moved = *value;
    *value = string_new(); /* before reserve: value may be a list element */
    string_list_reserve(list, tb_add_size(list->len, 1));
    list->data[list->len++] = moved;
}
static void string_list_push(StringList *list, const String *value) {
    String copy = string_clone(value);
    string_list_push_take(list, &copy);
}
static void string_list_push_text(StringList *list, const char *value) {
    String copy = string_from(value);
    string_list_push_take(list, &copy);
}
static bool string_list_insert(StringList *list, size_t pos, const String *value) {
    if (pos > list->len) return false;
    String copy = string_clone(value);
    string_list_reserve(list, tb_add_size(list->len, 1));
    memmove(list->data + pos + 1, list->data + pos,
            (list->len - pos) * sizeof *list->data);
    list->data[pos] = copy; ++list->len; return true;
}
static bool string_list_set(StringList *list, size_t pos, const String *value) {
    if (pos >= list->len) return false;
    String copy = string_clone(value);
    string_free(&list->data[pos]); list->data[pos] = copy;
    return true;
}
static bool string_list_erase(StringList *list, size_t pos, size_t count) {
    if (pos > list->len || count > list->len - pos) return false;
    for (size_t i = pos; i < pos + count; ++i) string_free(&list->data[i]);
    if (count) memmove(list->data + pos, list->data + pos + count,
                       (list->len - pos - count) * sizeof *list->data);
    list->len -= count; return true;
}
/* out, when non-NULL, must be initialized and must not point inside list. */
static bool string_list_pop(StringList *list, String *out) {
    if (!list->len) return false;
    String value = list->data[--list->len];
    if (out) { string_free(out); *out = value; }
    else string_free(&value);
    return true;
}
static void string_list_clear(StringList *list) {
    (void)string_list_erase(list, 0, list->len);
}
static StringList string_list_slice(const StringList *list, size_t pos, size_t count) {
    if (pos > list->len || count > list->len - pos) tb_fail("invalid string list slice");
    StringList result = string_list_new();
    string_list_reserve(&result, count);
    for (size_t i = 0; i < count; ++i) string_list_push(&result, &list->data[pos + i]);
    return result;
}
static StringList string_list_clone(const StringList *list) {
    return string_list_slice(list, 0, list->len);
}
static size_t string_list_find(const StringList *list, const String *value) {
    for (size_t i = 0; i < list->len; ++i)
        if (!strcmp(string_cstr(&list->data[i]), string_cstr(value))) return i;
    return SIZE_MAX;
}
static bool string_list_contains(const StringList *list, const String *value) {
    return string_list_find(list, value) != SIZE_MAX;
}
static int tb_string_compare(const void *a, const void *b) {
    return strcmp(string_cstr((const String *)a), string_cstr((const String *)b));
}
static void string_list_sort_by(StringList *list, int (*compare)(const void *, const void *)) {
    if (list->len > 1) qsort(list->data, list->len, sizeof *list->data, compare);
}
static void string_list_sort(StringList *list) { string_list_sort_by(list, tb_string_compare); }
static void string_list_reverse(StringList *list) {
    for (size_t i = 0; i < list->len / 2; ++i) {
        String value = list->data[i];
        list->data[i] = list->data[list->len - i - 1];
        list->data[list->len - i - 1] = value;
    }
}
/* Adjacent equals only; sort first for global deduplication. */
static void string_list_unique_sorted(StringList *list) {
    size_t written = 0;
    for (size_t i = 0; i < list->len; ++i) {
        if (written && !strcmp(string_cstr(&list->data[written - 1]),
                              string_cstr(&list->data[i]))) {
            string_free(&list->data[i]);
        } else {
            if (written != i) {
                list->data[written] = list->data[i];
                list->data[i] = string_new();
            }
            ++written;
        }
    }
    list->len = written;
}
static StringList string_split_whitespace(const String *s) {
    StringList words = string_list_new();
    size_t pos = 0;
    while (pos < s->len) {
        while (pos < s->len && isspace((unsigned char)s->data[pos])) ++pos;
        size_t start = pos;
        while (pos < s->len && !isspace((unsigned char)s->data[pos])) ++pos;
        if (pos > start) {
            String word = string_slice(s, start, pos - start);
            string_list_push_take(&words, &word);
        }
    }
    return words;
}
