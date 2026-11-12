BigInt big_factorial(int n) {
    BigInt result = big_zero();
    result.d[0] = 1;
    for (int i = 2; i <= n; i++) result = big_mul_small(result, i);
    return result;
}
