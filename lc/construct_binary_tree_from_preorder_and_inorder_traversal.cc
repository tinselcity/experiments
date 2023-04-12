#include <stdio.h>
#include <vector>
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

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

class Solution {
public:
    typedef std::vector<int> vint_t;
    static TreeNode* _bt(const vint_t& a_pre,
               int a_pre_start,
               int a_pre_end,
               const vint_t& a_ino,
               int a_ino_start,
               int a_ino_end) {
            // root is start of pre order
            int l_root_val = a_pre[a_pre_start];
            TreeNode* l_root = new TreeNode(l_root_val);
            if (a_pre_start == a_pre_end) {
                return l_root;
            }
            // find the root in the in order and bisect
            size_t l_mid = a_ino_start;
            for (; l_mid != a_ino_end; ++l_mid) {
                if (a_ino[l_mid] == l_root_val) {
                    break;
                }
            }
            // pass left tree
            int l_mid_pre_len = l_mid-a_ino_start;
            int l_mid_pst_len = a_ino_end - l_mid;
            if (l_mid_pre_len) {
                l_root->left = _bt(a_pre, a_pre_start+1, a_pre_start+l_mid_pre_len,
                                a_ino, a_ino_start, a_ino_start+l_mid_pre_len-1);
            }
            // pass right tree
            if (l_mid_pst_len) {
                l_root->right = _bt(a_pre, a_pre_end-l_mid_pst_len+1, a_pre_end,
                                a_ino, a_ino_end-l_mid_pst_len+1, a_ino_end);
            }
            return l_root;
    }
    static TreeNode* buildTree(std::vector<int>& preorder, std::vector<int>& inorder) {
        TreeNode* l_ret = nullptr;
        if (preorder.empty()) {
            return l_ret;
        }
        return l_ret = _bt(preorder, 0, preorder.size()-1, inorder, 0, inorder.size()-1);
    }
};

int main(void) {
    typedef std::vector<int> vint_t;
#if 0
    {
        vint_t l_pre = {3,9,20,15,7};
        vint_t l_ino = {9,3,15,20,7};
        TreeNode* l_root = Solution::buildTree(l_pre, l_ino);
        dfs_walk(l_root);
    }
#endif
    {
        vint_t l_pre = {1,2};
        vint_t l_ino = {2,1};
        TreeNode* l_root = Solution::buildTree(l_pre, l_ino);
        dfs_walk(l_root);
    }
    return 0;
}