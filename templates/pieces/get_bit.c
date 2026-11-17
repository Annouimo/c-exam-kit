int get_bit(unsigned long long x, int pos) { return (pos >= 0 && pos < 64) ? (int)((x >> pos) & 1ULL) : 0; }
