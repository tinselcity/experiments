//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <list>
//! ----------------------------------------------------------------------------
//! constants
//! ----------------------------------------------------------------------------
#define _NN -1234567
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef std::vector<int> int_vec_t;
typedef std::list<int> int_list_t;
//! ----------------------------------------------------------------------------
//! \class TreeNode
//! ----------------------------------------------------------------------------
struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};
typedef std::list<TreeNode*> p_list_t;
//! ----------------------------------------------------------------------------
//! macros
//! ----------------------------------------------------------------------------
#define NDBG_PRINT(...) \
    do { \
        fprintf(stdout, "%s:%s.%d: ", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stdout, __VA_ARGS__);               \
        fflush(stdout); \
    } while(0)
//! ----------------------------------------------------------------------------
//! depth first tree walk (not search)
//! ----------------------------------------------------------------------------
void dfs_walk(TreeNode* a_node)
{
    if (!a_node)
    {
        return;
    }
    dfs_walk(a_node->left);
    printf(": %d\n", a_node->val);
    dfs_walk(a_node->right);
}
//! ----------------------------------------------------------------------------
//! breadth first tree walk (not search)
//! ----------------------------------------------------------------------------
void bfs_walk(TreeNode* a_node)
{
    if (!a_node)
    {
        return;
    }
    p_list_t l_p_list;
    l_p_list.push_back(a_node);
    while(!l_p_list.empty())
    {
        TreeNode* l_node = l_p_list.front();
        l_p_list.pop_front();
        printf(": %d\n", l_node->val);
        if (l_node->left != nullptr)
        {
            l_p_list.push_back(l_node->left);
        }
        if (l_node->right != nullptr)
        {
            l_p_list.push_back(l_node->right);
        }
    }
}
//! ----------------------------------------------------------------------------
//! breadth first tree walk (not search)
//! ----------------------------------------------------------------------------
void bfs_walk_recursive(p_list_t& a_q, int a_level)
{
    if (a_q.empty()) {
        return;
    }
    TreeNode* l_node = a_q.front();
    a_q.pop_front();
    printf(": LEVEL[%d] VALUE: %d\n", a_level, l_node->val);
    if (l_node->left != nullptr)
    {
        a_q.push_back(l_node->left);
    }
    if (l_node->right != nullptr)
    {
        a_q.push_back(l_node->right);
    }
    return bfs_walk_recursive(a_q, a_level+1);
}
//! ----------------------------------------------------------------------------
//! breadth first construction
//! ----------------------------------------------------------------------------
TreeNode* bfs_construct(int_vec_t a_vec)
{
    if (a_vec.empty() ||
        (a_vec[0] == _NN))
    {
        return nullptr;
    }
    size_t l_cur_idx = 0;
    TreeNode* l_root = new TreeNode(a_vec[l_cur_idx], nullptr, nullptr);
    ++l_cur_idx;
    p_list_t l_p_list;
    l_p_list.push_back(l_root);
    while (l_cur_idx < a_vec.size())
    {
        // -------------------------------------------------
        // current parent
        // -------------------------------------------------
        auto l_cur_p = l_p_list.front();
        // -------------------------------------------------
        // left hand side
        // -------------------------------------------------
        int l_lhs = a_vec[l_cur_idx];
        ++l_cur_idx;
        if (l_lhs != _NN)
        {
            //NDBG_PRINT("[LHS]: %d -> %d\n", l_cur_p->val, l_lhs);
            TreeNode* l_node = new TreeNode(l_lhs, nullptr, nullptr);
            l_cur_p->left = l_node;
            l_p_list.push_back(l_node);
        }
        // -------------------------------------------------
        // right hand side
        // -------------------------------------------------
        int l_rhs = a_vec[l_cur_idx];
        ++l_cur_idx;
        if (l_rhs != _NN)
        {
            //NDBG_PRINT("[RHS]: %d -> %d\n", l_cur_p->val, l_rhs);
            TreeNode* l_node = new TreeNode(l_rhs, nullptr, nullptr);
            l_cur_p->right = l_node;
            l_p_list.push_back(l_node);
        }
        l_p_list.pop_front();
    }
    return l_root;
}
//! ----------------------------------------------------------------------------
//! test if is valid binary search tree
//! ----------------------------------------------------------------------------
TreeNode* g_prev = nullptr;
class Solution {
public:
    static bool isValidBST(TreeNode* root)
    {
            if (root == nullptr) {
                return true;
            }
            // walk left
            if (!isValidBST(root->left))
            {
                return false;
            }
            // compare left with parent
            if ((g_prev != nullptr) &&
                root->val <= g_prev->val)
            {
                return false;
            }
            // set new previous
            g_prev = root;
            // walk right
            return isValidBST(root->right);
    }
};
//! ----------------------------------------------------------------------------
//! test
//! ----------------------------------------------------------------------------
static void _test(int_vec_t l_vec)
{
    //NDBG_PRINT(":: CON ::\n");
    printf(":: TREE:     [");
    for (auto& i_v : l_vec) {
        printf("%d", i_v);
        if (i_v != l_vec.back())
        {
            printf(", ");
        }
    }
    printf("]\n");
    TreeNode* l_root = bfs_construct(l_vec);
    g_prev = nullptr;
    bool l_s = Solution::isValidBST(l_root);
    printf(":: IS VALID: %s\n", l_s ? "TRUE": "FALSE");
    NDBG_PRINT(":: DFS ::\n");
    dfs_walk(l_root);
    NDBG_PRINT(":: BFS ::\n");
    bfs_walk(l_root);
    NDBG_PRINT(":: BFS RECURSIVE::\n");
    p_list_t l_p_list;
    l_p_list.push_back(l_root);
    bfs_walk_recursive(l_p_list, 0);
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    _test({2,1,3});
    _test({5,1,4,8,13,3,6});
    _test({5,3,8,1,4,6,13});
    _test({5,1,4,_NN,_NN,3,6});
    return 0;
}