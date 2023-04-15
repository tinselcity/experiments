#include <string>
#include <queue>
#include <array>
#include <stdio.h>

using namespace std;


class Solution {
public:
    typedef pair<char, int> letter_t;
    class letter_compare {
    public:
            bool operator()(letter_t& t1, letter_t& t2) {
                    return (t1.second < t2.second);
            }
    };
    typedef priority_queue<letter_t, std::vector<letter_t>, letter_compare> maxq_t; 
    static string reorganizeString(string s) {
        string ret;
        // get counts
        array<int, 26> cnt = {0};
        for (auto && c : s) {
            cnt[int(c-'a')]++;
        }
        maxq_t maxq;
        for (int i = 0; i < 26; ++i) {
            if (cnt[i] > 0) {
                maxq.push({i, cnt[i]});
            }
        }
        while (!maxq.empty()) {
            letter_t t = maxq.top();
            maxq.pop();
            if (int(ret.back()-'a') == t.first) {
                if (maxq.empty()) {
                    return "";
                }
                letter_t n = maxq.top();
                if (n.second <= 0) {
                    return "";
                }
                maxq.pop();
                maxq.push(t);
                ret += (char)('a'+n.first);
                n.second -= 1;
                if (n.second > 0) {
                    maxq.push(n);
                }
                continue;
            }
            ret += (char)('a'+t.first);
            t.second -= 1;
            if (t.second > 0) {
                maxq.push(t);
            }
        }
        return ret;
    }
};

int main(void) {
    {
        string s = "aaabc";
        printf("solution[%s]: %s\n", s.c_str(), Solution::reorganizeString(s).c_str());
    }
}