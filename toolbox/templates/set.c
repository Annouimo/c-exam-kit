/* SPDX-License-Identifier: MIT-0 */
typedef struct { Map items; } Set;
static Set set_new(void) { return (Set){0}; }
static bool set_add(Set *s, const char *key) { return map_set(&s->items, key, 1); }
static bool set_contains(const Set *s, const char *key) { return map_contains(&s->items, key); }
static bool set_remove(Set *s, const char *key) { return map_remove(&s->items, key); }
static size_t set_size(const Set *s) { return s->items.len; }
static bool set_next(const Set *s, size_t *cursor, const char **key) {
    return map_next(&s->items, cursor, key, NULL);
}
static Set set_clone(const Set *s) { Set copy = {map_clone(&s->items)}; return copy; }
static void set_free(Set *s) { map_free(&s->items); }
static void set_clear(Set *s) { map_clear(&s->items); }
static bool set_is_subset(const Set *a, const Set *b) {
    size_t cursor = 0;
    const char *key;
    while (set_next(a, &cursor, &key))
        if (!set_contains(b, key)) return false;
    return true;
}
static Set set_union(const Set *a, const Set *b) {
    Set result = set_clone(a);
    size_t cursor = 0;
    const char *key;
    while (set_next(b, &cursor, &key)) (void)set_add(&result, key);
    return result;
}
static Set set_intersection(const Set *a, const Set *b) {
    Set result = {0};
    size_t cursor = 0;
    const char *key;
    while (set_next(a, &cursor, &key))
        if (set_contains(b, key)) (void)set_add(&result, key);
    return result;
}
static Set set_difference(const Set *a, const Set *b) {
    Set result = {0};
    size_t cursor = 0;
    const char *key;
    while (set_next(a, &cursor, &key))
        if (!set_contains(b, key)) (void)set_add(&result, key);
    return result;
}

