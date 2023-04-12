#include <stdio.h>
#include <stack>
#include <string>

using namespace std;
class Solution {
public:
    static bool isValid(string s) {
        stack<char> st;
        for (auto && c : s) {
            if (c == '(' || c == '{' || c == '[') {
                st.push(c);
            }
            else if (c == ')' || c == '}' || c == ']') {
                if (st.empty()) {
                    return false;
                }
                char t = st.top();
                if (t == '(' && c == ')') {
                    st.pop();
                }
                else if (t == '{' && c == '}') {
                    st.pop();
                }
                else if (t == '[' && c == ']') {
                    st.pop();
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        if (st.empty()) {
            return true;
        }
        return false;
    }
};

int main(void) {
    {
        string s = "]";
        printf("valid '%s': %d\n", s.c_str(), Solution::isValid(s));
    }
    return 0;
}