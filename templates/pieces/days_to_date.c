Date days_to_date(int serial) {
    int l=1,r=9999;
    while(l<r) {
        int m=l+(r-l+1)/2;
        Date first={m,1,1};
        if(date_to_days(first)<=serial) l=m; else r=m-1;
    }
    Date x={l,1,1};
    int remaining=serial-date_to_days(x);
    while(remaining>=days_in_month(x.y,x.m)) { remaining-=days_in_month(x.y,x.m); x.m++; }
    x.d=remaining+1;
    return x;
}
Date add_days(Date x,int delta) { return days_to_date(date_to_days(x)+delta); }
