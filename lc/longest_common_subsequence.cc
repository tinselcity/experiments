//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <vector>
//! ----------------------------------------------------------------------------
//! Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int longestCommonSubsequence(std::string text1, std::string text2) {
        if (text1.empty() || text2.empty()) {
            return 0;
        }
        std::vector <std::vector <int>> l_memo(text1.length()+1, std::vector<int>(text2.length()+1, 0));
        for (int i_1 = text1.length()-1; i_1 >= 0; --i_1) {
            for (int i_2 = text2.length()-1; i_2 >= 0; --i_2) {
                if (text1[i_1] == text2[i_2]){
                    l_memo[i_1][i_2] = 1 + l_memo[i_1+1][i_2+1];
                }
                else {
                    l_memo[i_1][i_2] = std::max(l_memo[i_1][i_2+1], l_memo[i_1+1][i_2]);
                }
            }
        }
        return l_memo[0][0];
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
#define _TEST(_a, _b) do { \
    int _val = Solution::longestCommonSubsequence(_a, _b); \
    printf("LCS: [%s] [%s]: %d\n", _a, _b, _val); \
    } while(0);
    _TEST("abcde", "ace");
    _TEST("abc", "abc");
    _TEST("abc", "def");
    _TEST("mhunuzqrkzsnidwbun", "szulspmhwpazoxijwbq");
    _TEST("mhunuzqrkzsni", "szulspmhw");
    return 0;
}
