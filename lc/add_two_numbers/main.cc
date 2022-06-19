#include <stdio.h>
#include <vector>
//! ----------------------------------------------------------------------------
//! Definition for singly-linked list.
//! ----------------------------------------------------------------------------
struct ListNode {
    int val;
    ListNode *next;
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
//! 
//! ----------------------------------------------------------------------------
class Solution {
public:
    static ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* l_ln = nullptr;
        ListNode* l_head = nullptr;
        int l_carry = 0;
        while ((l1) ||
               (l2))
        {
            int l_sum = ((l1) ? l1->val : 0) +
                        ((l2) ? l2->val : 0) +
                        l_carry;
            l_carry = l_sum / 10;
            l_sum = l_sum % 10;
            ListNode* l_new_node = new ListNode(l_sum);
            if (!l_head)
            {
                l_head = l_new_node;
            }
            if (l_ln)
            {
                l_ln->next = l_new_node;
            }
            l_ln = l_new_node;
            if (l1) { l1 = l1->next;}
            if (l2) { l2 = l2->next;}
        }
        if (l_carry)
        {
            ListNode* l_new_node = new ListNode(l_carry);
            l_ln->next = l_new_node;
        }
        return l_head;
    }
};
//! ----------------------------------------------------------------------------
//! print list
//! ----------------------------------------------------------------------------
void print_list(ListNode* a_list)
{
    printf("[");
    while (a_list)
    {
        printf("%d", a_list->val);
        if (a_list->next)
        {
        printf(",");            
        }
        a_list = a_list->next;
    }
    printf("]\n");
}
//! ----------------------------------------------------------------------------
//! create list from vector
//! ----------------------------------------------------------------------------
typedef std::vector <int> int_vec_t;
ListNode* create_list(int_vec_t& a_vec)
{
    ListNode* l_ln = nullptr;
    ListNode* l_head = nullptr;
    for (auto && i_v : a_vec)
    {
        ListNode* l_new_node = new ListNode(i_v);
        if (!l_head)
        {
            l_head = l_new_node;
        }
        if (l_ln)
        {
            l_ln->next = l_new_node;
        }
        l_ln = l_new_node;
    }
    return l_head;
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    int_vec_t l_ex1_l1_v = {2,4,3};
    ListNode* l_ex1_l1 = create_list(l_ex1_l1_v);
    print_list(l_ex1_l1);
    int_vec_t l_ex1_l2_v = {5,6,4};
    ListNode* l_ex1_l2 = create_list(l_ex1_l2_v);
    print_list(l_ex1_l2);
    ListNode* l_ex1_sum = Solution::addTwoNumbers(l_ex1_l1, l_ex1_l2);
    print_list(l_ex1_sum);
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    int_vec_t l_ex2_l1_v = {9};
    ListNode* l_ex2_l1 = create_list(l_ex2_l1_v);
    print_list(l_ex2_l1);
    int_vec_t l_ex2_l2_v = {1};
    ListNode* l_ex2_l2 = create_list(l_ex2_l2_v);
    print_list(l_ex2_l2);
    ListNode* l_ex2_sum = Solution::addTwoNumbers(l_ex2_l1, l_ex2_l2);
    print_list(l_ex2_sum);
    // -----------------------------------------------------
    // inputs
    // -----------------------------------------------------
    int_vec_t l_ex3_l1_v = {9,9,9,9,9,9,9};
    ListNode* l_ex3_l1 = create_list(l_ex3_l1_v);
    print_list(l_ex3_l1);
    int_vec_t l_ex3_l2_v = {9,9,9,9};
    ListNode* l_ex3_l2 = create_list(l_ex3_l2_v);
    print_list(l_ex3_l2);
    ListNode* l_ex3_sum = Solution::addTwoNumbers(l_ex3_l1, l_ex3_l2);
    print_list(l_ex3_sum);
    return 0;
}
