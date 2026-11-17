/* SPDX-License-Identifier: MIT-0 */
static double @NAME@_sum(const @NAME@ *v) {
    long double total = 0;
    for (size_t i = 0; i < v->len; ++i) total += (long double)v->data[i];
    double result = (double)total;
    if (!isfinite(result)) tb_fail("floating sum outside double range");
    return result;
}
