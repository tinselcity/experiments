#include <stdio.h>
#include <vector>
#include <map>
//! ----------------------------------------------------------------------------
//! \class: Node
//! ----------------------------------------------------------------------------
class Node {
public:
    Node() {
        val = 0;
        neighbors = std::vector<Node*>();
    }
    Node(int _val) {
        val = _val;
        neighbors = std::vector<Node*>();
    }
    Node(int _val, std::vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
    int val;
    std::vector<Node*> neighbors;
};
//! ----------------------------------------------------------------------------
//! Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    typedef std::map<Node*, Node*> visited_t;
    typedef std::vector<Node*>node_vec_t;
    static Node* _clone_graph(Node* a_n, visited_t& a_visited)
    {
        if (!a_n)
        {
            return nullptr;
        }
        auto && i_n = a_visited.find(a_n);
        if (i_n != a_visited.end())
        {
            return i_n->second;
        }
        Node* l_node = new Node(a_n->val);
        // record visited
        a_visited[a_n] = l_node;
        // walk neighbors dfs
        for (auto && i_nb : a_n->neighbors)
        {
            Node* l_nn = nullptr;
            l_nn = _clone_graph(*(&i_nb), a_visited);
            l_node->neighbors.push_back(l_nn);
        }
        return l_node;
    }
    static Node* cloneGraph(Node* node) {
        if (!node)
        {
            return nullptr;
        }
        visited_t l_visited;
        Node* l_node = nullptr;
        bool l_found = false;
        l_node = _clone_graph(node, l_visited);
        return l_node;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    return 0;
}