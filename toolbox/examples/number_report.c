/* SPDX-License-Identifier: MIT-0 */
int main(void) {
    int n = read_int();
    if (n <= 0) tb_fail("need at least one number");
    IntVector values = read_ints((size_t)n);
    String summary = string_format("count=%zu mean=%.2f median=%.2f",
        IntVector_size(&values), IntVector_mean(&values), IntVector_median(&values));
    print_string(&summary, "\n");
    IntVector_sort_asc(&values);
    IntVector_unique_sorted(&values);
    print_ints(&values, " ", "\n");
    string_free(&summary);
    IntVector_free(&values);
    return 0;
}
