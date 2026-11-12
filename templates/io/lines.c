int main(void) {
    char line[LINE_CAP];
    while (fgets(line, sizeof line, stdin) != NULL) {
        /* 保留空行和行首空白；line可能包含末尾换行；整行须放得下 */
@LOOP@    }
    return 0;
}
