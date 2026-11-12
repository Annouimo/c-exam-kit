int main(void) {
    FILE *input = fopen("input.txt", "r");  /* TODO: 修改文件名 */
    if (input == NULL) return 1;
    /* TODO: 声明字段并替换格式、地址、成功字段数 */
    while (fscanf(input, "TODO", /* TODO: 地址 */) == /* TODO: 字段数 */) {
@LOOP@    }
    fclose(input);
    return 0;
}
