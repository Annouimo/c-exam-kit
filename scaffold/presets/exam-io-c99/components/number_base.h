/** @category 数学、数位与二分 */
static inline int exam_digit_value(int c) {
    if (c >= '0' && c <= '9') return c-'0';
    if (c >= 'a' && c <= 'z') return c-'a'+10;
    if (c >= 'A' && c <= 'Z') return c-'A'+10;
    return -1;
}
/**
 * @public
 * @requires exam_algo_fail exam_digit_value
 * @category 数学、数位与二分
 */
/**
 * @id base_conversion
 * @title 进制转换
 * @members base_to_int int_to_base int_to_base_into
 * base 为 2..36。base_to_int 将字符串解析为 long long；int_to_base 将整数写成指定进制的字符串，目标容量包含结束符。
 * @example
 * long long n = base_to_int("1010", 2); // 10
 * char out[70];
 * int_to_base(n, 16, out); // A
 */
static inline long long base_to_int(const char *s, int base) {
    if (base < 2 || base > 36) exam_algo_fail("base must be 2..36");
    bool negative = false;
    if (*s == '+' || *s == '-') { negative = (*s == '-'); ++s; }
    if (!*s) exam_algo_fail("missing digits");
    unsigned long long magnitude = 0;
    unsigned long long limit = (unsigned long long)LLONG_MAX + (negative ? 1ULL : 0ULL);
    for (; *s; ++s) {
        int digit = exam_digit_value((unsigned char)*s);
        if (digit < 0 || digit >= base) exam_algo_fail("invalid base digit");
        if (magnitude > (limit - (unsigned long long)digit)/(unsigned)base)
            exam_algo_fail("base conversion overflow");
        magnitude = magnitude*(unsigned)base + (unsigned)digit;
    }
    if (negative) {
        if (magnitude == (unsigned long long)LLONG_MAX+1ULL) return LLONG_MIN;
        return -(long long)magnitude;
    }
    return (long long)magnitude;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void int_to_base_into(long long value, int base,
                                    char *out, size_t capacity) {
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (base < 2 || base > 36 || !out || !capacity)
        exam_algo_fail("invalid base conversion arguments");
    char reversed[70];
    size_t len = 0;
    bool negative = value < 0;
    unsigned long long magnitude = negative
        ? 0ULL - (unsigned long long)value : (unsigned long long)value;
    do {
        reversed[len++] = digits[magnitude % (unsigned)base];
        magnitude /= (unsigned)base;
    } while (magnitude);
    if (negative) reversed[len++] = '-';
    if (len >= capacity) exam_algo_fail("base conversion output too small");
    for (size_t i = 0; i < len; ++i) out[i] = reversed[len-1-i];
    out[len] = '\0';
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id digit_stats
 * @title 数位统计与取位
 * @members int_digit_count int_digit_sum int_digit_at
 * 按指定进制统计数位数、数位和，或取指定数位。pos 从最低位开始编号 0。
 * @example
 * int n = int_digit_count(123, 10); // 3
 * int s = int_digit_sum(123, 10); // 6
 * int d = int_digit_at(123, 10, 0); // 3
 */
static inline int int_digit_count(long long n, int base) {
    if (base < 2 || base > 36) exam_algo_fail("base must be 2..36");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    int count = 1;
    while (x >= (unsigned)base) { x /= (unsigned)base; ++count; }
    return count;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int int_digit_sum(long long n, int base) {
    if (base < 2 || base > 36) exam_algo_fail("base must be 2..36");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    int total = 0;
    do { total += (int)(x % (unsigned)base); x /= (unsigned)base; } while (x);
    return total;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int int_digit_at(long long n, int base, int position) {
    if (base < 2 || base > 36 || position < 0)
        exam_algo_fail("invalid digit position/base");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    while (position-- && x) x /= (unsigned)base;
    return (int)(x % (unsigned)base);
}
/**
 * @public
 * @requires int_to_base_into
 */
#define int_to_base(value, base, array) \
    int_to_base_into((value),(base),(array),sizeof(array))
