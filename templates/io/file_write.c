int main(void) {
    FILE *output = fopen("output.txt", "w");  /* TODO: 修改文件名；w覆盖已有文件 */
    if (output == NULL) return 1;
@BODY@    /* TODO: 按题意用fprintf(output, ...)写入 */
    if (fclose(output) != 0) return 1;
    return 0;
}
