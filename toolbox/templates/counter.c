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
