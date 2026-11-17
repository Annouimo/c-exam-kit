/* SPDX-License-Identifier: MIT-0 */
/* Word frequencies in lexicographic order, using toolbox objects throughout. */
int main(void) {
    String text = read_line();
    StringList words = string_split_whitespace(&text);
    Counter counts = counter_from_strings(&words);
    StringList keys = counter_keys(&counts);
    string_list_sort(&keys);
    for (size_t i = 0; i < string_list_size(&keys); ++i) {
        const String *word = string_list_get(&keys, i);
        print_string(word, ": ");
        print_ll(counter_get_s(&counts, word), "\n");
    }
    string_list_free(&keys);
    counter_free(&counts);
    string_list_free(&words);
    string_free(&text);
    return 0;
}
