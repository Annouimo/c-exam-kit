#define MATRIX_MAX 105
void matrix_multiply(long long a[][MATRIX_MAX], long long b[][MATRIX_MAX],
                     long long result[][MATRIX_MAX], int n, int m, int p) {
    for(int i=0;i<n;i++) for(int j=0;j<p;j++) {
        result[i][j]=0;
        for(int k=0;k<m;k++) result[i][j]+=a[i][k]*b[k][j];
    }
}
