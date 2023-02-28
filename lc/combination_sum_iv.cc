#include <stdio.h>
#include <vector>
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
    static int combinationSum4(std::vector<int>& nums, int target) {
        return 0;    
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    {
    std::vector l_v = {1,2,3};
    int l_res = Solution::combinationSum4(l_v, 4);
    printf("combination sum: ");
    _PRINT_VEC(l_v);
    printf(" == %d\n", l_res);
    }
    {
    std::vector l_v = {9};
    int l_res = Solution::combinationSum4(l_v, 3);
    printf("combination sum: ");
    _PRINT_VEC(l_v);
    printf(" == %d\n", l_res);
    }
    return 0;
}