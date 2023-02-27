//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include <algorithm>
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
//! types
//! ----------------------------------------------------------------------------
typedef std::vector<std::vector<int>> int_vec_vec_t;
typedef std::vector<int> int_vec_t;
//! ----------------------------------------------------------------------------
//! helper
//! ----------------------------------------------------------------------------
static inline void _print_vv(const int_vec_vec_t& a_vv)
{
    printf("[");
    for(auto && i_iv : a_vv) {
        printf("[");
        for (auto && i_v : i_iv) {
            printf("%d", i_v);
            if (i_v != i_iv.back())
            {
                printf(",");
            }
        }
        printf("]");
        if (i_iv != a_vv.back())
        {
            printf(",");
        }
    }
    printf("]");
}
//! ----------------------------------------------------------------------------
//! helper
//! ----------------------------------------------------------------------------
static inline void _print_v(const int_vec_t& a_vv)
{
    printf("[");
    for (auto && i_v : a_vv) {
        printf("%d", i_v);
        if (i_v != a_vv.back())
        {
            printf(",");
        }
    }
    printf("]");
}
//! ----------------------------------------------------------------------------
//! \class Solution
//! ----------------------------------------------------------------------------
class Solution {
public:
    typedef std::vector<std::vector<int>> int_vv_t;
    // -----------------------------------------------------
    // adding our own interval specific sort
    // -----------------------------------------------------    
    class iv_cmp {
    public:
        bool operator() (std::vector<int>& a_lhs, std::vector<int>& a_rhs) {
            return a_lhs[0] < a_rhs[0];
        };
    };
    static std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
        int_vv_t l_result;
        // -------------------------------------------------
        // sort intervals in place
        // -------------------------------------------------
        std::sort(intervals.begin(), intervals.end(), iv_cmp{});
        // -------------------------------------------------
        // walk sorted list
        // -------------------------------------------------
        for (auto && i_iv : intervals) {
            if (l_result.empty() ||
                (l_result.back()[1] < i_iv[0]))
            {
                l_result.push_back(i_iv);
            }
            else if(l_result.back()[1] < i_iv[1])
            {
                l_result.back()[1] = i_iv[1];
            }
        }
        return l_result;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    int_vec_vec_t l_result;
    int_vec_vec_t l_v;
#define _SHOW_RESULTS(_v, _r) do { \
    printf("+--- TEST INSERT ---+\n"); \
    printf(": ORIGINAL: "); \
    _print_vv(_v); \
    printf("\n"); \
    printf("+ --------->\n"); \
    printf(": RESULT:   "); \
    _print_vv(_r); \
    printf("\n"); \
    printf("+-------------------+\n\n"); \
} while(0)
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_v = {{1,3},{2,6},{8,10},{15,18}};
    l_result = Solution::merge(l_v);
    _SHOW_RESULTS(l_v, l_result);
    return 0;
}