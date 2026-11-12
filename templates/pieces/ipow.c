long long ipow(long long base, int exp) {
    long long result = 1;
    for (int i = 0; i < exp; i++) result *= base;
    return result;
}
