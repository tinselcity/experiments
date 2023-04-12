#include <stdio.h>
#include <string>
#include <stack>

class Solution {
public:
    static std::string simplifyPath(std::string path) {
        std::stack<std::string> l_ps;
        std::string l_cur = "";
        path += "/";
        for (size_t i_idx = 0; i_idx < path.length(); ++i_idx) {
            if (path[i_idx] == '/') {
                if (l_cur == "..") {
                    if (!l_ps.empty()) {
                        l_ps.pop();
                    }
                }
                else if (!l_cur.empty() &&
                         (l_cur != ".")) {
                    l_ps.push(l_cur);
                }
                l_cur.clear();
            }
            else {
                l_cur += path[i_idx];
            }
        }
        std::string l_ret;
        while (!l_ps.empty()) {
            printf("TOP: %s\n", l_ps.top().c_str());
            if (l_ret.empty()) {
                l_ret = l_ps.top();
            } else {
                l_ret = l_ps.top() + "/" + l_ret;
            }
            l_ps.pop();
        }
        l_ret = "/" + l_ret;
        return l_ret;
    }
};

int main(void) {
#define _SIMPLIFY(_s) do { \
        printf("Simplified %s --> %s\n", _s, Solution::simplifyPath(_s).c_str()); \
} while(0)
    _SIMPLIFY("/../abc//./def/");
    return 0;
}