static inline unsigned long long exam_abs_ull(long long a) {
    return a >= 0 ? (unsigned long long)a : 0ULL - (unsigned long long)a;
}
static inline unsigned long long gcd(long long a, long long b) {
    unsigned long long x = exam_abs_ull(a), y = exam_abs_ull(b);
    while (y) { unsigned long long r = x % y; x = y; y = r; }
    return x;
}
static inline unsigned long long lcm(long long a, long long b) {
    unsigned long long x = exam_abs_ull(a), y = exam_abs_ull(b);
    if (!x || !y) return 0;
    unsigned long long k = x / gcd(a, b);
    if (k > ULLONG_MAX / y) exam_algo_fail("lcm overflow");
    return k * y;
}
static inline long long exam_mul_ll(long long a, long long b) {
    if ((a > 0 && b > 0 && a > LLONG_MAX / b) ||
        (a > 0 && b < 0 && b < LLONG_MIN / a) ||
        (a < 0 && b > 0 && a < LLONG_MIN / b) ||
        (a < 0 && b < 0 && a < LLONG_MAX / b))
        exam_algo_fail("signed integer multiplication overflow");
    return a * b;
}
static inline long long exam_add_ll(long long a, long long b) {
    if ((b > 0 && a > LLONG_MAX - b) ||
        (b < 0 && a < LLONG_MIN - b))
        exam_algo_fail("signed integer addition overflow");
    return a + b;
}
static inline long long int_pow(long long base, unsigned int exponent) {
    long long result = 1;
    while (exponent) {
        if (exponent & 1U) result = exam_mul_ll(result, base);
        exponent >>= 1U;
        if (exponent) base = exam_mul_ll(base, base);
    }
    return result;
}
static inline long long factorial_basic(int n) {
    if (n < 0 || n > 20) exam_algo_fail("factorial requires 0 <= n <= 20");
    long long result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}
static inline bool is_prime(long long n) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (long long d = 3; d <= n / d; d += 2)
        if (n % d == 0) return false;
    return true;
}
static inline unsigned long long exam_mod_add(unsigned long long a,
                                              unsigned long long b,
                                              unsigned long long mod) {
    return a >= mod - b ? a - (mod - b) : a + b;
}
static inline unsigned long long exam_mod_mul(unsigned long long a,
                                              unsigned long long b,
                                              unsigned long long mod) {
    unsigned long long result = 0;
    while (b) {
        if (b & 1ULL) result = exam_mod_add(result, a, mod);
        b >>= 1U;
        if (b) a = exam_mod_add(a, a, mod);
    }
    return result;
}
static inline long long mod_pow(long long base, unsigned long long exponent,
                                long long modulus) {
    if (modulus <= 0) exam_algo_fail("modulus must be positive");
    unsigned long long mod = (unsigned long long)modulus;
    long long reduced = base % modulus;
    if (reduced < 0) reduced += modulus;
    unsigned long long x = (unsigned long long)reduced, result = 1ULL % mod;
    while (exponent) {
        if (exponent & 1ULL) result = exam_mod_mul(result, x, mod);
        exponent >>= 1U;
        if (exponent) x = exam_mod_mul(x, x, mod);
    }
    return (long long)result;
}
