long long fib_memo(int n) {
    if (n <= 1) return n;
    if (has[n]) return memo[n];
    memo[n] = fib_memo(n - 1) + fib_memo(n - 2);
    has[n] = 1;
    return memo[n];
}
