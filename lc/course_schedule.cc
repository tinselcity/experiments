#include <stdio.h>
#include <vector>
#include <map>
#include <set>
class Solution {
public:
    typedef std::vector<std::vector<int>> vv_int_t;
    typedef std::map<int, std::vector<int>> map_list_t;
    typedef std::set<int> set_t;
    static bool _dfs(int a_course, map_list_t& a_adj, set_t& a_visiting) {
        if (a_visiting.find(a_course) != a_visiting.end()) {
            return false;
        }
        auto i_adj = a_adj.find(a_course);
        if (i_adj == a_adj.end() ||
            !i_adj->second.size())
        {
            return true;
        }
        // add to visiting
        a_visiting.insert(a_course);
        for (auto && i_e : i_adj->second) {
            if (!_dfs(i_e, a_adj, a_visiting)) {
                return false;
            }
        }
        // remove from vistiting
        a_visiting.erase(a_course);
        i_adj->second.clear();
        return true;
    }
    static bool canFinish(int numCourses,
                          std::vector<std::vector<int>>& prerequisites) {
        map_list_t l_adj;
        for (auto && i_p : prerequisites) {
            l_adj[i_p[0]].push_back(i_p[1]);
        }
        set_t l_visiting;
        for (int i_c = 0; i_c < numCourses; ++i_c) {
            if (!_dfs(i_c, l_adj, l_visiting)) {
                return false;
            }
        }
        return true;
    }
};
int main(void) {
    {
    Solution::vv_int_t l_pr = {{1, 0}, {0,1}};
    int l_nc = 2;
    bool l_f = Solution::canFinish(l_nc, l_pr);
    printf("in num courses[%d] can take: %d\n", l_nc, l_f);
    }
    return 0;
}