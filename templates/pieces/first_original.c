int first_original(Entry a[],int n,int key) {
    int l=0,r=n;
    while(l<r){int m=l+(r-l)/2; if(a[m].value<key)l=m+1;else r=m;}
    return l<n && a[l].value==key ? a[l].index : -1;
}
