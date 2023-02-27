#include <stdio.h>
#include <vector>
#include <unordered_set>
//! ----------------------------------------------------------------------------
//! Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static bool containsDuplicate(std::vector<int>& nums) {
        if (nums.empty()) {
            return false;
        }
        std::unordered_set<int> l_has_v;
        for (auto && i_n : nums)
        {
            auto i_has = l_has_v.find(i_n);
            if (i_has != l_has_v.end())
            {
                return true;
            }
            l_has_v.insert(i_n);
        }
        return false;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    {
    bool l_b;
    std::vector l_v = {0, 1};
    l_b = Solution::containsDuplicate(l_v);
    printf("contains duplicate: %s\n", l_b ? "true": "false");
    }
    {
    bool l_b;
    std::vector l_v = {1, 2, 3, 1};
    l_b = Solution::containsDuplicate(l_v);
    printf("contains duplicate: %s\n", l_b ? "true": "false");
    }

    return 0;
}