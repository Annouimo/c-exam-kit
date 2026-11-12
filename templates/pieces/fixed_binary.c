void fixed_binary(unsigned long long x, int width, char out[]) {
    for(int i=0;i<width;i++) out[i]=(char)('0'+((x>>(width-1-i))&1ULL));
    out[width]='\0';
}
unsigned long long rotate_left64(unsigned long long x,int k) {
    k%=64;
    return k ? (x<<k)|(x>>(64-k)) : x;
}
unsigned long long rotate_right64(unsigned long long x,int k) {
    k%=64;
    return k ? (x>>k)|(x<<(64-k)) : x;
}
