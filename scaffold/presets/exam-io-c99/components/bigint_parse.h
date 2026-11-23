static inline void bigint_from_str(BigInt *out, const char *text) {
    if (!text || !out) exam_algo_fail("null BigInt string/destination");
    bool negative = false;
    if (*text == '+' || *text == '-') { negative = *text == '-'; ++text; }
    if (!*text) exam_algo_fail("BigInt has no digits");
    size_t len = strlen(text);
    if (len > EXAM_BIGINT_CAP) exam_algo_fail("BigInt input too long");
    BigInt temp;
    bigint_zero(&temp);
    temp.len = (int)len;
    temp.sign = negative ? -1 : 1;
    for (size_t i = 0; i < len; ++i) {
        char c = text[len-1-i];
        if (c < '0' || c > '9') exam_algo_fail("invalid BigInt digit");
        temp.d[i] = (unsigned char)(c-'0');
    }
    exam_bigint_normalize(&temp);
    *out = temp;
}
static inline void bigint_from_ll(BigInt *out, long long value) {
    if (!out) exam_algo_fail("null BigInt destination");
    BigInt temp;
    bigint_zero(&temp);
    unsigned long long x = value < 0 ? 0ULL-(unsigned long long)value :
                                              (unsigned long long)value;
    int n = 0;
    do {
        if (n >= EXAM_BIGINT_CAP) exam_algo_fail("BigInt capacity too small");
        temp.d[n++] = (unsigned char)(x % 10ULL);
        x /= 10ULL;
    } while (x);
    temp.len = n;
    temp.sign = value == 0 ? 0 : (value < 0 ? -1 : 1);
    *out = temp;
}
static inline void bigint_to_str_into(const BigInt *a, char *out,
                                      size_t capacity) {
    if (!a || !out) exam_algo_fail("null BigInt string/destination");
    size_t required = (size_t)a->len + (a->sign < 0 ? 1U : 0U) + 1U;
    if (required > capacity) exam_algo_fail("BigInt string buffer too small");
    size_t pos = 0;
    if (a->sign < 0) out[pos++] = '-';
    for (int i = a->len - 1; i >= 0; --i) out[pos++] = (char)('0'+a->d[i]);
    out[pos] = '\0';
}
static inline void bigint_print(const BigInt *a, const char *end) {
    if (!a || !end) exam_algo_fail("null BigInt/terminator");
    if (a->sign < 0 && putchar('-') == EOF) exam_algo_fail("BigInt output error");
    for (int i = a->len - 1; i >= 0; --i)
        if (putchar('0' + a->d[i]) == EOF) exam_algo_fail("BigInt output error");
    if (fputs(end, stdout) == EOF) exam_algo_fail("BigInt output error");
}
#define bigint_to_str(a, array) \
    bigint_to_str_into((a), (array), sizeof(array))
