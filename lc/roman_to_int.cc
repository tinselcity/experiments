#include <stdio.h>
#include <string>
//! ----------------------------------------------------------------------------
//!
//! ----------------------------------------------------------------------------
class Solution {
public:
    static int romanToInt(std::string s) {
        int l_result = 0;
        int l_idx = 0;
        for (std::string::iterator i_c = s.begin(); i_c != s.end(); ++i_c)
        {
            char i_n = char(0);
            if (l_idx < s.length()+1)
            {
                i_n = s[l_idx + 1];
            }
            ++l_idx;
            switch(*i_c)
            {
                case 'I':
                {
                    if (i_n == 'V')
                    {
                        l_result += 4;
                        ++i_c;
                        ++l_idx;
                    }
                    else if(i_n == 'X')
                    {
                        l_result += 9;
                        ++i_c;
                        ++l_idx;
                    }
                    else
                    {
                        l_result += 1;
                    }
                    break;
                }
                case 'V':
                {
                    l_result += 5;
                    break;
                }
                case 'X':
                {
                    if (i_n == 'L')
                    {
                        l_result += 40;
                        ++i_c;
                        ++l_idx;
                    }
                    else if(i_n == 'C')
                    {
                        l_result += 90;
                        ++i_c;
                        ++l_idx;
                    }
                    else
                    {
                        l_result += 10;
                    }
                    break;
                }
                case 'L':
                {
                    l_result += 50;
                    break;
                }
                case 'C':
                {
                    if (i_n == 'D')
                    {
                        l_result += 400;
                        ++i_c;
                        ++l_idx;
                    }
                    else if(i_n == 'M')
                    {
                        l_result += 900;
                        ++i_c;
                        ++l_idx;
                    }
                    else
                    {
                        l_result += 100;
                    }
                    break;
                }
                case 'D':
                {
                    l_result += 500;
                    break;
                }
                case 'M':
                {
                    l_result += 1000;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        return l_result;
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
#define _ROMAN_TO_INT(_v) do { \
    printf("%24s: %12d\n", _v, Solution::romanToInt(_v)); \
} while(0)
    _ROMAN_TO_INT("III");
    _ROMAN_TO_INT("LVIII");
    _ROMAN_TO_INT("MCMXCIV");
    return 0;
}
