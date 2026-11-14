int str_insert_char(char s[], size_t capacity, size_t pos, char ch) {
    size_t n = strlen(s);
    if (n >= capacity || pos > n || ch == '\0' || n + 1 >= capacity) return 0;
    memmove(s + pos + 1, s + pos, n - pos + 1);
    s[pos] = ch;
    return 1;
}
int str_erase(char s[], size_t pos, size_t count) {
    size_t n = strlen(s);
    if (pos > n || count > n - pos) return 0;
    memmove(s + pos, s + pos + count, n - pos - count + 1);
    return 1;
}
int str_run_bounds(const char s[], int n, int pos, int *left, int *right) {
    if (pos < 0 || pos >= n) return 0;
    int l = pos, r = pos + 1;
    while (l > 0 && s[l - 1] == s[pos]) l--;
    while (r < n && s[r] == s[pos]) r++;
    *left = l;
    *right = r;
    return 1;
}
