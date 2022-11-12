#!/usr/bin/env python3
class Solution:
	@staticmethod
	def findDuplicate(nums: list[int]) -> int:
		slow = 0
		fast = 0
		while True:
			slow = nums[slow]
			fast = nums[nums[fast]]
			if slow == fast:
				break

		slow2 = 0
		while True:
			slow = nums[slow]
			slow2 = nums[slow2]
			if slow == slow2:
				break
		return slow

l_list = [1,3,4,2,2]
l_res = Solution.findDuplicate(l_list)
print('{}: result: {}'.format(l_list, l_res))

l_list = [3,1,3,3,2]
l_res = Solution.findDuplicate(l_list)
print('{}: result: {}'.format(l_list, l_res))
