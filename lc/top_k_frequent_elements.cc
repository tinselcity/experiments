#include <stdio.h>
#include <vector>
#include <map>
using namespace std;

#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto&& i_n : _v) { \
        printf("%d", i_n); \
        if (&i_n != &_v.back()) { printf(",");} \
    } \
    printf("]"); \
} while(0)

class Solution {
public:
    static vector<int> topKFrequent(vector<int>& nums, int k) {
        map<int, int> m; // value, count
        for (auto && n : nums) {
            ++m[n];
        }
        vector<vector<int>> fm(nums.size()+1, vector<int>(0));
        for (auto && n : m) {
            fm[n.second].push_back(n.first);
        }
        vector<int> r;
        for (int i = nums.size(); i >= 0; --i) {
            for (auto && v : fm[i]) {
                r.push_back(v);
                --k;
                if (!k) { break;}
            }
            if (!k) { break;}
        }
        return r;
    }
};

int main(void) {
    {
        vector<int> v = {1};
        int k = 1;
        vector<int> r = Solution::topKFrequent(v, k);
        _PRINT_VEC(r); printf("\n");
    }
    return 0;
}