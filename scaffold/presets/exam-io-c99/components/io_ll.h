static inline bool try_read_ll(long long *value) {
    if (!value) exam_io_fail("null long long destination");
    char token[128];
    if (!exam_io_token(token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    long long result = strtoll(token, &end, 10);
    if (end == token || *end != '\0') exam_io_fail("invalid long long");
    if (errno == ERANGE) exam_io_fail("long long out of range");
    *value = result;
    return true;
}
static inline long long read_ll(void) {
    long long value;
    if (!try_read_ll(&value)) exam_io_fail("unexpected EOF while reading long long");
    return value;
}
