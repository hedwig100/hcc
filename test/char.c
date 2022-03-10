#include "test.h"

int f(char x, char *y) {
    return x + *y;
}

char *z;

int main() {
    ASSERT(3, ({char a;a = 3; a; }));
    ASSERT(3, ({char x[3];x[0]=-1;x[1]=2;int y;y = 4; x[0]+y; }));
    ASSERT(4, ({char *x[3];char y;y = 4;x[0] = &y; *x[0]; }));
    ASSERT(0, ({char a;char *b;a = -4;b = &a; f(a,b) + 8; }));
    char *x;
    x = "aie";
    ASSERT(97, ({ x[0]; }));
    z = "oa";
    ASSERT(97, ({ z[1]; }));
    char *w[3];
    w[0] = "joga";
    ASSERT(97, ({ w[0][3]; }));

    char y = 'a';
    ASSERT(97, y);
    char abc = '4';
    ASSERT(52, abc);
    ok();
    return 0;
}
