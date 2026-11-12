#define WORD_CAP 105
typedef struct { char text[WORD_CAP]; int original; } Word;
int cmp_word(const void *p, const void *q) {
    const Word *a=(const Word *)p,*b=(const Word *)q;
    size_t x=strlen(a->text),y=strlen(b->text);
    if(x!=y) return x<y?-1:1;
    int cmp=strcmp(a->text,b->text);
    if(cmp) return cmp;
    return (a->original>b->original)-(a->original<b->original);
}
