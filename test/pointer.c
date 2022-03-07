#include "test.h"

int f(int *a) {
    int c;
    c = *a;
    return c;
}

int main() {
    int a;
    int *b;
    int c;
    a = 3;
    b = &a;
    c = *b;
    ASSERT(7, c + 4);
    a = 10;
    b = &a;
    c = *b;
    ASSERT(6, c - 4);
    a = 10;
    b = &a;
    ASSERT(10, *b);
    b  = &a;
    *b = 5;
    ASSERT(5, a);
    int **d;
    b   = &a;
    d   = &b;
    **d = 10;
    *b  = *b + 5;
    ASSERT(15, a);
    a = 10;
    b = &a;
    ASSERT(10, f(b));
    int x;
    int y;
    int z;
    int *w;
    w = &x;
    w = w - 2;
    z = 5;
    ASSERT(5, *w);
    int a1;
    int b1;
    int c1;
    int d1;
    int *e1;
    e1 = &d1;
    e1 = e1 + 3;
    a1 = 20;
    ASSERT(20, *e1);
    int a2;
    int b2;
    int c2;
    int d2;
    int *e2;
    e2  = &d2;
    e2  = e2 + 2;
    b2  = 10;
    *e2 = *e2 + 10;
    ASSERT(20, b2);

    ok();
    return 0;
}
