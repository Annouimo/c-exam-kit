/* SPDX-License-Identifier: MIT-0 */
/* Build with string + counter. Reads one line; splits on literal spaces. */
int main(void) {
    String line = {0};
    Counter counts = counter_new();
    if (string_readline(&line, stdin)) {
        StringList words = string_split(&line, " ");
        for (size_t i = 0; i < words.len; ++i) {
            if (words.data[i].len &&
                !counter_add(&counts, string_cstr(&words.data[i]), 1))
                tb_fail("counter overflow");
        }
        size_t cursor = 0;
        const char *word;
        long long count;
        while (counter_next(&counts, &cursor, &word, &count))
            printf("%s %lld\n", word, count);
        string_list_free(&words);
    }
    string_free(&line);
    counter_free(&counts);
    return 0;
}
