int is_power_of_two(unsigned long long x) {
    return x != 0 && (x & (x - 1)) == 0;
}
