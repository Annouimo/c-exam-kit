/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_bigint_cmp_abs
 */
/**
 * @id bigint_compare
 * @title 大整数比较
 * @members bigint_compare
 * 比较两个大整数，返回 -1、0 或 1。
 * @example
 * BigInt a, b;
 * bigint_from_ll(&a, 5);
 * bigint_from_ll(&b, 7);
 * int cmp = bigint_compare(&a, &b); // -1
 */
static inline int bigint_compare(const BigInt *a, const BigInt *b) {
    if (a->sign != b->sign) return (a->sign > b->sign) ? 1 : -1;
    if (!a->sign) return 0;
    int cmp = exam_bigint_cmp_abs(a, b);
    return a->sign > 0 ? cmp : -cmp;
}
