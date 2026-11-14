int print_fixed(double x, int digits) {
    if (digits < 0 || digits > 15) return 0;
    int n = snprintf(NULL, 0, "%.*f", digits, x);
    if (n < 0) return 0;
    char *text = malloc((size_t)n + 1);
    if (!text) return 0;
    if (snprintf(text, (size_t)n + 1, "%.*f", digits, x) != n) {
        free(text);
        return 0;
    }
    int skip = text[0] == '-' && strtod(text, NULL) == 0.0;
    int ok = fputs(text + skip, stdout) != EOF;
    free(text);
    return ok;
}
