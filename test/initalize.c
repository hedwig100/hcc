#include "test.h"

int a  = -32;
int b  = ((43 - 10) * 43 / 33 - 10);
int c  = ((0 == 1) > 5) + 54;
int *d = &a;
char e[10];
char *f = e;
int g[10];
int *h         = g + 4;
int i[3]       = {0, 1, 2};
int j[3][5]    = {{0, 1, 2}, {0, 1}};
char str[5]    = "abcd";
char dou[3][4] = {"abc", "de", "f"};
char *fae      = "abcd";

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    ASSERT(-32, a);
    ASSERT(33, b);
    ASSERT(54, c);
    ASSERT(1, *d == a);
    // ASSERT(1, d == &a); this doesn't work though code below works.
    *d = 5;
    ASSERT(5, a);
    a = 10;
    ASSERT(10, *d);
    ASSERT(1, f == e);
    ASSERT(10, ({ g[4] = 10; *h; }));
    ASSERT(0, i[0]);
    ASSERT(1, i[1]);
    ASSERT(2, i[2]);

    ASSERT(0, j[0][0]);
    ASSERT(1, j[0][1]);
    ASSERT(2, j[0][2]);
    ASSERT(0, j[0][3]);
    ASSERT(0, j[0][4]);

    ASSERT(0, j[1][0]);
    ASSERT(1, j[1][1]);
    ASSERT(0, j[1][2]);
    ASSERT(0, j[1][3]);
    ASSERT(0, j[1][4]);

    ASSERT(0, j[2][0]);
    ASSERT(0, j[2][1]);
    ASSERT(0, j[2][2]);
    ASSERT(0, j[2][3]);
    ASSERT(0, j[2][4]);

    ASSERT(97, str[0]);
    ASSERT(98, str[1]);
    ASSERT(99, str[2]);
    ASSERT(100, str[3]);
    ASSERT(0, str[4]);

    ASSERT(97, dou[0][0]);
    ASSERT(98, dou[0][1]);
    ASSERT(99, dou[0][2]);
    ASSERT(0, dou[0][3]);
    ASSERT(100, dou[1][0]);
    ASSERT(101, dou[1][1]);
    ASSERT(0, dou[1][2]);
    ASSERT(0, dou[1][3]);
    ASSERT(102, dou[2][0]);
    ASSERT(0, dou[2][1]);
    ASSERT(0, dou[2][2]);
    ASSERT(0, dou[2][3]);

    int a   = -43;
    char b  = 2;
    char *c = "abc";
    int d   = a + i[1];
    ASSERT(-43, a);
    ASSERT(2, b);
    ASSERT(97, c[0]);
    ASSERT(98, c[1]);
    ASSERT(99, c[2]);
    ASSERT(0, c[3]);
    ASSERT(-42, d);

    int e[4] = {-3, fib(10), a, (b + 5) * (-1), j[0][1]};
    ASSERT(-3, e[0]);
    ASSERT(55, e[1]);
    ASSERT(-43, e[2]);
    ASSERT(-7, e[3]);
    ASSERT(1, e[4]);

    char *f[2] = {c, "ef"};
    ASSERT(97, f[0][0]);
    ASSERT(98, f[0][1]);
    ASSERT(99, f[0][2]);
    ASSERT(0, f[0][3]);
    ASSERT(101, f[1][0]);
    ASSERT(102, f[1][1]);
    ASSERT(0, f[1][2]);

    int g[3][4] = {{0, 1, fib(10)}, {97, 4}, {5}};
    ASSERT(0, g[0][0]);
    ASSERT(1, g[0][1]);
    ASSERT(55, g[0][2]);
    ASSERT(97, g[1][0]);
    ASSERT(4, g[1][1]);
    ASSERT(5, g[2][0]);

    ASSERT(97, fae[0]);
    ASSERT(98, fae[1]);
    ASSERT(99, fae[2]);
    ASSERT(100, fae[3]);
    ASSERT(0, fae[4]);

    int xa = 0, *xb = &xa, xc[3] = {0, 1, 2};
    ASSERT(0, xa);
    *xb = 4;
    ASSERT(4, xa);
    ASSERT(0, xc[0]);
    ASSERT(1, xc[1]);
    ASSERT(2, xc[2]);

    ok();
    return 0;
}