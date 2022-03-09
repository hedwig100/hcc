#include "test.h"

struct A {
    int a;  // -> 4
    char b; // -> 1
};

struct B {
    int *a;     // -> 8
    struct A b; // -> 5
    char c[6];  // -> 6
};

struct C {
    int *a; // -> 8
    char b; // -> 1 (+ 3 for alignment)
    int c;  // -> 4
}

main() {
    ASSERT(4, ({int a; sizeof(a); }));
    ASSERT(4, ({int a; sizeof a; }));
    ASSERT(8, ({int *a; sizeof(a); }));
    ASSERT(13, ({int *a; sizeof a +5; }));
    ASSERT(4, ({int *a; sizeof(*a+4); }));
    ASSERT(4, ({ sizeof(-130); }));
    ASSERT(4, ({int *a; sizeof sizeof(a) ; }));
    ASSERT(40, ({int a[10]; sizeof(a) ; }));
    ASSERT(160, ({int *a[20]; sizeof a ; }));

    ASSERT(4, sizeof(int));
    ASSERT(1, sizeof(char));
    ASSERT(5, sizeof(struct A));
    ASSERT(19, sizeof(struct B));
    ASSERT(16, sizeof(struct C));

    ok();
    return 0;
}