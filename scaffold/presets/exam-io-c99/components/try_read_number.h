/** @category 输入和输出 */

/* Strict int token: invalid input and overflow are errors, only EOF returns false. */
/**
 * @public
 * @requires exam_io_fail exam_io_token
 * @category 输入和输出
 * @title 尝试读取一个数
 * @members try_read_int try_read_ll try_read_double
 * `try_read_int(&x)`、`try_read_ll(&x)`、`try_read_double(&x)`。读到返回 `true`，正常 EOF 返回 `false`
 * @example int x;
 * @example while (try_read_int(&x)) {
 * @example     print_int(x, "\n");
 * @example }
 */
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

/**
 * @public
 * @requires exam_io_fail exam_io_token
 */
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

/**
 * @public
 * @requires exam_io_fail exam_io_token
 */
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
