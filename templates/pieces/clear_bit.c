unsigned long long clear_bit(unsigned long long x, int pos) { return (pos >= 0 && pos < 64) ? (x & ~(1ULL << pos)) : x; }
