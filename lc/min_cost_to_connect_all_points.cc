#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <set>
typedef std::vector<std::vector<int>> vv_int_t;
class Solution {
public:
    static int minCostConnectPoints(std::vector<std::vector<int>>& points) {
        if (!points.size()) {
            return 0;
        }
        int l_cost = 0;
        typedef std::pair<int, int> point_t;
        typedef struct {
            int m_cost;
            point_t m_point;
        } pc_t;
        class pc_compare {
        public:
                bool operator()(pc_t& t1, pc_t& t2) {
                        return (t1.m_cost > t2.m_cost);
                }
        };
        typedef std::priority_queue<pc_t, std::vector<pc_t>, pc_compare> min_heap_t;
        min_heap_t l_heap;
        std::set<point_t> l_visited;
        // push first point
        pc_t l_pc;
        l_pc.m_cost = 0;
        l_pc.m_point = {points[0][0], points[0][1]};
        l_heap.push(l_pc);
        while (l_visited.size() < points.size() &&
               !l_heap.empty()) {
            l_pc = l_heap.top();
            l_heap.pop();
            if (l_visited.find(l_pc.m_point) != l_visited.end()) {
                continue;
            }
            l_cost += l_pc.m_cost;
            l_visited.insert(l_pc.m_point);
            // build frontier not including visited
            for (auto && i_p : points) {
                // skip visited
                pc_t i_pc;
                i_pc.m_point = {i_p[0], i_p[1]};
                if (l_visited.find(i_pc.m_point) != l_visited.end()) {
                    continue;
                }
                i_pc.m_cost = abs(l_pc.m_point.first  - i_pc.m_point.first) +
                              abs(l_pc.m_point.second - i_pc.m_point.second);
                l_heap.push(i_pc);
            }
        }
        return l_cost;
    }
};
int main(void) {
    {
    vv_int_t l_pl = {{0, 0}, {2, 2}, {3, 10}, {5, 2}, {7, 0}};
    int l_c = Solution::minCostConnectPoints(l_pl);
    printf("min cost: %d\n", l_c);
    }
    return 0;
}
