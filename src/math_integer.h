/** @category 数学、数位与二分 */
static inline unsigned long long exam_abs_ull(long long a) {
    return a >= 0 ? (unsigned long long)a : 0ULL - (unsigned long long)a;
}
/**
 * @public
 * @requires exam_abs_ull
 * @category 数学、数位与二分
 */
/**
 * @id gcd_lcm
 * @title 最大公约数与最小公倍数
 * @members gcd lcm
 * 返回非负的 unsigned long long 结果。输入可以为负数。
 * @example
 * unsigned long long g = gcd(12, 18); // 6
 * unsigned long long l = lcm(12, 18); // 36
 */
static inline unsigned long long gcd(long long a, long long b) {
    unsigned long long x = exam_abs_ull(a), y = exam_abs_ull(b);
    while (y) { unsigned long long r = x % y; x = y; y = r; }
    return x;
}
/**
 * @public
 * @requires exam_abs_ull gcd exam_algo_fail
 */
static inline unsigned long long lcm(long long a, long long b) {
    unsigned long long x = exam_abs_ull(a), y = exam_abs_ull(b);
    if (!x || !y) return 0;
    unsigned long long k = x / gcd(a, b);
    if (k > ULLONG_MAX / y) exam_algo_fail("lcm overflow");
    return k * y;
}
/** @requires exam_algo_fail */
static inline long long exam_mul_ll(long long a, long long b) {
    if ((a > 0 && b > 0 && a > LLONG_MAX / b) ||
        (a > 0 && b < 0 && b < LLONG_MIN / a) ||
        (a < 0 && b > 0 && a < LLONG_MIN / b) ||
        (a < 0 && b < 0 && a < LLONG_MAX / b))
        exam_algo_fail("signed integer multiplication overflow");
    return a * b;
}
/** @requires exam_algo_fail */
static inline long long exam_add_ll(long long a, long long b) {
    if ((b > 0 && a > LLONG_MAX - b) ||
        (b < 0 && a < LLONG_MIN - b))
        exam_algo_fail("signed integer addition overflow");
    return a + b;
}
/**
 * @public
 * @requires exam_mul_ll
 */
/**
 * @id integer_power
 * @title 整数幂与模幂
 * @members int_pow mod_pow
 * 指数为非负整数；mod_pow 的模数为正，适合大指数取模。int_pow 的结果必须在 long long 范围内。
 * @example
 * long long x = int_pow(2, 10); // 1024
 * long long y = mod_pow(2, 10, 7); // 2
 */
static inline long long int_pow(long long base, unsigned int exponent) {
    long long result = 1;
    while (exponent) {
        if (exponent & 1U) result = exam_mul_ll(result, base);
        exponent >>= 1U;
        if (exponent) base = exam_mul_ll(base, base);
    }
    return result;
}
/**
 * @public
 * @requires exam_algo_fail
 * @category 数学、数位与二分
 */
/**
 * @id factorial_basic
 * @title 阶乘
 * @members factorial_basic
 * @related bigint_factorial
 * 计算 n!，n 范围 0..20，返回 long long。更大的阶乘使用 bigint_factorial。
 * @example
 * long long f = factorial_basic(5); // 120
 */
static inline long long factorial_basic(int n) {
    if (n < 0 || n > 20) exam_algo_fail("factorial requires 0 <= n <= 20");
    long long result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}
/** @public */
/**
 * @id prime_test
 * @title 质数判断
 * @members is_prime
 * @related prime_sieve
 * 判断单个整数是否为质数，小于 2 返回 false。多次查询可使用筛法。
 * @example
 * bool p = is_prime(97);
 */
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
/** @requires exam_mod_add */
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
/**
 * @public
 * @requires exam_algo_fail exam_mod_mul
 */
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
