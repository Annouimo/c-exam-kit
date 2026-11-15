int main(void) {
    FILE *input = fopen("input.txt", "r");  @COMMENT:file_records_0@
    if (input == NULL) return 1;
    @COMMENT:file_records_1@
    while (fscanf(input, "TODO", @COMMENT:file_records_2@) == @COMMENT:file_records_3@) {
@LOOP@    }
    fclose(input);
    return 0;
}
