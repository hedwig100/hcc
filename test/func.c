#include "test.h"

int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int param2(int a, int b) {
    return a - b;
}

int param3(int a, int b, int c) {
    return a - b + c;
}

int param4(int a, int b, int c, int d) {
    return a - b + c - d;
}

int param5(int a, int b, int c, int d, int e) {
    return a - b + c - d + e;
}

int param6(int a, int b, int c, int d, int e, int f) {
    return a - b + c - d + e - f;
}

void f(int a) {
    a = a * a;
    return;
}

int main() {
    ASSERT(55, fib(10));
    ASSERT(21, fib(fib(6)));
    ASSERT(55, fib(fib(fib(5)) + fib(5)));
    ASSERT(53, param2(10, -43));
    ASSERT(85, param3(10, -43, 32));
    ASSERT(185, param4(10, -43, 32, -100));
    ASSERT(184, param5(10, -43, 32, -100, -1));
    ASSERT(0, param6(10, -43, 32, -100, -1, 184));
    ASSERT(1, ({int a;a = 0;{int b;b = 4;a = a + 1;}a; }));
    ASSERT(1, ({{}int a;a = 0;a + 1; }));
    f(3);
    void *a;

    // watch
    ASSERT(-4, print8(0, 1, 2, 3, 4, 5, 6, 7));
    ASSERT(-7, print8(2, 3, 5, 7, 11, 13, 17, 19));
    ASSERT(0, print8(1, -43, 2, 31, 53, 3, 20, 85));
    ASSERT(19, print8(fib(1), fib(2), fib(3), fib(1 + fib(1)), 10, 10, -3, -21));
    printf("%d %d %d %d %d %d %d %d\n", 1, 2, 3, 4, 5, 6, 7, 8);
    ok();
    return 0;
}
