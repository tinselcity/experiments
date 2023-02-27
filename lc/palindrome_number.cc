//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
//! ----------------------------------------------------------------------------
//! solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int getNumDigits(int x) {
        int nd = 1;
        while(x = x/10) {
            ++nd;
        }
        return nd;
    }
    static int getNthDigit(int x, int n) {
        int p = pow(10, n);
        int h = (x / p) % 10;
        return h;
    }
    static bool isPalindrome(int x) {
        if (x < 0) {
            return false;
        }
        // get number of digits
        int nd = getNumDigits(x);
        // degenerate case of n == 1 is palindrome
        if (nd <= 1) {
            return true;
        }
        // if odd
        int half = nd / 2;
        int left = half;
        int right = half+1;
        if (nd % 2) {
            ++right;
        }
        // loop until done
        while (half)
        {
            if (getNthDigit(x, nd - left) != getNthDigit(x, nd - right)) {
                return false;
            }
            --left;
            ++right;
            --half;
        }
        return true;
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
#define _IS_P(_v) do { \
    printf("%12d is: %s\n", _v, Solution::isPalindrome(_v) ? "true": "false"); \
} while(0)
    _IS_P(0);
    _IS_P(5678);
    _IS_P(101);
    _IS_P(-100);
    _IS_P(5678765);
    _IS_P(56788765);
    _IS_P(56788865);
    return 0;
}
