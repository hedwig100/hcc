#include "test.h"

int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    ASSERT(0, 0);
    ASSERT(42, 42);
    ASSERT(21, 5 + 20 - 4);
    ASSERT(24, 4 - 10 + 30);
    ASSERT(11, 5 + 2 * 3);
    ASSERT(15, 5 * (9 - 6));
    ASSERT(33, (5 - 2) * 10 + 9 / (4 - 1));
    ASSERT(7, -3 + 10);
    ASSERT(4, -3 * (-5 + 9) / (-3));
    ASSERT(0, 0 == 1);
    ASSERT(1, 23 == 23);
    ASSERT(0, 25 == 23);
    ASSERT(1, 25 != 23);
    ASSERT(0, 23 != 23);
    ASSERT(1, 20 < 23);
    ASSERT(0, 1 < 1);
    ASSERT(0, 6 < 1);
    ASSERT(1, 6 <= 9);
    ASSERT(0, 10 <= 9);
    ASSERT(1, 10 > 9);
    ASSERT(0, 9 > 9);
    ASSERT(0, 10 > 17);
    ASSERT(1, 10 >= 9);
    ASSERT(1, 9 >= 9);
    ASSERT(0, 8 >= 9);
    ASSERT(1, (1 == 2) <= 1);
    ASSERT(0, (5 >= 3) != 1);
    ASSERT(1, (-6 < 10) == 1);

    int i = 0;
    ASSERT(1, ++i);
    ASSERT(0, --i);
    i = 9;
    ASSERT(55, fib(++i));

    ok();
    return 0;
}