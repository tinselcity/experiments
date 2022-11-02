//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto& i_v : _v) { \
        printf("%d, ", i_v); \
     } printf("]"); \
} while(0)
//! ----------------------------------------------------------------------------
//! \class Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int trap(std::vector<int>& height) {
        if (height.empty()) {
            return 0;
        }
        int l_total = 0;
        size_t l_h_size = height.size();
        // -------------------------------------------------
        // sweep from left
        // -------------------------------------------------
        std::vector<int> l_lmax(l_h_size, 0);
        l_lmax[0] = height[0];
        for (int i_cur = 1; i_cur < l_h_size; ++i_cur) {
            l_lmax[i_cur] = std::max(l_lmax[i_cur-1], height[i_cur]);
        }
        // -------------------------------------------------
        // sweep from right
        // -------------------------------------------------
        std::vector<int> l_rmax(l_h_size, 0);
        l_rmax[l_h_size-1] = height[l_h_size-1];
        for (int i_cur = l_h_size-2; i_cur >= 0; --i_cur) {
            l_rmax[i_cur] = std::max(height[i_cur], l_rmax[i_cur+1]);
        }
        // -------------------------------------------------
        // get min of each position minus height
        // -------------------------------------------------
        for (int i_cur = 1; i_cur < l_h_size - 1; ++i_cur) {
            int l_val = std::min(l_lmax[i_cur], l_rmax[i_cur]) - height[i_cur];
            l_total += l_val;
        }
        return l_total;
    }
};
//! ----------------------------------------------------------------------------
//! test
//! ----------------------------------------------------------------------------
static void _test(std::vector<int> a_vec)
{
    int l_amount = Solution::trap(a_vec);
    // print vector
    _PRINT_VEC(a_vec);
    printf("\n");
    printf("AMOUNT: %d\n", l_amount);
}
//! ----------------------------------------------------------------------------
//! \function main
//! ----------------------------------------------------------------------------
int main(void)
{
    _test({0,1,0,2,1,0,1,3,2,1,2,1});
    _test({1,2,1,0,1,2,3,2,1,0,1,2});
    return 0;
}