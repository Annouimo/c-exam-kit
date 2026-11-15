@COMMENT:output_items_0@
int first_output = 1;
const char *item_separator = SEPARATOR_LITERAL;
for (int item = 0; item < 4; item++) {
    if (!first_output) fputs(item_separator, stdout);
    first_output = 0;
    printf("%d", 1);  @COMMENT:output_items_1@
}
putchar('\n');
