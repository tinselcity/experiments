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
        printf("%d", i_v); \
        if (i_v != _v.back()) { printf(", "); }\
     } printf("]"); \
} while(0)
//! ----------------------------------------------------------------------------
//! \class Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int maxProfit(std::vector<int>& prices) {
        int l_max_profit = 0;
        // some max value > 10^4
        int l_min_price = 1000000;
        for (size_t i_i = 0; i_i < prices.size(); ++i_i) {
            if (prices[i_i] < l_min_price)
            {
                l_min_price = prices[i_i];
            }
            int l_profit = prices[i_i] - l_min_price;
            if (l_profit > l_max_profit)
            {
                l_max_profit = l_profit;
            }
        }
        return l_max_profit;
    }
};
//! ----------------------------------------------------------------------------
//! test
//! ----------------------------------------------------------------------------
static void _test(std::vector<int> a_vec)
{
    int l_profit = Solution::maxProfit(a_vec);
    // print vector
    _PRINT_VEC(a_vec);
    printf("\n");
    printf("PROFIT: %d\n", l_profit);
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    _test({7,1,5,3,6,4});
    return 0;
}