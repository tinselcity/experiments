#!/usr/bin/env python3
from typing import List

class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        '''
        for idx, val in enumerate(nums):
            for subidx, subval in enumerate(nums[idx+1:]):
                print('testing: {}[{}] + {}[{}] == {}'.format(val, idx, subval, subidx+idx+1, target))
                if val + subval == target:
                    return [idx, subidx+idx+1]
        '''
        hmap = {}
        for idx, val in enumerate(nums):
            desired = target - val
            if desired in hmap:
                return [hmap[desired], idx]
            hmap[val] = idx
        return None


c = Solution()
my_input = [0, 0, 2, 7, 11, 15]
my_target = 9
result = c.twoSum(my_input, my_target)
print('result: {}'.format(result))