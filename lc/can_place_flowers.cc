#include <stdio.h>
#include <vector>
using namespace std;
class Solution {
public:
    static bool canPlaceFlowers(vector<int>& flowerbed, int n) {
        // handle empty
        if (!n) {
            return true;
        }
        size_t s = flowerbed.size();
        if (!s) {
            return false;
        }
        // handle one
        else if (s == 1) {
            if (!flowerbed[0] && (n == 1)) {
                return true;
            }
            return false;
        }
        // handle two
        else if (s == 2) {
            if (!flowerbed[0] && !flowerbed[1] && (n == 1)) {
                return true;
            }
            return false;
        }
        // loop with boundary conditions from 1 to last-1
        for (size_t i = 0; i < s; ++i) {
            printf("i: %lu\n", i);
            // handle edges
            if (i == 0) {
                printf("left\n");
                printf("[%d] [%d]\n", flowerbed[i], flowerbed[i+1]);
                // treat border as a zero
                if (!flowerbed[i] && !flowerbed[i+1]) {
                    printf("can\n");
                    flowerbed[i] = 1;
                    n -= 1;
                }
            }
            else if (i == s-1) {
                printf("right\n");
                printf("[%d] [%d]\n", flowerbed[i-1], flowerbed[i]);
                // treat right side as zero
                if (!flowerbed[i-1] && !flowerbed[i]) {
                    printf("can\n");
                    flowerbed[i] = 1;
                    n -= 1;
                }
            }
            else {
                printf("[%d] [%d] [%d]\n", flowerbed[i-1], flowerbed[i], flowerbed[i+1]);
                if (!flowerbed[i-1] && !flowerbed[i] && !flowerbed[i+1]) {
                    printf("can\n");
                    flowerbed[i] = 1;
                    n -= 1;
                }
            }
            if (!n) {
                return true;
            }
        }
        return false;
    }
};

int main(void) {
    {
        vector<int> v = {0,0};
        int n = 2;
        bool b = Solution::canPlaceFlowers(v, n);
        printf("can place n: %d b: %d\n", n, b);
    }
}