//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <string>
//! ----------------------------------------------------------------------------
//! types
//! ----------------------------------------------------------------------------
typedef std::vector<std::vector<char>> board_t;
//! ----------------------------------------------------------------------------
//! ptrie
//! ----------------------------------------------------------------------------
#define _NUM_CHARS 26
struct node
{
        // constructor
        node(void):
                m_child(),
                m_terminal(false)
        {
        }
        ~node(void)
        {
                for (int i_c = 0; i_c < _NUM_CHARS; ++i_c)
                {
                        if (m_child[i_c])
                        {
                                delete m_child[i_c];
                                m_child[i_c] = nullptr;
                        }
                }
        }
        // members
        node *m_child[_NUM_CHARS];
        bool m_terminal;
};
typedef std::vector<std::string> str_vec_t;
class ptrie
{
public:
        // constructor
        ptrie(void):
                m_root()
        {
        }
        void init(const str_vec_t &a_vec);
        // members
        node m_root;
};
//! ----------------------------------------------------------------------------
//! init
//! ----------------------------------------------------------------------------
void ptrie::init(const str_vec_t &a_vec)
{
        // for each word
        for (auto &&i_w : a_vec)
        {
                node *l_cn = &m_root;
                for (auto &&i_c : i_w)
                {
                        unsigned int l_idx = i_c - 'a';
                        if (!l_cn->m_child[l_idx])
                        {
                                l_cn->m_child[l_idx] = new node();
                        }
                        l_cn = l_cn->m_child[l_idx];
                }
                l_cn->m_terminal = true;
        }
}
//! ----------------------------------------------------------------------------
//! \class Solution
//! ----------------------------------------------------------------------------
class Solution
{
public:
        typedef std::vector<std::vector<char>> char_vec_vec_t;
        // -------------------------------------------------
        // is inbounds
        // -------------------------------------------------
        static bool is_inbounds(int a_h,
                                int a_w,
                                int a_i,
                                int a_j)

        {
                if ((a_i < 0) ||
                    (a_i >= a_h) ||
                    (a_j < 0) ||
                    (a_j >= a_w))
                {
                        return false;
                }
                return true;
        }
        // -------------------------------------------------
        // dfs
        // -------------------------------------------------
        static void dfs(char_vec_vec_t &a_board,
                        int a_h,
                        int a_w,
                        int a_i,
                        int a_j,
                        node* a_cur_node,
                        std::string& ao_cur_word,
                        str_vec_t& ao_results)
        {
                if (!a_cur_node)
                {
                        return;
                }
                if(!is_inbounds(a_h, a_w, a_i, a_j))
                {
                        return;
                }
                // -----------------------------------------
                // get cur char
                // -----------------------------------------
                char l_c = a_board[a_i][a_j];
                if (l_c == '.')
                {
                        return;
                }
                // -----------------------------------------
                // get cur node
                // -----------------------------------------
                node* l_next_node = a_cur_node->m_child[l_c-'a'];
                if (!l_next_node)
                {
                        return;
                }
                //printf("DD [%d, %d]: %c %p\n", a_i, a_j, l_c, a_cur_node);
                ao_cur_word.push_back(l_c);
                if (l_next_node->m_terminal)
                {
                        // ---------------------------------
                        // remove from terminals to prevent
                        // finding duplicates
                        // ---------------------------------
                        l_next_node->m_terminal = false;
                        //printf("WW WORD: %s\n", ao_cur_word.c_str());
                        ao_results.push_back(ao_cur_word);
                }
                // -----------------------------------------
                // preserve char
                // -----------------------------------------
                a_board[a_i][a_j] = '.';
                // -----------------------------------------
                // neighbor up
                // -----------------------------------------
                dfs(a_board, a_h, a_w, a_i-1, a_j, l_next_node, ao_cur_word, ao_results);
                // -----------------------------------------
                // neighbor right
                // -----------------------------------------
                dfs(a_board, a_h, a_w, a_i, a_j+1, l_next_node, ao_cur_word, ao_results);
                // -----------------------------------------
                // neighbor down
                // -----------------------------------------
                dfs(a_board, a_h, a_w, a_i+1, a_j, l_next_node, ao_cur_word, ao_results);
                // -----------------------------------------
                // neighbor left
                // -----------------------------------------
                dfs(a_board, a_h, a_w, a_i, a_j-1, l_next_node, ao_cur_word, ao_results);
                // -----------------------------------------
                // pop back
                // -----------------------------------------
                ao_cur_word.pop_back();
                // -----------------------------------------
                // preserve char
                // -----------------------------------------
                a_board[a_i][a_j] = l_c;
                return;
        }
        static std::vector<std::string> findWords(std::vector<std::vector<char>> &board,
                                                  std::vector<std::string> &words)
        {
                str_vec_t l_res;
                // -----------------------------------------
                // init ptrie
                // -----------------------------------------
                ptrie l_ptrie;
                l_ptrie.init(words);
                // -----------------------------------------
                // iterate over board
                // -----------------------------------------
                int l_h = (int)board.size();
                int l_w = (int)board[0].size();
                //printf("Board spec %d / %d\n", l_w, l_h);
                for (int i_i = 0; i_i < l_h; ++i_i)
                {
                        for (int i_j = 0; i_j < l_w; ++i_j)
                        {
                                //printf(":: ======================================= \n");
                                //printf(":: [%d, %d]: %c %p\n", i_i, i_j, board[i_i][i_j], &(l_ptrie.m_root));
                                std::string l_word;
                                dfs(board, l_h, l_w, i_i, i_j, &(l_ptrie.m_root), l_word, l_res);
                        }
                }
                // -----------------------------------------
                // done...
                // -----------------------------------------
                return l_res;
        }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void)
{
        // -----------------------------------------------------
        // inputs
        // -----------------------------------------------------
        board_t l_board = {
                { 'o', 'a', 'a', 'n'},
                { 'e', 't', 'a', 'e'},
                { 'i', 'h', 'k', 'r'},
                { 'i', 'f', 'l', 'v'},
        };
        str_vec_t l_words = {
                "oath",
                "pea",
                "eat",
                "rain",
                "oathi",
                "oathk",
                "oathf",
                "oate",
                "oathii",
                "oathfi",
                "oathfii"
        };
        auto l_result = Solution::findWords(l_board, l_words);
        printf("Found %lu results: \n", l_result.size());
        for (auto &&i_r : l_result)
        {
                printf("%s, ", i_r.c_str());
        }
        printf("\n");
        return 0;
}
