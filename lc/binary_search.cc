#include <stdio.h>
#include <vector>
using namespace std;

class Solution {
public:
    static int search(vector<int>& nums, int target) {
        int start = 0;
        int end = nums.size()-1;
        while (start <= end) {
            int mid=(start+end)/2;
            int v = nums[mid];
            if (target == v) {
                return mid;
            }
            else if(target < v) {
                end = mid-1;
            }
            else {
                start = mid+1;
            }
        }
        return -1;
    }
};

int main(void) {
    {
        vector<int> v = {-1,0,3,5,9,12};
        int r = Solution::search(v, 2);
        printf("result: %d\n", r);
    }
    return 0;
}