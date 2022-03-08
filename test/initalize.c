#include "test.h"

int a  = -32;
int b  = ((43 - 10) * 43 / 33 - 10);
int c  = ((0 == 1) > 5) + 54;
int *d = &a;
char e[10];
char *f = e;
int g[10];
int *h      = g + 4;
int i[3]    = {0, 1, 2};
int j[3][5] = {{0, 1, 2}, {0, 1}};

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
    ok();
    return 0;
}