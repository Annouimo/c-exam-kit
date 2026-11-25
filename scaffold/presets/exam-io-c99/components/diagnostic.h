static inline void exam_failf(const char *group, const char *api,
                              const char *format, ...) {
    va_list args;
    fprintf(stderr, "exam %s: %s: ", group, api);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
