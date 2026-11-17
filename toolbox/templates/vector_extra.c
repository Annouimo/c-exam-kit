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
