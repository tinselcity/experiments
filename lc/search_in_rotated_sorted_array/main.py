#!/usr/bin/env python3
class Solution:
	@staticmethod
	def search(nums: list[int], target: int) -> int:
		l = 0
		r = len(nums) - 1

		while l <= r:
			print('l[{}] -> r[{}]'.format(l, r))
			mid = (l+r)//2
			val_lft = nums[l]
			val_mid = nums[mid]
			val_rgt = nums[r]
			print('l[{}]: {} m[{}]: {} r[{}]: {}'.format(l, val_lft, mid, val_mid, r, val_rgt))
			if target == val_mid:
				return mid

			# left sorted portion
			if val_lft <= val_mid:
				if target > val_mid or target < val_lft:
					l = mid+1
				else:
					r = mid-1

			# right sorted
			else:
				if target < val_mid or target > val_rgt:
					r = mid-1
				else:
					l = mid+1
		return -1

l_lst = [4,5,6,7,0,1,2]
l_res = Solution.search(l_lst, 0)
print('{}: {}'.format(l_lst, l_res))