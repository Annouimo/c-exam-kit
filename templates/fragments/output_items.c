/* TODO: 修改循环范围和输出内容；若筛选项目，把判断放在first_output之前 */
int first_output = 1;
const char *item_separator = SEPARATOR_LITERAL;
for (int item = 0; item < 4; item++) {
    if (!first_output) fputs(item_separator, stdout);
    first_output = 0;
    printf("%d", 1);  /* TODO: 替换为本项的输出 */
}
putchar('\n');
