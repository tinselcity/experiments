//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <list>
//! ----------------------------------------------------------------------------
//! solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static std::vector<int> twoSum(std::vector<int>& nums, int target) {
        std::vector<int> l_res;
        // -------------------------------------------------
        // make list from num <= target
        // -------------------------------------------------
        typedef std::pair<int, int> ii_pair_t;
        std::list<ii_pair_t> l_list;
        int l_idx = 0;
        for (auto&& i_n: nums) {
            printf("push_back: %d, %d\n", l_idx, i_n);
            l_list.push_back(ii_pair_t(l_idx, i_n));
            ++l_idx;
        }
        // -------------------------------------------------
        // walk list popping each entry
        // -------------------------------------------------
        while (!l_list.empty()) {
            ii_pair_t l_first = l_list.front();
            l_list.pop_front();
            printf("l_first: %d\n", l_first.second);
            for (auto&& i_p: l_list) {
                if (l_first.second + i_p.second == target)
                {
                    l_res.push_back(l_first.first);
                    l_res.push_back(i_p.first);
                    return l_res;
                }
            }
        }
        return l_res;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    std::vector<int> l_input = {-3,4,3,90};
    int l_target = 7;
    std::vector<int> l_result;
    l_result = Solution::twoSum(l_input, l_target);
    printf("Result is: [");
    for (auto&& i_r : l_result) {
        printf("%d, ", i_r);
    }
    printf("]\n");
    return 0;
}
