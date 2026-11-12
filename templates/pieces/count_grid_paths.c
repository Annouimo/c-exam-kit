#define GRID_PATH_CAP 105
int blocked[GRID_PATH_CAP][GRID_PATH_CAP];
long long path_dp[GRID_PATH_CAP][GRID_PATH_CAP];
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
