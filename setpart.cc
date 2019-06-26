#include <iostream>
using namespace std;

constexpr int N = 10;
typedef int partition[N+1];

void print_partition(const partition& p) {
    for (int i=1;i <= N;++i) {
        cout << p[i] << " ";
    }
    cout <<endl;
}

void part1() {
    partition c,g;
    c[0] = 0;
    int r = 0,j;
    int nl = N-1;
    g[0] = 0;
    do  {
        while (r < nl) {
            r++;
            c[r] = 1;
            g[r] = g[r-1];
        }
        for (j = 1; j <= g[nl] + 1; ++j) {
            c[N] = j;
            print_partition(c);
        }
        while (c[r] > g[r-1]) {
            r--;
        }
        c[r] = c[r]+1;
        if (c[r] > g[r]) {
            g[r] = c[r];
        }

    } while (r != 1);
}

int main() {
    part1();
    // your code goes here
    return 0;
}
