/* SPDX-License-Identifier: MIT-0 */
#include <stdarg.h>
static String tb_string_vformat(const char *format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (needed < 0) tb_fail("formatting failed");
    String result = string_new();
    string_reserve(&result, (size_t)needed);
    va_copy(copy, args);
    int written = vsnprintf(result.data, result.cap, format, copy);
    va_end(copy);
    if (written != needed) { string_free(&result); tb_fail("inconsistent formatted length"); }
    if (memchr(result.data, 0, (size_t)needed)) {
        string_free(&result); tb_fail("NUL in formatted text");
    }
    result.len = (size_t)needed;
    return result;
}
static String string_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    String result = tb_string_vformat(format, args);
    va_end(args);
    return result;
}
static void string_append_format(String *s, const char *format, ...) {
    va_list args;
    va_start(args, format);
    String part = tb_string_vformat(format, args);
    va_end(args);
    string_append(s, string_cstr(&part));
    string_free(&part);
}
static String string_from_int(int value) { return string_format("%d", value); }
static String string_from_ll(long long value) { return string_format("%lld", value); }
static String string_from_double(double value, int precision) {
    if (precision < 0) tb_fail("precision must be nonnegative");
    return string_format("%.*f", precision, value);
}
