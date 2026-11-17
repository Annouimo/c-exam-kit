/* SPDX-License-Identifier: MIT-0 */
/* A count on its own line, followed by exactly that many physical lines.
   Empty lines and leading/trailing spaces are part of the data. */
int main(void) {
    int n = read_int();
    if (n < 0) tb_fail("line count must be nonnegative");
    finish_line();
    for (int i = 0; i < n; ++i) {
        String line = read_line_raw();
        print_text("[", "");
        print_string(&line, "]\n");
        string_free(&line);
    }
    return 0;
}
