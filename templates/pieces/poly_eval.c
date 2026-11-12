long long poly_eval(long long a[], int degree, long long x) {
    long long result = 0;
    for (int i = degree; i >= 0; i--) result = result * x + a[i];
    return result;
}
