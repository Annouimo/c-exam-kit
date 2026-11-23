/* Strict int token: invalid input and overflow are errors, only EOF returns false. */
static inline bool try_read_int(int *value) {
    if (!value) exam_io_fail("null int destination");
    char token[128];
    if (!exam_io_token(token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    long long result = strtoll(token, &end, 10);
    if (end == token || *end != '\0') exam_io_fail("invalid integer");
    if (errno == ERANGE || result < INT_MIN || result > INT_MAX)
        exam_io_fail("integer out of range");
    *value = (int)result;
    return true;
}
static inline int read_int(void) {
    int value;
    if (!try_read_int(&value)) exam_io_fail("unexpected EOF while reading int");
    return value;
}
