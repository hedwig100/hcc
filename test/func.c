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

int param7(int a, int b, int c, int d, int e, int f, int g) {
    return a - b + c - d + e - f + g;
}

int param8(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a - b + c - d + e - f + g - h;
}

int varargs(int a, ...) {
    va_list ap;
    va_start(ap, a);
    va_end(ap);
    return a;
}

int A(int x, int y[10]) {
    return x + y[4];
}

int test_at(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    FILE *fp = fopen("test.txt", "w");
    if (!fp) return 1;
    vfprintf(fp, fmt, ap);
    va_end(ap);
    return 0;
}

char abcdefgh(int i) {
    return __func__[i];
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

    ASSERT(3, param7(0, 1, 2, 3, 4, 5, 6));
    ASSERT(-18, param7(-3, 2, -32, 10, 23, -3, 3));
    ASSERT(-4, param8(0, 1, 2, 3, 4, 5, 6, 7));
    ASSERT(-7, param8(2, 3, 5, 7, 11, 13, 17, 19));
    ASSERT(0, param8(1, -43, 2, 31, 53, 3, 20, 85));
    ASSERT(19, param8(fib(1), fib(2), fib(3), fib(1 + fib(1)), 10, 10, -3, -21));

    int x     = 10;
    int y[10] = {1, 2, 3, 4, 5, 5};
    ASSERT(15, A(x, y));

    // watch
    ASSERT(10, varargs(10, -3, -43, 4, 10, 3, -3));
    ASSERT(0, test_at("this is error. %d %d %d\n", 0, 1, 2));
    ASSERT(0, test_at("this is error. %d %d %d %d %d %d\n", 0, 1, 2, 3, 4, 5));
    ASSERT(97, abcdefgh(0));
    ASSERT(98, abcdefgh(1));
    ASSERT(99, abcdefgh(2));
    ok();
    return 0;
}
