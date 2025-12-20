#!/usr/bin/env python3

class Solution:
    def __init__(self):
        self.memo = {}

    def climbStairs(self, n: int) -> int:
        if n <= 1:
            return 1
        if (n-1) in self.memo:
            n1 = self.memo[n-1]
        return self.climbStairs(n-1) + self.climbStairs(n-2)

s = Solution()
print('result: {}'.format(s.climbStairs(2)))