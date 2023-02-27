//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
using namespace std;
//! ----------------------------------------------------------------------------
//! solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int maxSubArray(vector<int>& nums) {
        int l_cur = 0;
        int l_result = -1000000;
        for(auto && i_n : nums) {
            l_cur = std::max(i_n, i_n + l_cur);
            l_result = std::max(l_result, l_cur);
            printf("i_n:      %d\n", i_n);
            printf("l_cur:    %d\n", l_cur);
            printf("l_result: %d\n", l_result);
        }
        return l_result;
#if 0
        int l_result = nums[0];
        int l_cur = 0;
        for(auto && i_n : nums) {
            if (l_cur < 0) {
                l_cur = 0;
            }
            l_cur += i_n;
            l_result = std::max(l_result, l_cur);
        }
        return l_result;
#endif
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
    std::vector<int> l_v;
#define _MAX_SUBARRAY(_v) do { \
    printf("max_subarray: %12d\n", Solution::maxSubArray(_v)); \
} while(0)
    l_v = {1,2,-6,1,2,3,4,5};
    _MAX_SUBARRAY(l_v);
    //l_v = {-2,1,-3,4,-1,2,1,-5,4};
    //_MAX_SUBARRAY(l_v);
    //l_v = {1};
    //_MAX_SUBARRAY(l_v);
    //l_v = {5,4,-1,7,8};
    //_MAX_SUBARRAY(l_v);
    return 0;
}
