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
    struct A a;
    ok();
    return 0;
}