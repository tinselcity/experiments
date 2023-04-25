#include <stdio.h>
#include <vector>

using namespace std;
class Solution {
public:
  static int dfs(vector<vector<int>>& grid,
                 int r,
                 int c)
  {
    // check if:
    //  -out of bounds
    //  -not water
    if (r < 0 ||
        r >= grid.size() ||
        c < 0 ||
        c >= grid[0].size() ||
        grid[r][c] == 0) {
        return 0;
    }
    // mark as visited
    grid[r][c] = 0;
    return 1 +
           dfs(grid, r+1, c) +
           dfs(grid, r-1, c) +
           dfs(grid, r,   c+1) +
           dfs(grid, r,   c-1);
  }
  static int maxAreaOfIsland(vector<vector<int>>& grid) {
    if (!grid.size()) {
        return 0;
    }
    if (!grid[0].size()) {
        return 0;
    }
    int max = 0;
    for (int r = 0; r < grid.size(); ++r) {
        for (int c = 0; c < grid[0].size(); ++c) {
            int s = dfs(grid, r, c);
            if (s > max) {
                max = s;
            }
        }
    }
    return max;
  }
};

int main(void) {
  {
    vector<vector<int>> map = {
      {0,0,1,0,0,0,0,1,0,0,0,0,0},
      {0,0,0,0,0,0,0,1,1,1,0,0,0},
      {0,1,1,0,1,0,0,0,0,0,0,0,0},
      {0,1,0,0,1,1,0,0,1,0,1,0,0},
      {0,1,0,0,1,1,0,0,1,1,1,0,0},
      {0,0,0,0,0,0,0,0,0,0,1,0,0},
      {0,0,0,0,0,0,0,1,1,1,0,0,0},
      {0,0,0,0,0,0,0,1,1,0,0,0,0}
    };
    printf("solution: %d\n", Solution::maxAreaOfIsland(map));
  }
  return 0;
}