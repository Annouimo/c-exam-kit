void build_primes_sieve(int n) {
    memset(is_prime, 0, sizeof is_prime);
    pc = 0;
    for (int i = 2; i <= n; i++) is_prime[i] = 1;
    for (int i = 2; i <= n / i; i++) {
        if (is_prime[i])
            for (int j = i * i; j <= n; j += i) is_prime[j] = 0;
    }
    for (int i = 2; i <= n; i++)
        if (is_prime[i]) primes[pc++] = i;
}
