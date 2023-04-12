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

#define _PRINT_VEC_VEC(_vv) do { \
    printf("["); \
    for (auto&& i_v : _vv) { \
        printf("["); \
        for (auto&& i_n : i_v) { \
            printf("%d", i_n); \
            if (&i_n != &i_v.back()) { printf(",");} \
        } \
        printf("]"); \
        if (&i_v != &_vv.back()) { printf(",");} \
    } \
    printf("]"); \
} while(0)

class Solution {
public:
    typedef std::vector<std::vector<int>> vv_int_t; 
    static std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
        vv_int_t l_ret;
        return l_ret;
    }
};

int main(void) {
    {
        std::vector<int> l_v = {-1,0,1,2,-1,-4};
        auto l_r = Solution::threeSum(l_v);
        _PRINT_VEC(l_v); printf("\n");
        _PRINT_VEC_VEC(l_r); printf("\n");
    }

}