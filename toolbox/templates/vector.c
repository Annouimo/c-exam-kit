/* SPDX-License-Identifier: MIT-0 */
/* Typed, contiguous storage. Elements are shallow copied; no element destructor. */
typedef struct { @TYPE@ *data; size_t len, cap; } @NAME@;
static @NAME@ @NAME@_new(void) { return (@NAME@){0}; }
static void @NAME@_reserve(@NAME@ *v, size_t needed) {
    if (needed <= v->cap) return;
    size_t cap = tb_capacity(v->cap, needed);
    v->data = tb_resize(v->data, cap, sizeof *v->data);
    v->cap = cap;
}
static void @NAME@_push(@NAME@ *v, @TYPE@ value) {
    @NAME@_reserve(v, tb_add_size(v->len, 1));
    v->data[v->len++] = value;
}
static bool @NAME@_pop(@NAME@ *v, @TYPE@ *out) {
    if (!v->len) return false;
    --v->len;
    if (out) *out = v->data[v->len];
    return true;
}
static bool @NAME@_insert(@NAME@ *v, size_t pos, @TYPE@ value) {
    if (pos > v->len) return false;
    @NAME@_reserve(v, tb_add_size(v->len, 1));
    memmove(v->data + pos + 1, v->data + pos, (v->len - pos) * sizeof *v->data);
    v->data[pos] = value;
    ++v->len;
    return true;
}
static bool @NAME@_erase(@NAME@ *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) return false;
    if (count) memmove(v->data + pos, v->data + pos + count,
                       (v->len - pos - count) * sizeof *v->data);
    v->len -= count;
    return true;
}
static @NAME@ @NAME@_clone(const @NAME@ *v) {
    @NAME@ copy = {0};
    if (v->len) {
        @NAME@_reserve(&copy, v->len);
        memcpy(copy.data, v->data, v->len * sizeof *v->data);
    }
    copy.len = v->len;
    return copy;
}
static void @NAME@_clear(@NAME@ *v) { v->len = 0; }
static void @NAME@_sort(@NAME@ *v, int (*compare)(const void *, const void *)) {
    if (v->len > 1) qsort(v->data, v->len, sizeof *v->data, compare);
}
static void @NAME@_free(@NAME@ *v) { free(v->data); *v = (@NAME@){0}; }
