#include "test.h"

int a = 0;
int b = ((43 - 10) * 43 / 33 - 10);
int c = ((0 == 1) > 5) + 54;

int main() {
    ASSERT(0, a);
    ASSERT(33, b);
    ASSERT(54, c);
    ok();
    return 0;
}