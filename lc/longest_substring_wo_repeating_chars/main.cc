//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <map>
#include <array>
//! ----------------------------------------------------------------------------
//! Solutions
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int lengthOfLongestSubstring(std::string s) {
        if(!s[0]) {
            return 0;
        }
        else if(!s[1]) {
            return 1;
        }
        int l_cur_max_len=1;
        int l_max_len=1;
        int j_start=0;
        for(int i=1; s[i]; ++i)
        {
            printf("INCR: s[%d](%c) ********** j_start[%d] < %d\n", i, s[i], j_start, i);
            for(int j=j_start; j<i; ++j)
            {
                printf("TEST: s[%d](%c) != s[%d](%c)\n", i, s[i], j, s[j]);
                if(s[i] != s[j]) {
                    ++l_cur_max_len;
                }
                else {
                    j_start = j+1;
                    printf("SETJ: %d\n", j_start);
                    break;
                }
            }
            if(l_max_len < l_cur_max_len) {
                l_max_len = l_cur_max_len;
            }
            l_cur_max_len = 1;
        }
        return l_max_len;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
#define _TEST(_s) do { \
    printf("%s: %d\n", _s, Solution::lengthOfLongestSubstring(_s)); \
} while(0)
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    _TEST("0abcabcbb");
#if 0
    _TEST("bbbbb");
    _TEST("pwwkew");
    _TEST("a");
    _TEST(" ");
    _TEST("au");
#endif
    return 0;
}
