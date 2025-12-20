#!/usr/bin/env python3
from typing import List

class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        # brute
        '''
        max = 0
        for i, b in enumerate(prices):
            for j, s in enumerate(prices[i+1:]):
                profit = s - b
                if profit > max:
                    max = profit
        return max
        '''
        # l/r
        b = 0
        s = 1
        pMax = 0

        while s < len(prices):
            #print('b[{}] s[{}]'.format(b, s))
            profit = prices[s] - prices[b]
            if profit > 0:
                pMax = max(pMax, profit)
            else:
                b = s
            s += 1
        return pMax


s = Solution()
prices = [7,1,5,3,6,4]
result = s.maxProfit(prices)
print('max profit: {}'.format(result))