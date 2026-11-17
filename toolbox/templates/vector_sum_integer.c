/* SPDX-License-Identifier: MIT-0 */
/* Exact sequential sum; fails before a signed overflow, including intermediate sums. */
static long long @NAME@_sum(const @NAME@ *v) {
    long long result = 0;
    for (size_t i = 0; i < v->len; ++i) {
        long long value = v->data[i];
        if ((value > 0 && result > LLONG_MAX - value) ||
            (value < 0 && result < LLONG_MIN - value)) tb_fail("integer sum overflow");
        result += value;
    }
    return result;
}
