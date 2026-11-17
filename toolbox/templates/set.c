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
