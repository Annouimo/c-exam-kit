static inline bool try_read_double(double *value) {
    if (!value) exam_io_fail("null double destination");
    char token[128];
    if (!exam_io_token(token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    double result = strtod(token, &end);
    if (end == token || *end != '\0') exam_io_fail("invalid double");
    if (errno == ERANGE || !isfinite(result)) exam_io_fail("double out of range");
    *value = result;
    return true;
}
static inline double read_double(void) {
    double value;
    if (!try_read_double(&value)) exam_io_fail("unexpected EOF while reading double");
    return value;
}
