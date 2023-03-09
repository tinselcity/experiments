#include <stdio.h>
#include <vector>
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
    static std::vector<int> productExceptSelf(std::vector<int>& nums) {
        std::vector<int> l_ret(nums.size());
        std::vector<int> l_pre(nums.size());
        std::vector<int> l_pst(nums.size());
        size_t l_idx = 0;
        int l_last = 1;
        for (auto && i_v : nums) {
            int i_cur = i_v * l_last;
            l_pre[l_idx] = i_cur;
            l_last = i_cur; 
            ++l_idx;
        }
        l_last = 1;
        l_idx = nums.size() - 1;
        for (std::vector<int>::const_reverse_iterator i_v = nums.rbegin();
             i_v != nums.rend();
             ++i_v) {
            int i_cur = *i_v * l_last;
            l_pst[l_idx] = i_cur;
            l_last = i_cur; 
            --l_idx;
        }
        for (size_t i_idx = 0; i_idx < nums.size(); ++i_idx) {
            // multiply pre * post
            int i_pre = 1;
            int i_post = 1;
            if (i_idx > 0) {
                i_pre = l_pre[i_idx-1];
            }
            if (i_idx < nums.size()-1) {
                i_post = l_pst[i_idx+1];
            }
            l_ret[i_idx] = i_pre*i_post;
        }
        return l_ret;
    }
};
int main(void) {
    {
        std::vector<int> l_v = {1,2,3,4};
        auto l_r = Solution::productExceptSelf(l_v);
        _PRINT_VEC(l_v); printf("\n");
        _PRINT_VEC(l_r); printf("\n");
    }
    {
        std::vector<int> l_v = {-1,1,0,-3,3};
        auto l_r = Solution::productExceptSelf(l_v);
        _PRINT_VEC(l_v); printf("\n");
        _PRINT_VEC(l_r); printf("\n");
    }
    return 0;
}
