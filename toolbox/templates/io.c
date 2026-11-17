/* SPDX-License-Identifier: MIT-0 */
#include <errno.h>
#include <math.h>

typedef enum {
    IO_OK = 0, IO_EOF, IO_FORMAT, IO_RANGE, IO_STREAM_ERROR,
    IO_EXTRA_DATA
} IOStatus;
/* One reader per FILE. Do not mix other input APIs on the same stream. */
typedef struct { FILE *stream; bool line_pending; } IOReader;
static IOReader io_reader(FILE *stream) {
    IOReader r = {stream, false};
    return r;
}
static IOReader *tb_input(void) {
    static IOReader reader = {NULL, false};
    if (!reader.stream) reader.stream = stdin;
    return &reader;
}
static const char *io_status_message(IOStatus status) {
    switch (status) {
        case IO_OK: return "success";
        case IO_EOF: return "unexpected end of input";
        case IO_FORMAT: return "invalid input token";
        case IO_RANGE: return "number outside supported range";
        case IO_STREAM_ERROR: return "stream read/write error";
        case IO_EXTRA_DATA:
            return "extra text after token: use read_rest() to keep it, or discard_line() intentionally";
    }
    return "unknown IO status";
}
static void io_require(IOStatus status) {
    if (status != IO_OK) tb_fail(io_status_message(status));
}
static IOStatus tb_io_unget(IOReader *r, int ch) {
    return ch == EOF || ungetc(ch, r->stream) != EOF ? IO_OK : IO_STREAM_ERROR;
}
/* Token mode skips leading whitespace, but leaves the terminating delimiter. */
static IOStatus io_read_word(IOReader *r, String *out) {
    String text = string_new();
    int ch;
    do { ch = fgetc(r->stream); } while (ch != EOF && isspace((unsigned char)ch));
    if (ch == EOF) return ferror(r->stream) ? IO_STREAM_ERROR : IO_EOF;
    bool valid = true;
    do {
        if (!ch) valid = false;
        else string_push(&text, (char)ch);
        ch = fgetc(r->stream);
    } while (ch != EOF && !isspace((unsigned char)ch));
    r->line_pending = true;
    IOStatus status = ferror(r->stream) ? IO_STREAM_ERROR : tb_io_unget(r, ch);
    if (status == IO_OK && !valid) status = IO_FORMAT;
    if (status != IO_OK) { string_free(&text); return status; }
    string_free(out); *out = text;
    return IO_OK;
}
/* Physical remainder: preserves whitespace and blank lines; strips LF/CRLF/CR. */
static IOStatus io_read_rest(IOReader *r, String *out) {
    String text = string_new();
    bool any = false, valid = true;
    int ch;
    while ((ch = fgetc(r->stream)) != EOF) {
        any = true;
        if (ch == '\n') break;
        if (ch == '\r') {
            int next = fgetc(r->stream);
            if (next != '\n' && tb_io_unget(r, next) != IO_OK) {
                string_free(&text); return IO_STREAM_ERROR;
            }
            break;
        }
        if (!ch) valid = false;
        else string_push(&text, (char)ch);
    }
    r->line_pending = false;
    IOStatus status = ferror(r->stream) ? IO_STREAM_ERROR :
                      !any ? IO_EOF : !valid ? IO_FORMAT : IO_OK;
    if (status != IO_OK) { string_free(&text); return status; }
    string_free(out); *out = text;
    return IO_OK;
}
/* Default: ignore blank lines and trim outer whitespace; retain inner spaces. */
static IOStatus io_read_line(IOReader *r, String *out) {
    String line = string_new();
    IOStatus status;
    while ((status = io_read_rest(r, &line)) == IO_OK) {
        string_trim(&line);
        if (line.len) { string_free(out); *out = line; return IO_OK; }
    }
    string_free(&line);
    return status;
}
static IOStatus io_read_line_raw(IOReader *r, String *out) {
    return io_read_rest(r, out);
}
/* Consumes only horizontal whitespace and one line ending.
   On extra text, leaves its first nonspace byte for a subsequent read_rest. */
static IOStatus io_finish_line(IOReader *r) {
    if (!r->line_pending) return IO_OK;
    int ch;
    while ((ch = fgetc(r->stream)) != EOF) {
        if (ch == '\n') { r->line_pending = false; return IO_OK; }
        if (ch == '\r') {
            int next = fgetc(r->stream);
            IOStatus status = next == '\n' ? IO_OK : tb_io_unget(r, next);
            if (ferror(r->stream)) status = IO_STREAM_ERROR;
            if (status == IO_OK) r->line_pending = false;
            return status;
        }
        if (!isspace((unsigned char)ch)) {
            IOStatus status = tb_io_unget(r, ch);
            return status == IO_OK ? IO_EXTRA_DATA : status;
        }
    }
    if (ferror(r->stream)) return IO_STREAM_ERROR;
    r->line_pending = false;
    return IO_OK;
}
static IOStatus io_read_next_line(IOReader *r, String *out) {
    IOStatus status = io_finish_line(r);
    return status == IO_OK ? io_read_line(r, out) : status;
}
static IOStatus io_read_next_line_raw(IOReader *r, String *out) {
    IOStatus status = io_finish_line(r);
    return status == IO_OK ? io_read_line_raw(r, out) : status;
}
static IOStatus io_discard_line(IOReader *r) {
    String ignored = string_new();
    IOStatus status = io_read_rest(r, &ignored);
    string_free(&ignored);
    return status;
}
/* Raw byte input, including whitespace. Unlike line mode, CRLF is two bytes. */
static IOStatus io_read_char_raw(IOReader *r, char *out) {
    int ch = fgetc(r->stream);
    if (ch == EOF) return ferror(r->stream) ? IO_STREAM_ERROR : IO_EOF;
    r->line_pending = ch != '\n';
    *out = (char)ch;
    return IO_OK;
}
static IOStatus io_read_char(IOReader *r, char *out) {
    int ch;
    do { ch = fgetc(r->stream); } while (ch != EOF && isspace((unsigned char)ch));
    if (ch == EOF) return ferror(r->stream) ? IO_STREAM_ERROR : IO_EOF;
    r->line_pending = true;
    *out = (char)ch;
    return IO_OK;
}
/* Strict whole-text conversion; leading/trailing whitespace is accepted. */
static IOStatus parse_ll(const char *text, long long *out) {
    char *end;
    errno = 0;
    long long value = strtoll(text, &end, 10);
    int error = errno;
    if (end == text) return IO_FORMAT;
    while (isspace((unsigned char)*end)) ++end;
    if (*end) return IO_FORMAT;
    if (error == ERANGE) return IO_RANGE;
    *out = value; return IO_OK;
}
static IOStatus parse_int(const char *text, int *out) {
    long long value;
    IOStatus status = parse_ll(text, &value);
    if (status != IO_OK) return status;
    if (value < INT_MIN || value > INT_MAX) return IO_RANGE;
    *out = (int)value; return IO_OK;
}
static IOStatus parse_double(const char *text, double *out) {
    char *end;
    errno = 0;
    double value = strtod(text, &end);
    int error = errno;
    if (end == text) return IO_FORMAT;
    while (isspace((unsigned char)*end)) ++end;
    if (*end) return IO_FORMAT;
    if (error == ERANGE || !isfinite(value)) return IO_RANGE;
    *out = value; return IO_OK;
}
static IOStatus io_read_ll(IOReader *r, long long *out) {
    String token = string_new();
    IOStatus status = io_read_word(r, &token);
    if (status == IO_OK) status = parse_ll(string_cstr(&token), out);
    string_free(&token); return status;
}
static IOStatus io_read_int(IOReader *r, int *out) {
    String token = string_new();
    IOStatus status = io_read_word(r, &token);
    if (status == IO_OK) status = parse_int(string_cstr(&token), out);
    string_free(&token); return status;
}
static IOStatus io_read_double(IOReader *r, double *out) {
    String token = string_new();
    IOStatus status = io_read_word(r, &token);
    if (status == IO_OK) status = parse_double(string_cstr(&token), out);
    string_free(&token); return status;
}
static IOStatus try_read_int(int *out) { return io_read_int(tb_input(), out); }
static IOStatus try_read_ll(long long *out) { return io_read_ll(tb_input(), out); }
static IOStatus try_read_double(double *out) { return io_read_double(tb_input(), out); }
static IOStatus try_read_word(String *out) { return io_read_word(tb_input(), out); }
static IOStatus try_read_line(String *out) { return io_read_line(tb_input(), out); }
static IOStatus try_read_line_raw(String *out) { return io_read_line_raw(tb_input(), out); }
static IOStatus try_read_rest(String *out) { return io_read_rest(tb_input(), out); }
static IOStatus try_read_next_line(String *out) { return io_read_next_line(tb_input(), out); }
static IOStatus try_read_next_line_raw(String *out) { return io_read_next_line_raw(tb_input(), out); }
static IOStatus try_read_char(char *out) { return io_read_char(tb_input(), out); }
static IOStatus try_read_char_raw(char *out) { return io_read_char_raw(tb_input(), out); }
static int read_int(void) { int value; io_require(try_read_int(&value)); return value; }
static long long read_ll(void) { long long value; io_require(try_read_ll(&value)); return value; }
static double read_double(void) { double value; io_require(try_read_double(&value)); return value; }
static char read_char(void) { char value; io_require(try_read_char(&value)); return value; }
static char read_char_raw(void) { char value; io_require(try_read_char_raw(&value)); return value; }
static String read_word(void) {
    String s = string_new(); io_require(try_read_word(&s)); return s;
}
static String read_line(void) {
    String s = string_new(); io_require(try_read_line(&s)); return s;
}
static String read_line_raw(void) {
    String s = string_new(); io_require(try_read_line_raw(&s)); return s;
}
static String read_rest(void) {
    String s = string_new(); io_require(try_read_rest(&s)); return s;
}
static String read_next_line(void) {
    String s = string_new(); io_require(try_read_next_line(&s)); return s;
}
static String read_next_line_raw(void) {
    String s = string_new(); io_require(try_read_next_line_raw(&s)); return s;
}
static void finish_line(void) { io_require(io_finish_line(tb_input())); }
static void discard_line(void) {
    IOStatus status = io_discard_line(tb_input());
    if (status != IO_EOF) io_require(status);
}
static IntVector read_ints(size_t count) {
    IntVector v = IntVector_new();
    IntVector_reserve(&v, count);
    for (size_t i = 0; i < count; ++i) IntVector_push(&v, read_int());
    return v;
}
static LongLongVector read_lls(size_t count) {
    LongLongVector v = LongLongVector_new();
    LongLongVector_reserve(&v, count);
    for (size_t i = 0; i < count; ++i) LongLongVector_push(&v, read_ll());
    return v;
}
static DoubleVector read_doubles(size_t count) {
    DoubleVector v = DoubleVector_new();
    DoubleVector_reserve(&v, count);
    for (size_t i = 0; i < count; ++i) DoubleVector_push(&v, read_double());
    return v;
}
static StringList read_words(size_t count) {
    StringList words = {0};
    if (count) words.data = tb_resize(NULL, count, sizeof *words.data);
    words.cap = count;
    for (size_t i = 0; i < count; ++i) words.data[words.len++] = read_word();
    return words;
}
static void print_text(const char *text, const char *end) {
    if (fputs(text, stdout) == EOF || fputs(end, stdout) == EOF)
        io_require(IO_STREAM_ERROR);
}
static void print_string(const String *s, const char *end) { print_text(string_cstr(s), end); }
static void print_int(int value, const char *end) {
    if (printf("%d%s", value, end) < 0) io_require(IO_STREAM_ERROR);
}
static void print_ll(long long value, const char *end) {
    if (printf("%lld%s", value, end) < 0) io_require(IO_STREAM_ERROR);
}
static void print_double(double value, int precision, const char *end) {
    if (precision < 0) tb_fail("precision must be nonnegative");
    if (printf("%.*f%s", precision, value, end) < 0) io_require(IO_STREAM_ERROR);
}
static void print_ints(const IntVector *v, const char *sep, const char *end) {
    for (size_t i = 0; i < v->len; ++i) {
        if (i) print_text(sep, "");
        print_int(v->data[i], "");
    }
    print_text("", end);
}
static void print_lls(const LongLongVector *v, const char *sep, const char *end) {
    for (size_t i = 0; i < v->len; ++i) {
        if (i) print_text(sep, "");
        print_ll(v->data[i], "");
    }
    print_text("", end);
}
static void print_doubles(const DoubleVector *v, int precision, const char *sep, const char *end) {
    if (precision < 0) tb_fail("precision must be nonnegative");
    for (size_t i = 0; i < v->len; ++i) {
        if (i) print_text(sep, "");
        print_double(v->data[i], precision, "");
    }
    print_text("", end);
}
static void print_strings(const StringList *words, const char *sep, const char *end) {
    for (size_t i = 0; i < words->len; ++i) {
        if (i) print_text(sep, "");
        print_string(&words->data[i], "");
    }
    print_text("", end);
}
