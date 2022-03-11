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

    ok();
    return 0;
}