static inline int exam_bit_check(int bit) {
    if (bit < 0 || bit >= 64) exam_algo_fail("bit index must be 0..63");
    return bit;
}
static inline int bit_get(unsigned long long x, int bit) {
    return (int)((x >> exam_bit_check(bit)) & 1ULL);
}
static inline unsigned long long bit_set(unsigned long long x, int bit) {
    return x | (1ULL << exam_bit_check(bit));
}
static inline unsigned long long bit_clear(unsigned long long x, int bit) {
    return x & ~(1ULL << exam_bit_check(bit));
}
static inline unsigned long long bit_toggle(unsigned long long x, int bit) {
    return x ^ (1ULL << exam_bit_check(bit));
}
static inline int bit_count(unsigned long long x) {
    int count = 0;
    while (x) { x &= x - 1ULL; ++count; }
    return count;
}
static inline bool is_power_of_two(unsigned long long x) {
    return x && !(x & (x - 1ULL));
}
static inline int bit_lowest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (!(x & 1ULL)) { x >>= 1U; ++k; }
    return k;
}
static inline int bit_highest_set_index(unsigned long long x) {
    if (!x) return -1;
    int k = 0;
    while (x >>= 1U) ++k;
    return k;
}
static inline unsigned long long bit_rotate_left(unsigned long long x, int count) {
    unsigned int n = (unsigned int)((count % 64 + 64) % 64);
    if (n == 0) return x;
    return (x << n) | (x >> (64U - n));
}
