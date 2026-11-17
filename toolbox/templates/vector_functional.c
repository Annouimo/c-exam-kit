/* SPDX-License-Identifier: MIT-0 */
/* Eager, same-type operations. Element copies follow the vector's shallow policy. */
static @NAME@ @NAME@_from(@TYPE@ const *items, size_t count) {
    @NAME@ result = @NAME@_new();
    @NAME@_extend(&result, items, count);
    return result;
}
/* At least one value; use _new() for empty. Arguments are evaluated once. */
#define @NAME@_values(...) @NAME@_from((@TYPE@ const[]){__VA_ARGS__}, \
    sizeof((@TYPE@ const[]){__VA_ARGS__}) / sizeof(@TYPE@))
static @NAME@ @NAME@_filter(const @NAME@ *v, bool (*predicate)(@TYPE@)) {
    if (!predicate) tb_fail("missing predicate");
    @NAME@ result = @NAME@_new();
    for (size_t i = 0; i < v->len; ++i)
        if (predicate(v->data[i])) @NAME@_push(&result, v->data[i]);
    return result;
}
static @NAME@ @NAME@_map(const @NAME@ *v, @TYPE@ (*transform)(@TYPE@)) {
    if (!transform) tb_fail("missing transform");
    @NAME@ result = @NAME@_new();
    @NAME@_reserve(&result, v->len);
    for (size_t i = 0; i < v->len; ++i) @NAME@_push(&result, transform(v->data[i]));
    return result;
}
static void @NAME@_map_inplace(@NAME@ *v, @TYPE@ (*transform)(@TYPE@)) {
    if (!transform) tb_fail("missing transform");
    for (size_t i = 0; i < v->len; ++i) v->data[i] = transform(v->data[i]);
}
static size_t @NAME@_find_if(const @NAME@ *v, bool (*predicate)(@TYPE@)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) return i;
    return SIZE_MAX;
}
static size_t @NAME@_count_if(const @NAME@ *v, bool (*predicate)(@TYPE@)) {
    if (!predicate) tb_fail("missing predicate");
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (predicate(v->data[i])) ++count;
    return count;
}
static bool @NAME@_any(const @NAME@ *v, bool (*predicate)(@TYPE@)) {
    return @NAME@_find_if(v, predicate) != SIZE_MAX;
}
static bool @NAME@_all(const @NAME@ *v, bool (*predicate)(@TYPE@)) {
    if (!predicate) tb_fail("missing predicate");
    for (size_t i = 0; i < v->len; ++i) if (!predicate(v->data[i])) return false;
    return true;
}
static @TYPE@ @NAME@_reduce(const @NAME@ *v, @TYPE@ initial,
                           @TYPE@ (*combine)(@TYPE@, @TYPE@)) {
    if (!combine) tb_fail("missing reducer");
    @TYPE@ result = initial;
    for (size_t i = 0; i < v->len; ++i) result = combine(result, v->data[i]);
    return result;
}
