/* Shared validation for independent C99 algorithm components. */
static inline void exam_algo_fail(const char *message) {
    fprintf(stderr, "exam algorithm: %s\n", message);
    exit(EXIT_FAILURE);
}
static inline void exam_check_n(int n) {
    if (n < 0) exam_algo_fail("negative array length");
}
