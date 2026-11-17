/* SPDX-License-Identifier: MIT-0 */
/* Ring buffer. Elements are shallow copied and never individually destroyed. */
typedef struct { @TYPE@ *data; size_t len, cap, head; } @NAME@;
static @NAME@ @NAME@_new(void) { return (@NAME@){0}; }
static size_t @NAME@_size(const @NAME@ *q) { return q->len; }
static bool @NAME@_empty(const @NAME@ *q) { return q->len == 0; }
static size_t @NAME@_slot(const @NAME@ *q, size_t pos) {
    size_t tail = q->cap - q->head;
    return pos < tail ? q->head + pos : pos - tail;
}
static void @NAME@_reserve(@NAME@ *q, size_t needed) {
    if (needed <= q->cap) return;
    size_t cap = tb_capacity(q->cap, needed);
    @TYPE@ *data = tb_resize(NULL, cap, sizeof *data);
    for (size_t i = 0; i < q->len; ++i) data[i] = q->data[@NAME@_slot(q, i)];
    free(q->data);
    q->data = data; q->cap = cap; q->head = 0;
}
static void @NAME@_push_back(@NAME@ *q, @TYPE@ value) {
    @NAME@_reserve(q, tb_add_size(q->len, 1));
    q->data[@NAME@_slot(q, q->len)] = value;
    ++q->len;
}
static void @NAME@_push_front(@NAME@ *q, @TYPE@ value) {
    @NAME@_reserve(q, tb_add_size(q->len, 1));
    q->head = q->head ? q->head - 1 : q->cap - 1;
    q->data[q->head] = value; ++q->len;
}
static bool @NAME@_get(const @NAME@ *q, size_t pos, @TYPE@ *out) {
    if (pos >= q->len) return false;
    if (out) *out = q->data[@NAME@_slot(q, pos)];
    return true;
}
static bool @NAME@_front(const @NAME@ *q, @TYPE@ *out) { return @NAME@_get(q, 0, out); }
static bool @NAME@_back(const @NAME@ *q, @TYPE@ *out) {
    return q->len && @NAME@_get(q, q->len - 1, out);
}
static bool @NAME@_pop_front(@NAME@ *q, @TYPE@ *out) {
    if (!@NAME@_front(q, out)) return false;
    q->head = q->head + 1 == q->cap ? 0 : q->head + 1;
    --q->len; return true;
}
static bool @NAME@_pop_back(@NAME@ *q, @TYPE@ *out) {
    if (!@NAME@_back(q, out)) return false;
    --q->len; return true;
}
static void @NAME@_clear(@NAME@ *q) { q->len = q->head = 0; }
static @NAME@ @NAME@_clone(const @NAME@ *q) {
    @NAME@ copy = {0};
    @NAME@_reserve(&copy, q->len);
    for (size_t i = 0; i < q->len; ++i)
        copy.data[i] = q->data[@NAME@_slot(q, i)];
    copy.len = q->len;
    return copy;
}
static void @NAME@_free(@NAME@ *q) { free(q->data); *q = (@NAME@){0}; }
