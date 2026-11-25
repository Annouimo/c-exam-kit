/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail EXAM_BIGINT_CAP exam_io_token bigint_from_str
 */
static inline bool try_read_bigint(BigInt *out) {
    if (!out) exam_algo_fail("null BigInt destination");
    char buffer[EXAM_BIGINT_CAP+2];
    if (!exam_io_token("try_read_bigint", buffer, sizeof buffer)) return false;
    bigint_from_str(out, buffer);
    return true;
}
/**
 * @public
 * @requires BigInt try_read_bigint exam_algo_fail
 */
/**
 * @id bigint_io
 * @title 大整数输入输出
 * @members read_bigint try_read_bigint bigint_print bigint_to_str bigint_to_str_into
 * @related bigint_create
 * 传入 BigInt 地址。try_read_bigint 在正常 EOF 返回 false；bigint_print 追加 end；bigint_to_str 写入足够大的字符数组（含符号和结束符）。
 * @example
 * BigInt a;
 * read_bigint(&a);
 * bigint_print(&a, "\n");
 */
static inline void read_bigint(BigInt *out) {
    if (!try_read_bigint(out)) exam_algo_fail("unexpected EOF reading BigInt");
}
