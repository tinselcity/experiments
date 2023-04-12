#include <stdio.h>
#include <vector>
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
    static int _fm(const std::vector<int>&nums, size_t left, size_t right) {
        if (nums[left] < nums[right]) {
            return nums[0];
        }
        size_t middle = ((right-left) / 2) + left;
        if (nums[middle] > nums[middle+1]) {
            return nums[middle+1];
        }
        else if (nums[middle] < nums[middle-1]) {
            return nums[middle];
        }
        if (nums[right] > nums[middle]) {
            return _fm(nums, left, middle);
        }
        else {
            return _fm(nums, middle+1, right);
        }
    }
    static int findMin(std::vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        } else if (nums.size() == 1) {
            return nums[0];
        }
        return _fm(nums, 0, nums.size()-1);
    }
};
int main(void) {
    {
        std::vector<int> l_v = {1,2,3,4,5};
        int l_r = Solution::findMin(l_v);
        _PRINT_VEC(l_v); printf(": %d\n", l_r);
    }
    {
        std::vector<int> l_v = {3,4,5,1,2};
        int l_r = Solution::findMin(l_v);
        _PRINT_VEC(l_v); printf(": %d\n", l_r);
    }
    return 0;
}