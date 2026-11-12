/* TODO: a和n为要输出的数组与长度；放在计算完成之后 */
const char *array_separator = SEPARATOR_LITERAL;
for (int i = 0; i < n; i++) {
    if (i > 0) fputs(array_separator, stdout);
    printf(VALUE_PRINT, a[i]);
}
putchar('\n');
