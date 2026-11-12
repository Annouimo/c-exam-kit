int longest_char_run(const char s[], char target) {
    int length = 0, best = 0;
    for (int i = 0; s[i]; i++) {
        length = s[i] == target ? length + 1 : 0;
        if (length > best) best = length;
    }
    return best;
}
