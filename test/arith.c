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
    ASSERT(1, 10 % 3);
    ASSERT(3, (10 - 3) % 3 + 2);
    ASSERT(-1, (9 - 10) + 43 % 4 - 10 / 3);
    ASSERT(-46, ((44 % 43 - 4 % 9)) - 43);
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
    ASSERT(0, 2 & 1);
    ASSERT(4, (12 & 8 - 4) & 4);
    ASSERT(8, 10 & 8 * 1 & (20 - 6));
    ASSERT(9, 10 ^ 43 - 4 + 4 & 3);
    ASSERT(11, 10 ^ (13 ^ -43) + 43 & 43 % 3);
    ASSERT(15, 1 | 2 | 4 | 8);
    ASSERT(3, 1 | 43 & 43 - 43 * (1 - 1 ^ 43 | 4));
    ASSERT(1, 43 / (10 | 43 ^ 3 & 1));
    ASSERT(0, 0 && 1);
    ASSERT(1, -43 && 431);
    ASSERT(0, 4 - 3 && 0);
    ASSERT(1, 1 && 43 & 13 - 1 ^ 40 | 91);
    ASSERT(1, (43 - 32 ^ (10 && 3 - 3 / 4 - 3) && 3 - 10));
    ASSERT(0, 0 || 0);
    ASSERT(1, 412 || 0);
    ASSERT(1, 0 || -1);
    ASSERT(1, 41 || -43);
    ASSERT(1, 10 && 43 || 43 - 43 ^ 3 & 4 | 3);
    ASSERT(-32, 1 != 0 ? -32 : 10);
    ASSERT(3, 0 ? 13 - 43 : 1 ^ 43 & 2);
    ASSERT(1, 'a' ? 1 - 0 : 76);

    int i = 0;
    ASSERT(1, ++i);
    ASSERT(0, --i);
    i = 9;
    ASSERT(55, fib(++i));
    ASSERT(55, fib(i++));
    int a[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    ASSERT(11, a[i--]);
    ASSERT(10, a[i++]);
    ASSERT(12, a[++i]);

    ok();
    return 0;
}