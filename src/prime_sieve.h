/**
 * @public
 * @requires exam_algo_fail
 */
static inline void sieve_primes_into(unsigned char *flags, int limit,
                                     size_t capacity) {
    if (limit < 0 || (size_t)limit >= capacity || !flags)
        exam_algo_fail("prime sieve exceeds buffer capacity");
    for (int i = 0; i <= limit; ++i) flags[i] = (i >= 2);
    for (int p = 2; p <= limit / p; ++p) if (flags[p]) {
        for (int j = p * p; j <= limit;) {
            flags[j] = 0;
            if (j > limit - p) break;
            j += p;
        }
    }
}
/**
 * @public
 * @requires sieve_primes_into
 * @id prime_sieve
 * @title 质数筛法
 * @tags 整数 数学计算 数组
 * @summary 批量标记指定范围内的质数。
 * @pre flags 至少有 limit+1 项。
 * @keywords 筛法 素数 埃氏筛
 * @members sieve_primes sieve_primes_into
 * @related prime_test
 * 标记 0..limit 是否为质数，flags 为至少 limit+1 项 unsigned char 数组。
 * @example
 * unsigned char flags[101];
 * sieve_primes(flags, 100);
 * if (flags[97]) print_text("prime", "\n");
 */
#define sieve_primes(array, limit) \
    sieve_primes_into((array), (limit), sizeof(array) / sizeof((array)[0]))
