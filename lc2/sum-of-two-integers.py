#!/usr/bin/env python3


class Solution:
    def getSum(self, a: int, b: int) -> int:
        # val   = a xor b
        # carry = a & b << 1
        return 0

def printGetSum(a: int, b: int):
    s = Solution()
    print('{} + {} = {}'.format(a, b, s.getSum(a, b)))

printGetSum(10, 13)
