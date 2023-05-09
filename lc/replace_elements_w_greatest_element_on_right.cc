#include <stdio.h>
#include <vector>

using namespace std;

#define _PRINT_VEC(_v) do { \
    printf("["); \
    for (auto& i_v : _v) { \
        printf("%d", i_v); \
        if (i_v != _v.back()) { printf(", "); }\
     } printf("]"); \
} while(0)

class Solution {
public:
  static vector<int> replaceElements(vector<int>& arr) {
    int rm = -1;
    int as = arr.size();
    for (int i = as-1; i >= 0; --i) {
      int nm = std::max(rm , arr[i]);
      printf("i[%d]: \n", i);
      arr[i] = rm;
      rm = nm;
    }
    arr.back() = -1;
    return arr;
  }
};

int main(void) {
  {
    vector<int> v = {17,18,5,4,6,1};
    auto res = Solution::replaceElements(v);
    _PRINT_VEC(res);    
  }
}

