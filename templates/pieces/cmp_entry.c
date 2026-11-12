int cmp_entry(const void *a, const void *b) {
    Entry x=*(const Entry *)a,y=*(const Entry *)b;
    if(x.value!=y.value) return x.value<y.value?-1:1;
    return (x.index>y.index)-(x.index<y.index);
}
