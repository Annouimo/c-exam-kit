void trim_newline(char s[]) {
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[--n] = '\0';
    if (n && s[n-1] == '\r') s[n-1] = '\0';
}
int is_palindrome(char s[]) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n/2; i++) if (s[i] != s[n-1-i]) return 0;
    return 1;
}
int erase_first(char s[], char key[]) {
    size_t n = strlen(key);
    if (!n) return 0;
    char *p = strstr(s, key);
    if (!p) return 0;
    memmove(p, p+n, strlen(p+n)+1);
    return 1;
}
int count_substring(char s[], char key[], int overlap) {
    size_t n = strlen(key);
    if (!n) return 0;
    int count = 0;
    char *p = s;
    while ((p = strstr(p, key)) != NULL) { count++; p += overlap ? 1 : n; }
    return count;
}
