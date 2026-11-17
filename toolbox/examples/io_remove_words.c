/* SPDX-License-Identifier: MIT-0 */
int main(void) {
    String text = read_line_raw();
    int n = read_int();
    if (n < 0) tb_fail("word count must be nonnegative");
    for (int i = 0; i < n; ++i) {
        String word = read_word();
        (void)string_remove_repeated_s(&text, &word);
        string_free(&word);
    }
    print_string(&text, "\n");
    string_free(&text);
    return 0;
}
