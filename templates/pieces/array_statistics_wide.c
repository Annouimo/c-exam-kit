double array_mean(const long long a[], int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) sum += (double)a[i];
    return sum / n;
}
double array_variance(const long long a[], int n, double mean) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = (double)a[i] - mean;
        sum += diff * diff;
    }
    return sum / n;
}
