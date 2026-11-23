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
#define sieve_primes(array, limit) \
    sieve_primes_into((array), (limit), sizeof(array) / sizeof((array)[0]))
