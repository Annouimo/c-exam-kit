#define FREQ_MAX 200000
int frequency[FREQ_MAX+1];
void build_frequency(int a[], int n) {
    memset(frequency, 0, sizeof frequency);
    for (int i = 0; i < n; i++) frequency[a[i]]++;
}
