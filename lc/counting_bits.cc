#include <stdio.h>
#include <vector>
//! ----------------------------------------------------------------------------
//! Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static std::vector<int> countBits(int n) {
        std::vector<int> l_ret;
        for (size_t i_n = 0; i_n <= n; ++i_n)
        {
            l_ret.push_back(__builtin_popcount(i_n));
        }
        return l_ret;
    }
};
//! ----------------------------------------------------------------------------
//! test
//! ----------------------------------------------------------------------------
static void _test(int a_n)
{
    std::vector<int> l_v;
    l_v = Solution::countBits(a_n);
    printf("[");
    for (auto && i_n : l_v)
    {
        printf("%d, ", i_n);
    }
    printf("]\n");
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    _test(5);
    return 0;
}