#include <stdio.h>
#include <vector>
class Solution {
public:
    typedef std::vector<std::vector<int>> vec_vec_int_t;
    static int _uniquePaths(vec_vec_int_t& a_memo, int a_i, int a_j) {
        // out of bounds
        if ((a_i >= std::size(a_memo)) ||
            (a_j >= std::size(a_memo[0]))) {
            return 0;
        }
        // reached end
        if ((a_i == std::size(a_memo)-1) &&
            (a_j == std::size(a_memo[0])-1)) {
            return 1;
        }
        // if in memo
        if (a_memo[a_i][a_j]) {
            return a_memo[a_i][a_j];
        }
        // store result
        return a_memo[a_i][a_j] = _uniquePaths(a_memo, a_i+1, a_j) +
                                  _uniquePaths(a_memo, a_i, a_j+1);
    }
    static int uniquePaths(int m, int n) {
        vec_vec_int_t l_memo(m, std::vector<int>(n));
        return _uniquePaths(l_memo, 0, 0);
    }
};
int main(void)
{
    #define _TEST(_m, _n) do { \
        printf("num ways [%d, %d]: %d\n", _m,_n, Solution::uniquePaths(_m,_n)); \
    } while(0)
    _TEST(3, 7);
    _TEST(3, 2);
}