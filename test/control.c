#include "test.h"

int main() {
    int a;
    int b;
    int c;
    a = 343;
    b = 34;
    if (a > b)
        c = 4;
    else
        c = 45;
    ASSERT(4, c);
    a = 34;
    b = -35;
    if (a + b <= 0)
        c = 34;
    else if (a + b <= 10)
        c = 0;
    else
        c = 43;
    ASSERT(34, c);
    a = 40;
    b = -35;
    if (a + b <= 0)
        c = 34;
    else if (a + b <= 10)
        c = 0;
    else
        c = 43;
    ASSERT(0, c);
    a = 50;
    b = -35;
    if (a + b <= 0)
        c = 34;
    else if (a + b <= 10)
        c = 0;
    else
        c = 43;
    ASSERT(43, c);
    int i;
    i = 0;
    while (i < 10)
        i = i + 1;
    ASSERT(10, i);
    i = 0;
    while (i > 0)
        i = i - 1;
    ASSERT(0, i);
    c = 2;
    for (i = 0; i < 32; i = i + 1)
        c = c + 2;
    ASSERT(66, c);
    int ze;
    int j;
    ze = 1;
    for (j = 10; j >= 0; j = j - 2)
        ze = ze * 2;
    ASSERT(64, ze);
    b = 0;
    c = -4;
    for (i = 0; i < 4; i = i + 1) {
        b = b + 1;
        c = c + 2;
    }
    ASSERT(8, b + c);
    b = 0;
    c = -4;
    i = 50;
    while (i < 54) {
        i = i + 1;
        b = b + 1;
        c = c + 2;
    }
    ASSERT(0, b - c);
    int ans;
    ans = 0;
    for (i = 0; i < 10; i = i + 1) {
        for (j = 0; j < 10; j = j + 1) {
            ans = ans + 1;
        }
    }
    ASSERT(100, ans);
    ans = 1;
    i   = 0;
    while (i < 30) {
        if (i < 10) {
            ans = ans * 2;
        } else {
            ans = ans - 50;
        }
        i = i + 1;
    }
    ASSERT(24, ans);
    int a1;
    int b_1;
    a1  = 0;
    b_1 = 3;
    ASSERT(3, a1 + b_1);
    ok();
    return 0;
}