#!/usr/bin/env python3

class MedianFinder:
    def __init__(self):
        self.small, self.larger = [], []
    def addNum(self, num: int) -> None:
        heapq.heappush(self.small, -1*num)
    def findMedian(self) -> float:
        pass

# Your MedianFinder object will be instantiated and called as such:
# obj = MedianFinder()
# obj.addNum(num)
# param_2 = obj.findMedian()
medianFinder = MedianFinder()
medianFinder.addNum(1)    #// arr = [1]
medianFinder.addNum(2)    #// arr = [1, 2]
medianFinder.findMedian() #// return 1.5 (i.e., (1 + 2) / 2)
medianFinder.addNum(3)    #// arr[1, 2, 3]
medianFinder.findMedian() #// return 2.0