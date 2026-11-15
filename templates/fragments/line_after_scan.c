@COMMENT:line_after_scan_0@
int rest;
while ((rest = getchar()) != '\n' && rest != EOF) {}
char line[LINE_CAP];
if (fgets(line, sizeof line, stdin) != NULL) {
    @COMMENT:line_after_scan_1@
}
