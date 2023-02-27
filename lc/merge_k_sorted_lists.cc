//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <queue>
#include <list>
#include <vector>
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef std::list<int> int_list_t;
//! ----------------------------------------------------------------------------
//! ListNode
//! ----------------------------------------------------------------------------
struct ListNode {
  int val;
  ListNode *next;
  ListNode(int_list_t a_list):
    val(0),
    next(nullptr)
  {
        printf("INSERTING!!!\n");
        ListNode* l_cur = this;
        ListNode* l_prev = nullptr;
        for(auto && i_v : a_list)
        {
            printf("append i_v: %d\n", i_v);
            ListNode* l_ln = new ListNode();
            l_cur->val = i_v;
            l_cur->next = l_ln;
            l_prev = l_cur;
            l_cur = l_ln;
        }
        // hack pop off last
        delete l_cur;
        l_prev->next = nullptr;
  }
  ListNode():
    val(0),
    next(nullptr)
  {}
  ListNode(int x):
    val(x),
    next(nullptr)
  {}
  ListNode(int x, ListNode *next):
    val(x),
    next(next)
  {}
};
//! ----------------------------------------------------------------------------
//! solution
//! ----------------------------------------------------------------------------
class Solution {
public:
  static ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    ListNode* l_res = nullptr;
    typedef std::priority_queue<int, std::vector<int>, std::greater<int>> int_pq_t;
    int_pq_t l_pq;
    // insert all lists into a priority queue
    for (auto && i_l : lists)
    {
          ListNode* l_cur = i_l;
          while (l_cur)
          {
            l_pq.push(l_cur->val);
            l_cur = l_cur->next;
          }
    }
    // flush priority queue back into list
    if (!l_pq.size())
    {
        return nullptr;
    }
    ListNode* l_cur = nullptr;
    ListNode* l_last = nullptr;
    do {
        l_cur = new ListNode();
        if (l_last)
        {
            l_last->next = l_cur;
        }
        l_cur->val = l_pq.top();
        l_pq.pop();
        if (!l_res)
        {
            l_res = l_cur;
        }
        l_last = l_cur;
    } while (!l_pq.empty());
    return l_res;
  }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
  typedef std::vector<ListNode*> list_vec_t;
  list_vec_t l_vec = {
    new ListNode((int_list_t){1,4,5}),
    new ListNode((int_list_t){1,3,4}),
    new ListNode((int_list_t){2,6})
  };
  ListNode* l_res = nullptr;
  l_res = Solution::mergeKLists(l_vec);
  ListNode* l_cur = l_res;
  printf("Result: [");
  while (l_cur)
  {
    printf("%d, ", l_cur->val);
    l_cur = l_cur->next;
  }
  printf("]\n");
  return 0;
}