/* SPDX-License-Identifier: MIT-0 */
/* First line: source text. Then n and n whitespace-delimited words to delete. */
int main(void) {
    String text = string_new(), word = string_new();
    if (!string_readline(&text, stdin)) {
        string_free(&text);
        return 0;
    }
    int n;
    if (scanf("%d", &n) == 1) {
        for (int i = 0; i < n && string_read_token(&word, stdin); ++i)
            (void)string_remove_repeated(&text, string_cstr(&word));
    }
    puts(string_cstr(&text));
    string_free(&word);
    string_free(&text);
    return 0;
}
