#include <stdio.h>
#include <vector>
#include <unordered_set>
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
    static int longestConsecutive(std::vector<int>& nums) {
        // stuff all into a set
        std::unordered_set <int>l_set;
        for (auto && i_v : nums) {
            l_set.insert(i_v);
        }
        // walk thru list
        int l_max = 0;
        for (auto && i_v : l_set) {
            // if no left neighbor -start of sequence
            auto i_f = l_set.find(i_v-1);
            int l_len = 1;
            if (i_f == l_set.end()) {
                printf("check v: %d\n", i_v);
                // calculate seq length
                int l_val = i_v+1;
                while (l_set.find(l_val) != l_set.end()) {
                    ++l_len;
                    ++l_val;
                }
            }
            if (l_len > l_max) {
                l_max = l_len;
            }
        }
        return l_max;
    }
};
int main(void) {
    {
        std::vector<int> l_v = {100,4,200,1,3,2};
        int l_s = Solution::longestConsecutive(l_v);
        _PRINT_VEC(l_v);
        printf(" soln: %d\n", l_s);
    }
    {
        std::vector<int> l_v = {0,3,7,2,5,8,4,6,0,1};
        int l_s = Solution::longestConsecutive(l_v);
        _PRINT_VEC(l_v);
        printf(" soln: %d\n", l_s);
    }
    return 0;
}