static inline int exam_digit_value(int c) {
    if (c >= '0' && c <= '9') return c-'0';
    if (c >= 'a' && c <= 'z') return c-'a'+10;
    if (c >= 'A' && c <= 'Z') return c-'A'+10;
    return -1;
}
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
static inline int int_digit_count(long long n, int base) {
    if (base < 2 || base > 36) exam_algo_fail("base must be 2..36");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    int count = 1;
    while (x >= (unsigned)base) { x /= (unsigned)base; ++count; }
    return count;
}
static inline int int_digit_sum(long long n, int base) {
    if (base < 2 || base > 36) exam_algo_fail("base must be 2..36");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    int total = 0;
    do { total += (int)(x % (unsigned)base); x /= (unsigned)base; } while (x);
    return total;
}
static inline int int_digit_at(long long n, int base, int position) {
    if (base < 2 || base > 36 || position < 0)
        exam_algo_fail("invalid digit position/base");
    unsigned long long x = n < 0 ? 0ULL-(unsigned long long)n : (unsigned long long)n;
    while (position-- && x) x /= (unsigned)base;
    return (int)(x % (unsigned)base);
}
#define int_to_base(value, base, array) \
    int_to_base_into((value),(base),(array),sizeof(array))
