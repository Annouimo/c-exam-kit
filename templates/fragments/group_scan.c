/* TODO: a[0..n-1]须已排序；在循环中使用段长j-i */
for (int i = 0; i < n; ) {
    int j = i+1;
    while (j < n && a[j] == a[i]) j++;
    /* TODO: 值a[i]出现j-i次 */
    i = j;
}
