#!/usr/bin/env python3
from typing import Optional
from collections import deque

# Definition for a binary tree node.
class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def maxDepth(self, root: Optional[TreeNode]) -> int:
        if not root:
            return 0
        '''
        return 1 + max(self.maxDepth(root.left), self.maxDepth(root.right))
        '''
        level = 1
        q = deque([root])

        while q:
            for i in range(len(q)):
                node = q.popleft()
                if node.left:
                    q.append(node.left)
                if node.right:
                    q.append(node.right)
            
            level += 1

        return level



s = Solution()
root = [3,9,20,None,None,15,7]
print('result: {}'.format(s.maxDepth(root)))
