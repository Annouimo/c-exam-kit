/** @category 输入和输出 */

/* Strict int token: invalid input and overflow are errors, only EOF returns false. */
/**
 * @public
 * @requires exam_io_fail exam_io_token
 * @category 输入和输出
 */
/**
 * @id try_read_number
 * @title 尝试读取一个数
 * @keywords EOF 循环 输入
 * @variant int try_read_int
 * @variant ll try_read_ll
 * @variant double try_read_double
 * 读到数时返回 `true`，正常 EOF 返回 `false`。传入存放结果的变量地址。输入无效时结束程序。
 * @example
 * {{type}} x;
 * while ({{fn1}}(&x)) {
 *     // 使用 x
 * }
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
