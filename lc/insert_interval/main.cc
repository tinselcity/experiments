//! ----------------------------------------------------------------------------
//! includes
//! ----------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
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
    typedef std::vector<std::vector<int>> int_vec_vec_t;
    typedef std::vector<int> int_vec_t;

    static std::vector<std::vector<int>> insert(const std::vector<std::vector<int>>& intervals,
                                                const std::vector<int>& newInterval) {
        int_vec_vec_t l_result;
        int_vec_t l_ni = newInterval;
        // -------------------------------------------------
        // intervals is empty
        // -------------------------------------------------
        if (intervals.empty())
        {
            l_result.push_back(l_ni);
            return l_result;
        }
        // -------------------------------------------------
        // new interval is empty
        // -------------------------------------------------
        if (l_ni.empty())
        {
            l_result = intervals;
            return l_result;
        }
        // -------------------------------------------------
        // doesn't intersect with first
        // -------------------------------------------------
        if (l_ni[1] < intervals.front()[0])
        {
            l_result.push_back(l_ni);
            for(auto && i_iv : intervals) {
                //NDBG_PRINT("PUSH\n");
                l_result.push_back(i_iv);
            }
            return l_result;
        }
        // -------------------------------------------------
        // doesn't intersect with last
        // -------------------------------------------------
        if (l_ni[0] > intervals.back()[1])
        {
            for(auto && i_iv : intervals) {
                //NDBG_PRINT("PUSH\n");
                l_result.push_back(i_iv);
            }
            //NDBG_PRINT("PUSH\n");
            l_result.push_back(l_ni);
            return l_result;
        }
        size_t l_cur_iv = 0;
        // -------------------------------------------------
        // walk until intersection
        // -------------------------------------------------
        //NDBG_PRINT("cur[%lu]: [%d, %d]\n", l_cur_iv, intervals[l_cur_iv][0], intervals[l_cur_iv][1]);
        size_t l_ivs_size = intervals.size();
        while((l_cur_iv < l_ivs_size) &&
              (l_ni[0] > intervals[l_cur_iv][1])) {
            //NDBG_PRINT("PUSH\n");
            l_result.push_back(intervals[l_cur_iv]);
            ++l_cur_iv;
        }
        if (l_cur_iv >= l_ivs_size) {
            //NDBG_PRINT("done\n");
            return l_result;
        }
        // -------------------------------------------------
        // if front less than extend newInterval
        // -------------------------------------------------
        //NDBG_PRINT("cur[%lu]: [%d, %d]\n", l_cur_iv, intervals[l_cur_iv][0], intervals[l_cur_iv][1]);
        if (l_ni[0] > intervals[l_cur_iv][0]) {
            l_ni[0] = intervals[l_cur_iv][0];
        }
        // -------------------------------------------------
        // skip intervals
        // -------------------------------------------------
        //NDBG_PRINT("cur[%lu]: [%d, %d]\n", l_cur_iv, intervals[l_cur_iv][0], intervals[l_cur_iv][1]);
        while((l_cur_iv < l_ivs_size) &&
              (l_ni[1] >= intervals[l_cur_iv][0])) {
            ++l_cur_iv;
        }
        if (l_ni[1] < intervals[l_cur_iv-1][1])
        {
            l_ni[1] = intervals[l_cur_iv-1][1];
        }
        l_result.push_back(l_ni);
        if (l_cur_iv >= l_ivs_size) {
            //NDBG_PRINT("done\n");
            return l_result;
        }
        // -------------------------------------------------
        // add remainder
        // -------------------------------------------------
        //NDBG_PRINT("cur[%lu]: [%d, %d]\n", l_cur_iv, intervals[l_cur_iv][0], intervals[l_cur_iv][1]);
        while((l_cur_iv < l_ivs_size)) {
            l_result.push_back(intervals[l_cur_iv]);
            ++l_cur_iv;
        }
        //NDBG_PRINT("done\n");
        return l_result;
    }
};
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(void) {
    int_vec_vec_t l_result;
    int_vec_vec_t l_vv;
    int_vec_t l_v;
#define _SHOW_RESULTS(_vv, _v, _r) do { \
    printf("+--- TEST INSERT ---+\n"); \
    printf(": ORIG: "); \
    _print_vv(_vv); \
    printf("\n"); \
    printf(": INST: "); \
    _print_v(_v); \
    printf("\n"); \
    printf("+ --------->\n"); \
    printf(": RSLT: "); \
    _print_vv(_r); \
    printf("\n"); \
    printf("+-------------------+\n\n"); \
} while(0)
#if 1
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {};
    l_v = {2,5};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,3}, {6,9}};
    l_v = {};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,3}, {6,9}};
    l_v = {10,15};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{3,5}, {6,9}};
    l_v = {1,2};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,5}};
    l_v = {1,7};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,3}, {6,9}};
    l_v = {2,5};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,2},{3,5},{6,7},{8,10},{12,16}};
    l_v = {4, 8};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,5}};
    l_v = {2,3};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
#endif
    // -----------------------------------------------------
    // TEST
    // -----------------------------------------------------
    l_vv = {{1,5}};
    l_v = {5,7};
    l_result = Solution::insert(l_vv, l_v);
    _SHOW_RESULTS(l_vv, l_v, l_result);
    return 0;
}