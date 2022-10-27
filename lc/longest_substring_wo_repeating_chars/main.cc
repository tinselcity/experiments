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
        int l_l_start=0;
        for(int l_r_idx=1; s[l_r_idx]; ++l_r_idx)
        {
            for(int l_l_idx=l_l_start; l_l_idx<l_r_idx; ++l_l_idx)
            {
                if(s[l_r_idx] != s[l_l_idx]) {
                    ++l_cur_max_len;
                }
                else {
                    l_l_start = l_l_idx+1;
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
    _TEST("bbbbb");
    _TEST("pwwkew");
    _TEST("a");
    _TEST(" ");
    _TEST("au");
    return 0;
}
