#include <string>
#include <stdio.h>
#include <vector>
class Solution {
public:
    static int _nd(int a_pos,
                   std::string &a_str,
                   std::vector<int>& a_memo)
    {
        int n = a_str.size();
        if (a_str[a_pos] == '0') {
            return 0;
        }
        if (a_pos == a_str.size()) {
            return 1;
        }
        if (a_memo[a_pos] != -1) {
            return a_memo[a_pos];
        }
        int l_val = _nd(a_pos+1, a_str, a_memo);
        if((a_pos+1 < a_str.size()) &&
           ((a_str[a_pos]=='1') ||
            (a_str[a_pos]=='2') && 
            (a_str[a_pos+1]<='6'))) {
            l_val += _nd(a_pos+2, a_str, a_memo);
        }
        return a_memo[a_pos] = l_val;
    }
    static int numDecodings(std::string s) {
        std::vector<int> l_memo(s.size()+1,-1);
        return _nd(0, s, l_memo);
    }
};
int main(void)
{
#define _TEST(_str) do { \
    printf("str: [%s] num ways: %d\n", _str, Solution::numDecodings(_str)); \
    } while(0)
    //_TEST("12");
    //_TEST("226");
    //_TEST("06");
    //_TEST("27");
    //_TEST("1123");
    _TEST("2101");
    return 0;
}