/* 仅当题意要求下一整行：丢弃当前行剩余内容，保留下一行行首空白 */
int rest;
while ((rest = getchar()) != '\n' && rest != EOF) {}
char line[LINE_CAP];
if (fgets(line, sizeof line, stdin) != NULL) {
    /* TODO: 处理line；如需去换行，选字符串基础组件 */
}
