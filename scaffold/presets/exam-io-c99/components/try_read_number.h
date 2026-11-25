/** @category 输入和输出 */

/** @requires exam_failf */
static inline void exam_number_error(const char *api, const char *field,
                                     const char *reason, const char *token) {
    char preview[32 * 4 + 4];
    size_t j = 0;
    for (size_t i = 0; token[i]; ++i) {
        if (i == 32) {
            preview[j++] = '.';
            preview[j++] = '.';
            preview[j++] = '.';
            break;
        }
        unsigned char c = (unsigned char)token[i];
        if (c >= 32 && c <= 126 && c != '"' && c != '\\') {
            preview[j++] = (char)c;
        } else {
            const char *hex = "0123456789ABCDEF";
            preview[j++] = '\\';
            preview[j++] = 'x';
            preview[j++] = hex[c >> 4];
            preview[j++] = hex[c & 15];
        }
    }
    preview[j] = '\0';
    exam_failf("io", api, "%s%s%s (token=\"%s\")",
               field ? field : "", field ? ": " : "", reason, preview);
}


/* Strict int token: invalid input and overflow are errors, only EOF returns false. */
/** @requires exam_failf exam_io_token exam_number_error */
static inline bool exam_try_read_int(const char *api, const char *field, int *value) {
    if (!value) exam_failf("io", api, "destination must not be NULL");
    char token[128];
    if (!exam_io_token(api, token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    long long result = strtoll(token, &end, 10);
    if (end == token || *end != '\0') exam_number_error(api, field, "invalid integer", token);
    if (errno == ERANGE || result < INT_MIN || result > INT_MAX)
        exam_number_error(api, field, "integer out of range", token);
    *value = (int)result;
    return true;
}

/**
 * @public
 * @requires exam_try_read_int
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
    return exam_try_read_int("try_read_int", NULL, value);
}

/** @requires exam_failf exam_io_token exam_number_error */
static inline bool exam_try_read_ll(const char *api, const char *field, long long *value) {
    if (!value) exam_failf("io", api, "destination must not be NULL");
    char token[128];
    if (!exam_io_token(api, token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    long long result = strtoll(token, &end, 10);
    if (end == token || *end != '\0') exam_number_error(api, field, "invalid long long", token);
    if (errno == ERANGE) exam_number_error(api, field, "long long out of range", token);
    *value = result;
    return true;
}

/**
 * @public
 * @requires exam_try_read_ll
 */
static inline bool try_read_ll(long long *value) {
    return exam_try_read_ll("try_read_ll", NULL, value);
}

/** @requires exam_failf exam_io_token exam_number_error */
static inline bool exam_try_read_double(const char *api, const char *field, double *value) {
    if (!value) exam_failf("io", api, "destination must not be NULL");
    char token[128];
    if (!exam_io_token(api, token, sizeof token)) return false;
    char *end = NULL;
    errno = 0;
    double result = strtod(token, &end);
    if (end == token || *end != '\0') exam_number_error(api, field, "invalid double", token);
    if (errno == ERANGE || !isfinite(result)) exam_number_error(api, field, "double out of range", token);
    *value = result;
    return true;
}

/**
 * @public
 * @requires exam_try_read_double
 */
static inline bool try_read_double(double *value) {
    return exam_try_read_double("try_read_double", NULL, value);
}
