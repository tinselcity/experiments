#!/usr/bin/env python3

class Solution:
    def isAnagram(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        
        countS, countT = {}, {}

        for i in range(len(s)):
            countS[s[i]] = 1 + countS.get(s[i], 0)
            countT[t[i]] = 1 + countT.get(s[i], 0)
        for c in countS:
            print('compare: {} {} =? {}'.format(c, countS[c], countT.get(c, 0)))
            if countS[c] != countT.get(c, 0):
                return False
        
        return True


def printResult(s: str, t: str):
    soln = Solution()
    print('{} {}: isAnagram: {}'.format(s, t, soln.isAnagram(s, t)))

printResult('anagram', 'nagaram')
printResult('rat', 'car')
