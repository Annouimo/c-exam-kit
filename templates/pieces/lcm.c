long long lcm(long long a, long long b) {
    if (a == 0 || b == 0) return 0;
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    return a / gcd(a, b) * b;
}
