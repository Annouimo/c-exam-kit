@COMMENT:group_scan_0@
for (int i = 0; i < n; ) {
    int j = i+1;
    while (j < n && a[j] == a[i]) j++;
    @COMMENT:group_scan_1@
    i = j;
}
