int lower_bound_int(int a[], int n, int key) {
    int l=0, r=n;
    while(l<r) { int m=l+(r-l)/2; if(a[m]<key) l=m+1; else r=m; }
    return l;
}
int upper_bound_int(int a[], int n, int key) {
    int l=0, r=n;
    while(l<r) { int m=l+(r-l)/2; if(a[m]<=key) l=m+1; else r=m; }
    return l;
}
