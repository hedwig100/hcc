#include "test.h"

struct A {
    int a;
    char b;
    int c[10];
    int *d;
    char *e[4];
    int f[2][3];
};

int main() {
    struct A x;
    x.a = 10;
    ASSERT(10, x.a);
    x.b = 97;
    ASSERT(97, x.b);
    x.c[2] = 3;
    ASSERT(3, x.c[2]);
    int y = 10;
    x.d   = &y;
    ASSERT(10, *(x.d));
    char z = 2;
    x.e[3] = &z;
    ASSERT(2, *(x.e[3]));
    x.f[3][1] = 43;
    ASSERT(43, x.f[3][1]);

    struct A *abc = &x;
    (*abc).a      = 3;
    ASSERT(3, (*abc).a);
    (*abc).b = x.a + x.b;
    ASSERT(100, (*abc).b);
    (*abc).c[0] = 7;
    ASSERT(7, (*abc).c[0]);
    (*abc).d = &((*abc).a);
    ASSERT(3, *((*abc).d));
    (*abc).e[3] = &z;
    ASSERT(2, *((*abc).e[3]));
    (*abc).f[0][1] = -34;
    ASSERT(-34, (*abc).f[0][1]);
    ok();
    return 0;
}