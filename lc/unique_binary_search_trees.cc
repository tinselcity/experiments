#include <stdio.h>
#include <array>
using namespace std;

class Solution {
public:
    static int _dp(int n, array<int, 20>& memo) {
        if (memo[n] != 0) {
            return memo[n];
        }
        int res = 0;
        for (int i = 1; i < n+1; ++i) {
            res += _dp(i-1, memo)*_dp(n-i, memo);
        }
        memo[n] = res;
        return res;
    }
    static int numTrees(int n) {
        array<int, 20> memo = {0};
        memo[0] = 1;
        memo[1] = 1;
        return _dp(n, memo);
    }
};

int main(void) {
    {
    int n = 3;
    printf("input: %d output: %d\n", n, Solution::numTrees(n));
    }
    {
    int n = 1;
    printf("input: %d output: %d\n", n, Solution::numTrees(n));
    }
}