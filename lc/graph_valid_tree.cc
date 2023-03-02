#include <stdio.h>
#include <vector>
typedef std::vector<std::vector<int>> vv_int_t;
class Solution {
public:
    static bool validTree(int nodes,
                          std::vector<std::vector<int>>& edge_list) {
        return false;
    }
};
int main(void) {
    {
    vv_int_t l_el = {{0, 1}, {0, 2}, {0, 3}, {1, 4}};
    int l_n = 2;
    bool l_f = Solution::validTree(l_n, l_el);
    printf("valid tree: %d\n", l_f);
    }
    return 0;
}