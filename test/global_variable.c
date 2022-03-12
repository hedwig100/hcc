#include "test.h"

int a;
int *b;
int c[10];
int *d[2][2];
int *e[10];

int f() {
    a = 4;
    return 0;
}

int counter() {
    static int i = 0;
    return i++;
}

int main() {
    ASSERT(3, ({ a = 3; a; }));
    ASSERT(4, ({int c;b = &c;*b = 4;c; }));
    ASSERT(3, ({c[3]=3;c[3]; }));
    ASSERT(2, ({int b;int c;c = 2;e[3] = &b; *(e[3] - 1); }));
    ASSERT(4, ({f(); a; }));

    ASSERT(0, counter());
    ASSERT(1, counter());
    ASSERT(2, counter());
    ASSERT(3, counter());

    ok();
    return 0;
}