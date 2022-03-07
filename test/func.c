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

int main() {
    ASSERT(55, fib(10));
    ASSERT(21, fib(fib(6)));
    ASSERT(55, fib(fib(fib(5)) + fib(5)));
    ASSERT(53, param2(10, -43));
    ASSERT(85, param3(10, -43, 32));
    ASSERT(185, param4(10, -43, 32, -100));
    ASSERT(184, param5(10, -43, 32, -100, -1));
    ASSERT(0, param6(10, -43, 32, -100, -1, 184));
    int a;
    a = 0;
    {
        int b;
        b = 4;
        a = a + 1;
    }
    ASSERT(1, a);
    {}
    a = 0;
    ASSERT(1, a + 1);
    ok();
    return 0;
}
