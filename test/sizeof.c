#include "test.h"

int main() {
    ASSERT(4, ({int a; sizeof(a); }));
    ASSERT(4, ({int a; sizeof a; }));
    ASSERT(8, ({int *a; sizeof(a); }));
    ASSERT(13, ({int *a; sizeof a +5; }));
    ASSERT(4, ({int *a; sizeof(*a+4); }));
    ASSERT(4, ({ sizeof(-130); }));
    ASSERT(4, ({int *a; sizeof sizeof(a) ; }));
    ASSERT(40, ({int a[10]; sizeof(a) ; }));
    ASSERT(160, ({int *a[20]; sizeof a ; }));
    ok();
    return 0;
}