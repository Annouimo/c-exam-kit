int main(void) {
    VALUE x;
    while (scanf(VALUE_SCAN, &x) == 1) {
        if (x == -1) break;  @COMMENT:sentinel_0@
@LOOP@    }
    @COMMENT:sentinel_1@
    return 0;
}
