#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto&& i_n : _v) { \
        printf("%d", i_n); \
        if (&i_n != &_v.back()) { printf(",");} \
    } \
    printf("]"); \
} while(0)
class Solution {
public:
    typedef std::vector<std::vector<int>> vv_int_t;
    typedef std::map<int, std::vector<int>> map_list_t;
    typedef std::set<int> set_t;
    static bool _dfs(int a_course, 
                     set_t& a_visit,
                     set_t& a_cycle,
                     map_list_t& a_adj,
                     std::vector<int>& ao_result) {
        if (a_cycle.find(a_course) != a_cycle.end()) {
            return false;
        }
        if (a_visit.find(a_course) != a_visit.end()) {
            return true;
        }
        a_cycle.insert(a_course);
        auto i_prl = a_adj[a_course];
        for (auto && i_p : i_prl) {
            if (!_dfs(i_p, a_visit, a_cycle, a_adj, ao_result)) {
                return false;
            }
        }
        a_cycle.erase(a_course);
        a_visit.insert(a_course);
        ao_result.push_back(a_course);
        return true;
    }
    static std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        std::vector<int> l_res;
        std::vector<int> l_empty;
        // -------------------------------------------------
        // build adjacency list
        // -------------------------------------------------
        map_list_t l_adj;
        for (auto && i_p : prerequisites) {
            l_adj[i_p[0]].push_back(i_p[1]);
        }
        // -------------------------------------------------
        // dfs
        // -------------------------------------------------
        set_t l_visit;
        set_t l_cycle;
        for (int i_cr = 0; i_cr < numCourses; ++i_cr) {
            if (!_dfs(i_cr, l_visit, l_cycle, l_adj, l_res)) {
                return l_empty;
            }
        }
        return l_res;
    }
};

int main(void) {
    {
    Solution::vv_int_t l_pr = {{1, 0}};
    int l_nc = 2;
    std::vector<int> l_fo = Solution::findOrder(l_nc, l_pr);
    printf("in num courses[%d] ", l_nc);
    _PRINT_VEC(l_fo);
    printf("\n");
    }
    {
    Solution::vv_int_t l_pr = {{1,0},{2,0},{3,1},{3,2}};
    int l_nc = 4;
    std::vector<int> l_fo = Solution::findOrder(l_nc, l_pr);
    printf("in num courses[%d] ", l_nc);
    _PRINT_VEC(l_fo);
    printf("\n");
    }
    {
    Solution::vv_int_t l_pr = {{5,0},{4,0},{0,1},{0,2},{1,3},{3,2}};
    int l_nc = 6;
    std::vector<int> l_fo = Solution::findOrder(l_nc, l_pr);
    printf("in num courses[%d] ", l_nc);
    _PRINT_VEC(l_fo);
    printf("\n");
    }
    return 0;
}