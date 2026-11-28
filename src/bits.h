/**
 * @requires exam_algo_fail
 */
static inline int exam_bit_check(int bit) {
    if (bit < 0 || bit >= 64) exam_algo_fail("bit index must be 0..63");
    return bit;
}
/**
 * @public
 * @requires exam_bit_check
 * @id bit_edit
 * @title 读取与修改二进制位
 * @tags 整数 位 修改
 * @summary 读取或修改无符号整数的指定二进制位。
 * @pre 位编号为 0..63。
 * @keywords 二进制 位运算 置位 清零 翻转
 * @members bit_get bit_set bit_clear bit_toggle
 * 对 unsigned long long 操作，最低位为 0，位编号 0..63。修改接口返回新值，不修改传入变量。
 * @example
 * unsigned long long x = 4;
 * int b = bit_get(x, 2); // 1
 * x = bit_set(x, 0); // 5
 * x = bit_clear(x, 2); // 1
 * x = bit_toggle(x, 0); // 0
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
/**
 * @public
 * @id bit_stats
 * @title 置位数与位置
 * @tags 整数 位 统计 判断
 * @summary 统计置位数、置位位置或判断二的幂。
 * @keywords 置位 二进制 1的个数 二的幂
 * @members bit_count is_power_of_two bit_lowest_set_index bit_highest_set_index
 * 统计为 1 的位数、判断二的幂，或返回最低/最高置位的编号。
 * @example
 * int n = bit_count(10); // 2
 * int lo = bit_lowest_set_index(10); // 1
 * int hi = bit_highest_set_index(10); // 3
 * bool p = is_power_of_two(8);
 */
static inline int bit_count(unsigned long long x) {
    int count = 0;
    while (x) { x &= x - 1ULL; ++count; }
    return count;
}
/**
 * @public
 */
static inline bool is_power_of_two(unsigned long long x) {
    return x && !(x & (x - 1ULL));
}
/**
 * @public
 */
static inline int bit_lowest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (!(x & 1ULL)) { x >>= 1U; ++k; }
    return k;
}
/**
 * @public
 */
static inline int bit_highest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (x >>= 1U) ++k;
    return k;
}
/**
 * @public
 * @id bit_rotate_left
 * @title 循环左移
 * @tags 整数 位 转换
 * @summary 对 64 位无符号整数循环左移。
 * @keywords 循环移位 左移
 * @members bit_rotate_left
 * 在 64 位无符号整数内循环左移，移出的位回到最低端。
 * @example
 * unsigned long long x = bit_rotate_left(1, 3); // 8
 */
static inline unsigned long long bit_rotate_left(unsigned long long x, int count) {
    unsigned int n = (unsigned int)((count % 64 + 64) % 64);
    if (n == 0) return x;
    return (x << n) | (x >> (64U - n));
}
