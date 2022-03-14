#include "test.h"

int f(int *a) {
    int c;
    c = *a;
    return c;
}

int main() {
    ASSERT(7, ({int a;a = 3;int *b;b = &a;int c;c = *b; c + 4; }));
    ASSERT(6, ({int a;int *b;int c;a = 10;b = &a;c = *b; c - 4; }));
    ASSERT(10, ({int a;int *b;a = 10;b = &a; *b; }));
    ASSERT(5, ({int a;int *b;b = &a;*b = 5; a; }));
    ASSERT(15, ({int a;int *b;int **c;b = &a;c = &b;**c = 10;*b = *b + 5; a; }));
    ASSERT(10, ({int a;int *b;a = 10;b = &a; f(b); }));
    ASSERT(5, ({int a;int b;int c;int *d;d = &a;d = d - 2;c = 5;*d; }));
    ASSERT(20, ({int a;int b;int c;int d;int *e;e = &d;e = e + 3;a = 20; *e; }));
    ASSERT(20, ({int a;int b;int c;int d;int *e;e = &d;e = e + 2;b = 10;*e = *e + 10; b; }));

    char *x = "AIEIE";
    char *y;
    y = x + 3;
    ASSERT(3, y - x);
    ok();
    return 0;
}