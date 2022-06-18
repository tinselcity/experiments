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
    static double new_guess(double guess, double x) {
        double q = x/guess;
        double ng = (q + guess) / 2;
        return ng;
    }
    static int mySqrt(int x) {
        if (x < 0)
        {
            return 0;
        }
        double dx = (double)x;
        double guess = 1.0;
        double diff = 1.0;
        while (diff > 0.01)
        {
            double ng = new_guess(guess, dx);
            diff = ng - guess;
            if (diff < 0) {
                diff = -1.0 * diff;
            }
            guess = ng;
        }
        return (int)guess;
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
#define _SQRT(_v) do { \
    printf("sqrt(%12d): %12d\n", _v, Solution::mySqrt(_v)); \
} while(0)
    _SQRT(-1);
    _SQRT(0);
    _SQRT(121);
    _SQRT(144);
    _SQRT(49);
    return 0;
}
