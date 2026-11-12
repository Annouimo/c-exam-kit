#define PATH_MAX 105
int blocked[PATH_MAX][PATH_MAX];
long long path_dp[PATH_MAX][PATH_MAX];
long long count_grid_paths(int rows, int cols) {
    memset(path_dp,0,sizeof path_dp);
    for(int r=0;r<rows;r++) for(int c=0;c<cols;c++) {
        if(blocked[r][c]) continue;
        if(r==0 && c==0) path_dp[r][c]=1;
        else {
            if(r) path_dp[r][c]+=path_dp[r-1][c];
            if(c) path_dp[r][c]+=path_dp[r][c-1];
        }
    }
    return path_dp[rows-1][cols-1];
}
