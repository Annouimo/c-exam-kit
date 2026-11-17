/* SPDX-License-Identifier: MIT-0 */
/* compare(a,b)<0 means a has higher priority. Same callback shape as qsort. */
typedef struct {
    @TYPE@ *data;
    size_t len, cap;
    int (*compare)(const void *, const void *);
} @NAME@;
static @NAME@ @NAME@_new(int (*compare)(const void *, const void *)) {
    if (!compare) tb_fail("heap needs a comparator");
    @NAME@ h = {0}; h.compare = compare; return h;
}
static void @NAME@_reserve(@NAME@ *h, size_t needed) {
    if (needed <= h->cap) return;
    size_t cap = tb_capacity(h->cap, needed);
    h->data = tb_resize(h->data, cap, sizeof *h->data);
    h->cap = cap;
}
static void @NAME@_sift_down(@NAME@ *h, size_t pos) {
    @TYPE@ value = h->data[pos];
    while (pos < h->len / 2) {
        size_t child = pos * 2 + 1;
        if (child + 1 < h->len &&
            h->compare(&h->data[child + 1], &h->data[child]) < 0) ++child;
        if (h->compare(&value, &h->data[child]) <= 0) break;
        h->data[pos] = h->data[child]; pos = child;
    }
    h->data[pos] = value;
}
static void @NAME@_push(@NAME@ *h, @TYPE@ value) {
    if (!h->compare) tb_fail("heap needs a comparator");
    @NAME@_reserve(h, tb_add_size(h->len, 1));
    size_t pos = h->len++;
    while (pos) {
        size_t parent = (pos - 1) / 2;
        if (h->compare(&h->data[parent], &value) <= 0) break;
        h->data[pos] = h->data[parent]; pos = parent;
    }
    h->data[pos] = value;
}
static bool @NAME@_peek(const @NAME@ *h, @TYPE@ *out) {
    if (!h->len) return false;
    if (out) *out = h->data[0];
    return true;
}
static bool @NAME@_pop(@NAME@ *h, @TYPE@ *out) {
    if (!@NAME@_peek(h, out)) return false;
    --h->len;
    if (h->len) {
        h->data[0] = h->data[h->len];
        @NAME@_sift_down(h, 0);
    }
    return true;
}
static @NAME@ @NAME@_from(@TYPE@ const *items, size_t count,
                         int (*compare)(const void *, const void *)) {
    @NAME@ h = @NAME@_new(compare);
    @NAME@_reserve(&h, count);
    if (count) memcpy(h.data, items, count * sizeof *h.data);
    h.len = count;
    for (size_t i = count / 2; i > 0; --i) @NAME@_sift_down(&h, i - 1);
    return h;
}
static @NAME@ @NAME@_clone(const @NAME@ *h) {
    @NAME@ copy = *h;
    copy.data = NULL; copy.cap = 0;
    @NAME@_reserve(&copy, h->len);
    if (h->len) memcpy(copy.data, h->data, h->len * sizeof *h->data);
    return copy;
}
static void @NAME@_clear(@NAME@ *h) { h->len = 0; }
static void @NAME@_free(@NAME@ *h) { free(h->data); *h = (@NAME@){0}; }
