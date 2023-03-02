#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <queue>
#include <set>
#include <list>
typedef std::vector<std::vector<char>> vvc_t;

class Solution {
public:
    static int numIslands(std::vector<std::vector<char>>& grid) {
        if (grid.empty() ||
            grid[0].empty())
        {
            return 0;
        }
        int l_ret = 0;
        size_t l_rows = grid.size();
        size_t l_cols = grid[0].size();
        typedef std::pair<int, int> pair_t;
        std::list <pair_t> l_dirs;
        l_dirs.push_back(pair_t( 1,  0));
        l_dirs.push_back(pair_t(-1,  0));
        l_dirs.push_back(pair_t( 0,  1));
        l_dirs.push_back(pair_t( 0, -1));
        std::set<pair_t> l_visited;
        for (int i_r = 0; i_r < (int)l_rows; ++i_r) {
            for (int i_c = 0; i_c < (int)l_cols; ++i_c) {
                pair_t l_p = pair_t(i_r, i_c);
                if (l_visited.find(l_p) != l_visited.end()) {
                    continue;
                }
                if (grid[i_r][i_c] == '1') {
                    l_visited.insert(l_p);
                    std::queue <pair_t>l_q;
                    l_q.push(l_p);
                    while (!l_q.empty()) {
                        pair_t i_p = l_q.front();
                        l_q.pop();
                        for (auto && i_d : l_dirs) {
                            int l_r = i_p.first + i_d.first;
                            int l_c = i_p.second + i_d.second;
                            pair_t l_v(l_r, l_c);
                            if (l_r >= 0 && l_r < l_rows &&
                                l_c >= 0 && l_c < l_cols &&
                                grid[l_r][l_c] == '1' &&
                                l_visited.find(l_v) == l_visited.end()) {
                                // add to visited
                                l_visited.insert(l_v);
                                l_q.push(l_v);
                            }
                        }
                    }
                    l_ret += 1;
                }
            }
        }
        return l_ret;
    }
};
int main(void) {
    {
        int l_s = 0;
        vvc_t l_m = {
            {'1','1','1','1','0'},
            {'1','1','0','1','0'},
            {'1','1','0','0','0'},
            {'0','0','0','0','0'}
        };
        l_s = Solution::numIslands(l_m);
        printf("num islands: %d\n", l_s);
    }
    {
        int l_s = 0;
        vvc_t l_m = {
          {'1','1','0','0','0'},
          {'1','1','0','0','0'},
          {'0','0','1','0','0'},
          {'0','0','0','1','1'}
        };
        l_s = Solution::numIslands(l_m);
        printf("num islands: %d\n", l_s);
    }
    return 0;
}