#include "test.h"

struct A {
    int a;
    char b;
    int c[10];
    int *d;
    char *e[4];
    int f[2][3];
};

struct B {
    int x;
    char y;
};

struct C {
    struct D {
        int a;
        int b;
    } ab;
    int c;
    int d;
};

struct A global_a;

struct Name {
    char *name;
    int len;
};

struct Name *name1;

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

    struct B xB;
    struct B yB;
    xB.x = 3;
    xB.y = 4;
    yB   = xB;
    ASSERT(3, yB.x);
    ASSERT(4, yB.y);
    yB.x = 5;
    yB.y = 6;
    ASSERT(3, xB.x);
    ASSERT(4, xB.y);
    ASSERT(5, yB.x);
    ASSERT(6, yB.y);

    struct C c;
    c.ab.a = 4;
    c.ab.b = 32;
    c.c    = -4;
    c.d    = 43;
    ASSERT(4, c.ab.a);
    ASSERT(32, c.ab.b);
    ASSERT(-4, c.c);
    ASSERT(43, c.d);

    global_a.a    = -43;
    global_a.d    = &y;
    *(global_a.d) = 4;
    global_a.c[4] = -42;
    ASSERT(-43, global_a.a);
    ASSERT(4, y);
    ASSERT(-42, global_a.c[4]);

    struct Name name2;
    name2.name = "abcde";
    name2.len  = 6;
    name1      = &name2;
    ASSERT(97, name1->name[0]);
    ASSERT(98, name1->name[1]);
    ASSERT(99, name1->name[2]);
    ASSERT(100, name1->name[3]);
    ASSERT(101, name1->name[4]);
    ASSERT(0, name1->name[5]);
    name1->len = 7;
    ASSERT(7, name2.len);
    struct Name *names[10];
    names[0]       = &name2;
    names[0]->name = "fgh";
    ASSERT(102, name2.name[0]);
    ASSERT(103, name2.name[1]);
    ASSERT(104, name2.name[2]);
    ASSERT(0, name2.name[3]);
    ASSERT(102, name1->name[0]);
    ASSERT(103, name1->name[1]);
    ASSERT(104, name1->name[2]);
    ASSERT(0, name1->name[3]);

    struct A {
        int abc;
    };
    struct A xyz;
    xyz.abc = 4;
    ASSERT(4, xyz.abc);

    ok();
    return 0;
}