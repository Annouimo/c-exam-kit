/** @category 输入和输出 */
/* Lightweight C99 IO: shared internal helpers. No allocation per token. */
static inline void exam_io_fail(const char *message) {
    fprintf(stderr, "exam io: %s\n", message);
    exit(EXIT_FAILURE);
}
static inline int exam_io_space(int c) {
    return c != EOF && isspace((unsigned char)c);
}
static inline int *exam_io_partial_line(void) {
    static int partial = 0;
    return &partial;
}
/** @requires exam_io_fail */
static inline int exam_io_get(void) {
    int c = fgetc(stdin);
    if (c == EOF && ferror(stdin)) exam_io_fail("input stream error");
    return c;
}
/** @requires exam_io_fail */
static inline void exam_io_unget(int c) {
    if (c != EOF && ungetc(c, stdin) == EOF)
        exam_io_fail("cannot restore input delimiter");
}
/* Return 0 only on clean EOF before a token. Keep the delimiter unread. */
/** @requires exam_failf exam_io_get exam_io_space exam_io_unget exam_io_partial_line */
static inline int exam_io_token(const char *api, char *out, size_t capacity) {
    if (!capacity) exam_failf("io", api, "buffer capacity is 0");
    int c;
    do { c = exam_io_get(); } while (exam_io_space(c));
    if (c == EOF) return 0;
    size_t len = 0;
    do {
        if (len >= capacity - 1) exam_failf("io", api, "token exceeds capacity=%zu (maximum content: %zu bytes)", capacity, capacity - 1);
        if (c == 0) exam_failf("io", api, "NUL byte in token at byte %zu", len);
        out[len++] = (char)c;
        c = exam_io_get();
    } while (c != EOF && !exam_io_space(c));
    exam_io_unget(c);
    out[len] = '\0';
    *exam_io_partial_line() = 1;
    return 1;
}
/* Consume the current line, including LF, CRLF or CR. */
/** @requires exam_io_get exam_io_unget exam_io_partial_line */
static inline void exam_io_skip_current_line(void) {
    int c;
    do { c = exam_io_get(); } while (c != EOF && c != '\n' && c != '\r');
    if (c == '\r') {
        int next = exam_io_get();
        if (next != '\n') exam_io_unget(next);
    }
    *exam_io_partial_line() = 0;
}
/* Read exactly the remainder of the current line; retain real empty lines. */
/** @requires exam_failf exam_io_get exam_io_unget exam_io_partial_line */
static inline int exam_io_line_buffer(const char *api, char *out, size_t capacity) {
    if (!capacity) exam_failf("io", api, "buffer capacity is 0");
    int c = exam_io_get();
    if (c == EOF) return 0;
    size_t len = 0;
    while (c != EOF && c != '\n' && c != '\r') {
        if (len >= capacity - 1) exam_failf("io", api, "line exceeds capacity=%zu (maximum content: %zu bytes)", capacity, capacity - 1);
        if (c == 0) exam_failf("io", api, "NUL byte in line at byte %zu", len);
        out[len++] = (char)c;
        c = exam_io_get();
    }
    if (c == '\r') {
        int next = exam_io_get();
        if (next != '\n') exam_io_unget(next);
    }
    out[len] = '\0';
    *exam_io_partial_line() = 0;
    return 1;
}
/** @requires exam_io_fail */
static inline void exam_io_write(const char *s) {
    if (fputs(s, stdout) == EOF) exam_io_fail("output stream error");
}
