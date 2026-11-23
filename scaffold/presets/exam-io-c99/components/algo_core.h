/* Shared validation for independent C99 algorithm components. */
static inline void exam_algo_fail(const char *message) {
    fprintf(stderr, "exam algorithm: %s\n", message);
    exit(EXIT_FAILURE);
}
static inline void exam_check_n(int n) {
    if (n < 0) exam_algo_fail("negative array length");
}
static inline long long exam_add_checked_ll(long long a, long long b) {
    if ((b > 0 && a > LLONG_MAX - b) ||
        (b < 0 && a < LLONG_MIN - b))
        exam_algo_fail("long long addition overflow");
    return a + b;
}
static inline long long exam_sub_checked_ll(long long a, long long b) {
    if ((b > 0 && a < LLONG_MIN + b) ||
        (b < 0 && a > LLONG_MAX + b))
        exam_algo_fail("long long subtraction overflow");
    return a - b;
}
