/** @category 数学、数位与二分 */
/** @requires exam_algo_fail */
static inline int exam_bit_check(int bit) {
    if (bit < 0 || bit >= 64) exam_algo_fail("bit index must be 0..63");
    return bit;
}
/**
 * @public
 * @requires exam_bit_check
 * @category 数学、数位与二分
 * @title 位运算
 * @members bit_get bit_set bit_clear bit_toggle bit_count is_power_of_two
 * `bit_get(x, i)`、`bit_set(x, i)`、`bit_clear(x, i)`、`bit_toggle(x, i)`。最低位编号为 `0`，后三者返回修改后的值。`bit_count(x)` 统计置位数，`is_power_of_two(x)` 判断是否为二的幂
 */
static inline int bit_get(unsigned long long x, int bit) {
    return (int)((x >> exam_bit_check(bit)) & 1ULL);
}
/**
 * @public
 * @requires exam_bit_check
 */
static inline unsigned long long bit_set(unsigned long long x, int bit) {
    return x | (1ULL << exam_bit_check(bit));
}
/**
 * @public
 * @requires exam_bit_check
 */
static inline unsigned long long bit_clear(unsigned long long x, int bit) {
    return x & ~(1ULL << exam_bit_check(bit));
}
/**
 * @public
 * @requires exam_bit_check
 */
static inline unsigned long long bit_toggle(unsigned long long x, int bit) {
    return x ^ (1ULL << exam_bit_check(bit));
}
/** @public */
static inline int bit_count(unsigned long long x) {
    int count = 0;
    while (x) { x &= x - 1ULL; ++count; }
    return count;
}
/** @public */
static inline bool is_power_of_two(unsigned long long x) {
    return x && !(x & (x - 1ULL));
}
/** @public */
static inline int bit_lowest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (!(x & 1ULL)) { x >>= 1U; ++k; }
    return k;
}
/** @public */
static inline int bit_highest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (x >>= 1U) ++k;
    return k;
}
/** @public */
static inline unsigned long long bit_rotate_left(unsigned long long x, int count) {
    unsigned int n = (unsigned int)((count % 64 + 64) % 64);
    if (n == 0) return x;
    return (x << n) | (x >> (64U - n));
}
