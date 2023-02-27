//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <list>
//! ----------------------------------------------------------------------------
//! solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int climbStairs(int n) {
        if (n <= 0)
        {
            return 0;
        }
        else if (n == 1)
        {
            return 1;
        }
        else if (n == 2)
        {
            return 2;
        }
        int l_n1 = 2;
        int l_n2 = 1;
        int l_result = 0;
        for (int i=2; i<n; ++i)
        {
            l_result = l_n1 + l_n2;
            l_n2 = l_n1;
            l_n1 = l_result;
        }
        return l_result;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
#define _CLIMB_STAIRS(_v) do { \
    printf("stairs(%12d): %12d\n", _v, Solution::climbStairs(_v)); \
} while(0)
    _CLIMB_STAIRS(2);
    _CLIMB_STAIRS(3);
    _CLIMB_STAIRS(4);
    _CLIMB_STAIRS(5);
    _CLIMB_STAIRS(10);
    return 0;
}
