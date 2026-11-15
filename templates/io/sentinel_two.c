int main(void) {
    VALUE a, b;
    while (scanf(VALUE_SCAN " " VALUE_SCAN, &a, &b) == 2) {
        if (a == 0 && b == 0) break;
@LOOP@
    }
    return 0;
}
