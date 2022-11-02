//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_set>
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto& i_v : _v) { \
        printf("%s", i_v.c_str()); \
        if (i_v != _v.back()) { printf(", "); }\
     } printf("]"); \
} while(0)
//! ----------------------------------------------------------------------------
//! \class Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    static bool wordBreak(std::string s, std::vector<std::string>& wordDict) {
        // -------------------------------------------------
        // put in map
        // -------------------------------------------------
        std::unordered_set<std::string> l_dict(wordDict.begin(), wordDict.end());
        std::vector<bool> l_match_offset(s.length() + 1);
        l_match_offset[0] = true;
        // -------------------------------------------------
        // walk backwards looking for a match
        // -------------------------------------------------
        for (size_t i_cur = 1; i_cur <= s.size(); ++i_cur) {
            for (size_t i_off = 0; i_off < i_cur; ++i_off) {
                // if match
                if (!l_match_offset[i_off])
                {
                    continue;
                }
                if (l_dict.find(s.substr(i_off, i_cur - i_off)) !=  l_dict.end()) {
                    l_match_offset[i_cur] = true;
                    break;
                }
            }
        }
        if (l_match_offset[s.length()])
        {
            return true;
        }
        return false;
    }
};
//! ----------------------------------------------------------------------------
//! \function test
//! ----------------------------------------------------------------------------
void _test(std::string s, std::vector<std::string> wordDict)
{
    printf("BREAK:     %s\n", s.c_str());
    printf("DICT:      "); _PRINT_VEC(wordDict); printf("\n");
    bool l_s = Solution::wordBreak(s, wordDict);
    printf("CAN BREAK: %s\n", l_s ? "TRUE": "FALSE");
}
//! ----------------------------------------------------------------------------
//! \function main
//! ----------------------------------------------------------------------------
int main(void)
{
    _test("leetcode", {"leet", "code"});
    _test("aaaaaaa", {"aaaa", "aa"});
    _test("aaaaaaa", {"aaaa", "aaa"});
    return 0;
}