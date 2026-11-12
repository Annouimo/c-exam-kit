int main(void) {
    FILE *input = fopen("input.txt", "r");  /* TODO: 修改文件名 */
    if (input == NULL) return 1;
    char line[LINE_CAP];
    while (fgets(line, sizeof line, input) != NULL) {
@LOOP@    }
    fclose(input);
    return 0;
}
