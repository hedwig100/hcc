#include "test.h"

int main() {
    int i = 0, j = 4;
    i += 4;
    ASSERT(4, i);
    i += j - 8;
    ASSERT(0, i);

    ok();
    return 0;
}