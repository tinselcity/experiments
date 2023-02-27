//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <map>
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto&& i_n : a_v) { \
        printf("%d", i_n); \
        if (&i_n != &a_v.back()) { printf(",");} \
    } \
    printf("]\n"); \
} while(0)
#define _PRINT_VEC_R(_v, _min) do { \
    printf("["); \
    int _i = _min; \
    while (_i < _v.size()) { \
        printf("%d", _v[_i]); \
        if ((_i + 1) < _v.size()) { printf(",");} \
        ++_i; \
    } \
    printf("]\n"); \
} while(0)
//! ----------------------------------------------------------------------------
//! \class: Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int _rob(const std::vector<int>& a_v, size_t a_idx, std::map <size_t, int>& a_memo)
    {
        if (a_idx >= a_v.size())
        {
            return 0;
        }
        auto && i_m = a_memo.find(a_idx);
        if (i_m != a_memo.end())
        {
            return a_memo[a_idx];
        }
        int l_lhs = a_v[a_idx] + _rob(a_v, a_idx + 2, a_memo);
        //printf("l_lhs = %d\n", l_lhs);
        int l_rhs = 0;
        l_rhs = _rob(a_v, a_idx + 1, a_memo);
        //printf("l_rhs = %d\n", l_rhs);
        int l_result = std::max(l_lhs, l_rhs);
        a_memo[a_idx] = l_result;
        return l_result;
    }
    static int rob(const std::vector<int>& nums) {
        std::map <size_t, int>l_memo;
        return _rob(nums, 0, l_memo);
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
static void _test(const std::vector<int>& a_v)
{
    //_PRINT_VEC(a_v);
    int l_v = Solution::rob(a_v);
    printf(": Max: %d\n", l_v);
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    //_test({1,2,3,1});
    //_test({2,7,9,3,1});
    _test({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
    return 0;
}