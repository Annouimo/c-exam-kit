int main(void) {
    int ch;  /* getchar返回int，不能用char保存EOF */
    while ((ch = getchar()) != EOF) {
@LOOP@    }
    return 0;
}
