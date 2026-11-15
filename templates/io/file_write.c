int main(void) {
    FILE *output = fopen("output.txt", "w");  @COMMENT:file_write_0@
    if (output == NULL) return 1;
@BODY@    @COMMENT:file_write_1@
    if (fclose(output) != 0) return 1;
    return 0;
}
