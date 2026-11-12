typedef struct { int value, original; } RankItem;
int cmp_rank_item(const void *p, const void *q) {
    RankItem a=*(const RankItem *)p,b=*(const RankItem *)q;
    if(a.value!=b.value) return a.value<b.value?-1:1;
    return (a.original>b.original)-(a.original<b.original);
}
void make_rank(RankItem a[], int n, int rank[]) {
    qsort(a,n,sizeof a[0],cmp_rank_item);
    for(int i=0;i<n;i++) rank[a[i].original]=i;
}
int unique_sorted(long long a[], int n) {
    int size=0;
    for(int i=0;i<n;i++) if(i==0 || a[i]!=a[i-1]) a[size++]=a[i];
    return size;
}
