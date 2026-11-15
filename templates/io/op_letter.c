int main(void) {
    @COMMENT:op_letter_0@
@INPUT@    int q;
    scanf("%d", &q);
    while (q--) {
        char op;
        scanf(" %c", &op);
        switch (op) {
            case 'Q':
                @COMMENT:op_letter_1@
                break;
            case 'C':
                @COMMENT:op_letter_2@
                break;
        }
    }
    @COMMENT:op_letter_3@
@OUTPUT@    return 0;
}
