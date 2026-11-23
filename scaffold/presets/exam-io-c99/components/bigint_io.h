static inline bool try_read_bigint(BigInt *out) {
    if (!out) exam_algo_fail("null BigInt destination");
    char buffer[EXAM_BIGINT_CAP+2];
    if (!exam_io_token(buffer, sizeof buffer)) return false;
    bigint_from_str(out, buffer);
    return true;
}
static inline void read_bigint(BigInt *out) {
    if (!try_read_bigint(out)) exam_algo_fail("unexpected EOF reading BigInt");
}
