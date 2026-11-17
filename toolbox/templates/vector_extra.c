/* SPDX-License-Identifier: MIT-0 */
static bool @NAME@_get(const @NAME@ *v, size_t pos, @TYPE@ *out) {
    if (pos >= v->len) return false;
    if (out) *out = v->data[pos];
    return true;
}
static bool @NAME@_set(@NAME@ *v, size_t pos, @TYPE@ value) {
    if (pos >= v->len) return false;
    v->data[pos] = value;
    return true;
}
static bool @NAME@_front(const @NAME@ *v, @TYPE@ *out) {
    return @NAME@_get(v, 0, out);
}
static bool @NAME@_back(const @NAME@ *v, @TYPE@ *out) {
    return v->len && @NAME@_get(v, v->len - 1, out);
}
/* Snapshot supports extending from the vector's own storage. */
static void @NAME@_extend(@NAME@ *v, @TYPE@ const *items, size_t count) {
    if (!count) return;
    size_t length = tb_add_size(v->len, count);
    @TYPE@ *copy = tb_resize(NULL, count, sizeof *copy);
    memcpy(copy, items, count * sizeof *copy);
    @NAME@_reserve(v, length);
    memcpy(v->data + v->len, copy, count * sizeof *copy);
    v->len = length;
    free(copy);
}
static void @NAME@_resize(@NAME@ *v, size_t length, @TYPE@ fill) {
    @NAME@_reserve(v, length);
    for (size_t i = v->len; i < length; ++i) v->data[i] = fill;
    v->len = length;
}
static void @NAME@_reverse(@NAME@ *v) {
    for (size_t i = 0; i < v->len / 2; ++i) {
        @TYPE@ value = v->data[i];
        v->data[i] = v->data[v->len - 1 - i];
        v->data[v->len - 1 - i] = value;
    }
}
static size_t @NAME@_size(const @NAME@ *v) { return v->len; }
static bool @NAME@_empty(const @NAME@ *v) { return v->len == 0; }
static @TYPE@ @NAME@_at(const @NAME@ *v, size_t pos) {
    if (pos >= v->len) tb_fail("vector index out of range");
    return v->data[pos];
}
static void @NAME@_fill(@NAME@ *v, @TYPE@ value) {
    for (size_t i = 0; i < v->len; ++i) v->data[i] = value;
}
static @NAME@ @NAME@_slice(const @NAME@ *v, size_t pos, size_t count) {
    if (pos > v->len || count > v->len - pos) tb_fail("invalid vector slice");
    @NAME@ result = @NAME@_new();
    if (count) @NAME@_extend(&result, v->data + pos, count);
    return result;
}
static void @NAME@_extend_vector(@NAME@ *v, const @NAME@ *other) {
    @NAME@_extend(v, other->data, other->len);
}
