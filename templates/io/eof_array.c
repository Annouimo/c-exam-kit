int main(void) {
    static VALUE a[ARRAY_MAX];
    int n;
    while (scanf("%d", &n) == 1) {
        if (n < 0 || n > ARRAY_MAX) return 0;
        for (int i = 0; i < n; i++) {
            if (scanf(VALUE_SCAN, &a[i]) != 1) return 0;
        }
@LOOP@
    }
    return 0;
}
