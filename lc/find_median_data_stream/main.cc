//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <queue>
//! ----------------------------------------------------------------------------
//! MedianFinder
//! ----------------------------------------------------------------------------
class MedianFinder {
public:
  typedef std::priority_queue<int> max_heap_t;
  typedef std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap_t;
  max_heap_t m_left;
  min_heap_t m_right;
  MedianFinder() {
  }
  void addNum(int num) {
    printf("ADD NUM: %d\n", num);
    m_left.push(num);
    if (m_left.size())
    {
        m_right.push(m_left.top());
        m_left.pop();
    }
    printf("m_left size:  %lu\n", m_left.size());
    printf("m_right size: %lu\n", m_right.size());
    while (m_left.size() < m_right.size())
    {
        printf("rebalance!\n");
        m_left.push(m_right.top());
        m_right.pop();
    }
  }
  double findMedian() {
    if (!m_left.size() &&
        !m_right.size())
    {
        return 0.0;
    }
    if (m_left.size() > m_right.size())
    {
        return m_left.top();
    }
    else
    {
        return (m_right.top() + m_left.top()) / 2.0;
    }
  }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
  MedianFinder medianFinder;
  medianFinder.addNum(1);
  medianFinder.addNum(2);
  // return 1.5 (i.e., (1 + 2) / 2)
  printf("median: %8.2f\n", medianFinder.findMedian());
  medianFinder.addNum(3);
  medianFinder.addNum(8);
  medianFinder.addNum(19);
  medianFinder.addNum(21);
  medianFinder.addNum(6548);
  medianFinder.addNum(4);
  medianFinder.addNum(87);
  medianFinder.addNum(-1000);
  // return 2.0
  printf("median: %8.2f\n", medianFinder.findMedian());
  return 0;
}