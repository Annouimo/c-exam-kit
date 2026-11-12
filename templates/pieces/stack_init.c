#define STACK_CAP 10005
typedef struct { char data[STACK_CAP]; int size; } CharStack;
void stack_init(CharStack *s) { s->size=0; }
int stack_empty(CharStack *s) { return s->size==0; }
void stack_push(CharStack *s, char value) { s->data[s->size++]=value; }
char stack_top(CharStack *s) { return s->data[s->size-1]; }
void stack_pop(CharStack *s) { s->size--; }
int brackets_match(char text[]) {
    static CharStack s;
    stack_init(&s);
    for(int i=0; text[i]; i++) {
        char ch=text[i];
        if(ch=='(' || ch=='[' || ch=='{') stack_push(&s,ch);
        else if(ch==')' || ch==']' || ch=='}') {
            if(stack_empty(&s)) return 0;
            char t=stack_top(&s);
            if(!((t=='('&&ch==')')||(t=='['&&ch==']')||(t=='{'&&ch=='}'))) return 0;
            stack_pop(&s);
        }
    }
    return stack_empty(&s);
}
