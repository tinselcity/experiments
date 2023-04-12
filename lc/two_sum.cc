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
    static vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> v;
        map<int, int> m; // pos, value
        for (int i = 0; i < nums.size(); ++i) {
            int d = target - nums[i];
            auto f = m.find(d);
            if (f != m.end()) {
                v.push_back(f->second);
                v.push_back(i);
            }
            m[nums[i]] = i;
        }
        return v;
    }
};

int main(void) {
    {
        vector<int> v = {3,3};
        int t = 6;
        vector<int> r = Solution::twoSum(v, t);
        _PRINT_VEC(r); printf("\n");
    }
    return 0;
}