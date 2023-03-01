#include <stdio.h>
#include <vector>
#include <unordered_map>
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto& i_v : _v) { \
        printf("%d", i_v); \
        if (i_v != _v.back()) { printf(", "); }\
     } printf("]"); \
} while(0)
//! ----------------------------------------------------------------------------
//! Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int _combinationSum4(std::unordered_map<int, int>& a_memo,
                                std::vector<int>& a_nums,
                                int a_target) {
        // out of bounds
        if (a_target < 0) {
            return 0;
        }
        // found a combo
        if (a_target == 0) {
            return 1;
        }
        auto i_e = a_memo.find(a_target);
        if (i_e != a_memo.end()) {
            return i_e->second;
        }
        int l_val = 0;
        for (auto && i_v : a_nums) {
            l_val += _combinationSum4(a_memo, a_nums, a_target-i_v);
        }
        // store
        a_memo[a_target] = l_val;
        return l_val;
    }
    static int combinationSum4(std::vector<int>& nums, int target) {
        std::unordered_map<int, int> l_memo;
        return _combinationSum4(l_memo, nums, target);
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    {
    std::vector l_v = {1,2,3};
    int l_target = 4;
    int l_res = Solution::combinationSum4(l_v, l_target);
    printf("combination sum: ");
    _PRINT_VEC(l_v);
    printf(" target[%d]: %d\n", l_target, l_res);
    }
    {
    std::vector l_v = {9};
    int l_target = 3;
    int l_res = Solution::combinationSum4(l_v, l_target);
    printf("list ");
    _PRINT_VEC(l_v);
    printf(" target[%d]: %d\n", l_target, l_res);
    }
    return 0;
}