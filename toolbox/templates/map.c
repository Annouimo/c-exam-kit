/* SPDX-License-Identifier: MIT-0 */
/* Open addressing, owned copies of string keys, long long values.
   state: 0=empty, 1=live, 2=tombstone. Iteration order is unspecified. */
typedef struct { char *key; long long value; unsigned char state; } MapEntry;
typedef struct { MapEntry *entries; size_t len, used, cap; } Map;
static Map map_new(void) { return (Map){0}; }
static size_t tb_hash(const char *key) {
    size_t h = 5381;
    for (const unsigned char *p = (const unsigned char *)key; *p; ++p)
        h = h * 33u + *p;
    return h;
}
static size_t tb_map_slot(const Map *m, const char *key, bool *found) {
    size_t slot = tb_hash(key) % m->cap, deleted = SIZE_MAX;
    for (size_t n = 0; n < m->cap; ++n) {
        const MapEntry *e = &m->entries[slot];
        if (!e->state) {
            *found = false;
            return deleted != SIZE_MAX ? deleted : slot;
        }
        if (e->state == 1 && strcmp(e->key, key) == 0) {
            *found = true;
            return slot;
        }
        if (e->state == 2 && deleted == SIZE_MAX) deleted = slot;
        slot = (slot + 1) % m->cap;
    }
    *found = false;
    return deleted;
}
static void tb_map_rehash(Map *m, size_t cap) {
    Map fresh = {0};
    fresh.cap = cap;
    fresh.entries = tb_resize(NULL, cap, sizeof *fresh.entries);
    memset(fresh.entries, 0, cap * sizeof *fresh.entries);
    for (size_t i = 0; i < m->cap; ++i) {
        if (m->entries[i].state != 1) continue;
        bool found;
        size_t slot = tb_map_slot(&fresh, m->entries[i].key, &found);
        fresh.entries[slot] = m->entries[i];
        ++fresh.len;
        ++fresh.used;
    }
    free(m->entries);
    *m = fresh;
}
static bool map_get(const Map *m, const char *key, long long *out) {
    if (!m->cap) return false;
    bool found;
    size_t slot = tb_map_slot(m, key, &found);
    if (found && out) *out = m->entries[slot].value;
    return found;
}
static bool map_contains(const Map *m, const char *key) { return map_get(m, key, NULL); }
/* true if inserted; false if an existing value was replaced. */
static bool map_set(Map *m, const char *key, long long value) {
    if (!m->cap) tb_map_rehash(m, 16);
    bool found;
    size_t slot = tb_map_slot(m, key, &found);
    if (found) { m->entries[slot].value = value; return false; }
    if (m->used >= m->cap / 2) {
        size_t cap = m->cap;
        if (m->len >= m->cap / 4) cap = tb_add_size(cap, cap);
        tb_map_rehash(m, cap);
        slot = tb_map_slot(m, key, &found);
    }
    size_t n = tb_add_size(strlen(key), 1);
    char *copy = tb_resize(NULL, n, 1);
    memcpy(copy, key, n);
    if (!m->entries[slot].state) ++m->used;
    m->entries[slot] = (MapEntry){copy, value, 1};
    ++m->len;
    return true;
}
static bool map_remove(Map *m, const char *key) {
    if (!m->cap) return false;
    bool found;
    size_t slot = tb_map_slot(m, key, &found);
    if (!found) return false;
    free(m->entries[slot].key);
    m->entries[slot] = (MapEntry){NULL, 0, 2};
    --m->len;
    return true;
}
/* cursor starts at 0; keys are borrowed and invalid after mutation/free. */
static bool map_next(const Map *m, size_t *cursor, const char **key, long long *value) {
    while (*cursor < m->cap) {
        const MapEntry *e = &m->entries[(*cursor)++];
        if (e->state != 1) continue;
        if (key) *key = e->key;
        if (value) *value = e->value;
        return true;
    }
    return false;
}
static void map_free(Map *m) {
    for (size_t i = 0; i < m->cap; ++i)
        if (m->entries[i].state == 1) free(m->entries[i].key);
    free(m->entries);
    *m = (Map){0};
}
static Map map_clone(const Map *m) {
    Map copy = {0};
    for (size_t i = 0; i < m->cap; ++i)
        if (m->entries[i].state == 1)
            (void)map_set(&copy, m->entries[i].key, m->entries[i].value);
    return copy;
}
static size_t map_size(const Map *m) { return m->len; }
static long long map_get_or(const Map *m, const char *key, long long fallback) {
    long long value;
    return map_get(m, key, &value) ? value : fallback;
}
static void map_clear(Map *m) {
    for (size_t i = 0; i < m->cap; ++i)
        if (m->entries[i].state == 1) free(m->entries[i].key);
    if (m->cap) memset(m->entries, 0, m->cap * sizeof *m->entries);
    m->len = m->used = 0;
}

