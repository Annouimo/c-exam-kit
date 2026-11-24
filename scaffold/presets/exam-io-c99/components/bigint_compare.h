/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_bigint_cmp_abs
 */
static inline int bigint_compare(const BigInt *a, const BigInt *b) {
    if (a->sign != b->sign) return (a->sign > b->sign) ? 1 : -1;
    if (!a->sign) return 0;
    int cmp = exam_bigint_cmp_abs(a, b);
    return a->sign > 0 ? cmp : -cmp;
}
