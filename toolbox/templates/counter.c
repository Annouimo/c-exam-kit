/* SPDX-License-Identifier: MIT-0 */
typedef struct { Map items; } Counter;
static Counter counter_new(void) { return (Counter){0}; }
/* Missing keys read as zero. Zero/negative counts are retained, like Python Counter. */
static long long counter_get(const Counter *c, const char *key) {
    long long value = 0;
    (void)map_get(&c->items, key, &value);
    return value;
}
/* false on overflow; leaves the counter unchanged. */
static bool counter_add(Counter *c, const char *key, long long delta) {
    long long value = counter_get(c, key);
    if ((delta > 0 && value > LLONG_MAX - delta) ||
        (delta < 0 && value < LLONG_MIN - delta)) return false;
    (void)map_set(&c->items, key, value + delta);
    return true;
}
static bool counter_remove(Counter *c, const char *key) { return map_remove(&c->items, key); }
static bool counter_next(const Counter *c, size_t *cursor, const char **key, long long *value) {
    return map_next(&c->items, cursor, key, value);
}
static Counter counter_clone(const Counter *c) { Counter copy = {map_clone(&c->items)}; return copy; }
static void counter_free(Counter *c) { map_free(&c->items); }
static void counter_clear(Counter *c) { map_clear(&c->items); }
static size_t counter_size(const Counter *c) { return c->items.len; }
/* Highest count; ties choose lexicographically smallest key. Borrowed key. */
static bool counter_most_common(const Counter *c, const char **out_key, long long *out_count) {
    size_t cursor = 0;
    const char *key, *best_key = NULL;
    long long value, best = 0;
    while (counter_next(c, &cursor, &key, &value)) {
        if (!best_key || value > best || (value == best && strcmp(key, best_key) < 0)) {
            best_key = key; best = value;
        }
    }
    if (!best_key) return false;
    if (out_key) *out_key = best_key;
    if (out_count) *out_count = best;
    return true;
}

