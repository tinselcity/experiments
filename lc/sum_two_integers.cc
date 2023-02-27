//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
//! ----------------------------------------------------------------------------
//! \class: Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int _get_sum(unsigned int a_a, unsigned int a_b) {
        //printf("%s.%s:%d: a: %4u, b: %4u\n", __FILE__,__FUNCTION__,__LINE__, a_a, a_b);
        if (a_b <= 0) {
            return a_a;
        }
        int l_xor = a_a^a_b;
        int l_carry = (a_a & a_b) << 1;
        return _get_sum(l_xor, l_carry);
    }
    static int _get_diff(unsigned int a_a, unsigned int a_b) {
        //printf("%s.%s:%d: a: %4u, b: %4u\n", __FILE__,__FUNCTION__,__LINE__, a_a, a_b);
        if (a_b <= 0) {
            return a_a;
        }
        int l_xor = a_a^a_b;
        int l_borrow = (~a_a & a_b) << 1;
        return _get_diff(l_xor, l_borrow);
    }
    static int getSum(int a, int b) {
        if (a == 0) {
            return b;
        }
        if (b == 0) {
            return a;
        }
        int l_a_abs = abs(a);
        int l_b_abs = abs(b);
        if (l_a_abs < l_b_abs)
        {
            return getSum(b, a);
        }
        // -------------------------------------------------
        // addition
        // -------------------------------------------------
        if (a*b > 0) {
            int l_neg = 1;
            if (a < 0) {
                l_neg = -1;
            }
            return l_neg * _get_sum(l_a_abs, l_b_abs);
        }
        else {
            int l_neg = 1;
            if (a < 0) {
                l_neg = -1;
            }
            return l_neg * _get_diff(l_a_abs, l_b_abs);
        }
        return 0;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
#define _TEST(_a, _b) do {\
    int l_ts = Solution::getSum(_a, _b); \
    int l_cs = _a + _b; \
    printf("%4d + %4d = %4d (CHECK): %4d :: %s\n", \
        _a, _b, l_ts, l_cs, l_ts == l_cs ? "PASS": "FAIL"); \
} while(0);
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    _TEST(15, 2);
    _TEST(15, 0);
    _TEST(0, 0);
    _TEST(0, 2);
    _TEST(0, -2);
    _TEST(-15, -2);
    _TEST(-15, 2);
    _TEST(-2, 15);
    return 0;
}
