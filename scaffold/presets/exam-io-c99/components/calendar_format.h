/** @requires exam_failf */
static inline void exam_calendar_print(const char *api, const char *format,
                                        const char *end, const char *fields,
                                        const int values[3]) {
    if (!format || !end)
        exam_failf("algorithm", api, "%s must not be NULL", format ? "end" : "format");

    for (size_t i = 0; format[i]; ++i) {
        unsigned char c = (unsigned char)format[i];
        if (c != '%') {
            if (putchar(c) == EOF)
                exam_failf("algorithm", api, "output stream error");
            continue;
        }

        size_t start = i;
        c = (unsigned char)format[++i];
        if (c == '%') {
            if (putchar('%') == EOF)
                exam_failf("algorithm", api, "output stream error");
            continue;
        }
        bool unpadded = c == '-';
        if (unpadded) c = (unsigned char)format[++i];
        if (!c)
            exam_failf("algorithm", api, "incomplete format specifier at byte %zu", start);
        const char *field = strchr(fields, c);
        if (!field)
            exam_failf("algorithm", api, "unsupported format specifier at byte %zu (code=0x%02X)",
                       start, (unsigned)c);
        int width = unpadded ? 0 : (c == 'Y' ? 4 : 2);
        if (printf("%0*d", width, values[field - fields]) < 0)
            exam_failf("algorithm", api, "output stream error");
    }
    if (fputs(end, stdout) == EOF)
        exam_failf("algorithm", api, "output stream error");
}
