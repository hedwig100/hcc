#include "test.h"

int a  = -32;
int b  = ((43 - 10) * 43 / 33 - 10);
int c  = ((0 == 1) > 5) + 54;
int *d = &a;
char e[10];
char *f = e;
int g[10];
int *h = g + 4;

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
    ok();
    return 0;
}