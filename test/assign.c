#include "test.h"

int main() {
    int i = 0, j = 4;
    i += 4;
    ASSERT(4, i);
    i += j - 8;
    ASSERT(0, i);

    int k = 0, l = -43;
    k -= l;
    ASSERT(43, k);
    l -= (i + 4) * (l + 44);
    ASSERT(-47, l);

    int m = 1, n = -27;
    m *= (n + 30);
    ASSERT(3, m);
    n *= (m - 3);
    ASSERT(0, n);

    ok();
    return 0;
}