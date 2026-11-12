int count_ones(unsigned long long x) {
    int count = 0;
    while (x) {
        x &= x - 1;                /* 每次消掉最低位的一个 1 */
        count++;
    }
    return count;
}
