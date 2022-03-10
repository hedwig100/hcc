#include "test.h"

int f1(int a) {
    switch (a) {
    case 0:
        return a;
    case 1:
        a = 10;
        {
            int a;
            ++a;
        }
        break;
    case 2:
    case 3:
        return 2;
    default:
        return 100;
    }
    return -1; // a = 1
}

int f2(int a) {
    switch (a) {
        int b;
    case 0:
        b = 3;
        return 0;
    case 1:
        b = 4;
    case 2:
        switch (b) {
        case 4:
            return 1;
        default:
            return 2;
        }
    default:
        return 100;
    }
}

int f3(int a) {
    switch (a) {
        int b;
    case 0:
        if (b == 4) {
            break;
        }
        return 0;
    case 1:
        for (int i = 0; i < 4; ++i) {
            a = a + 1;
            if (i == 0) break;
        }
        break;
    case 2:
        while (a > 0)
            --a;
        return a + 2;
    default:
        break;
    }
    return a - 1;
}

int main() {
    ASSERT(0, ({int a;a = 0;if (3 == 4) a = 1;  a; }));
    ASSERT(71, ({int a;int b;a = 34;b = a + 3; if (b - a == 3) b = b*2; b-3; }));
    ASSERT(4, ({int a;int b;int c;a=343;b=34;if (a > b) c = 4; else c = 45; c; }));
    ASSERT(34, ({int a;int b;int c;a=34;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43; c; }));
    ASSERT(0, ({int a;int b;int c;a=40;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43;  c; }));
    ASSERT(43, ({int a;int b;int c;a=50;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43; c; }));
    ASSERT(10, ({int i;i=0;while (i<10) i=i+1; i; }));
    ASSERT(0, ({int i;i=0;while (i>0) i=i-1; i; }));
    ASSERT(66, ({int i;int c;c=2;for (i=0;i<32;i=i+1)c=c+2; c; }));
    ASSERT(64, ({int ze;int j;ze=1;for (j=10;j>=0;j=j-2)ze=ze*2; ze; }));
    ASSERT(8, ({int b;int c;int i;b=0;c=-4;for(i=0;i<4;i=i+1){b=b+1;c=c+2;}  b+c; }));
    ASSERT(0, ({int b;int c;int i;b=0;c=-4;i=50;while(i<54){i=i+1;b=b+1;c=c+2;}  b-c; }));
    ASSERT(100, ({int ans;int i;int j;ans=0;for(i=0;i<10;i=i+1){for(j=0;j<10;j=j+1){ans=ans+1;}} ans; }));
    ASSERT(24, ({int ans;int i;ans=1;i=0;while(i<30){if(i<10){ans=ans*2;}else{ans=ans-50;}i=i+1;} ans; }));
    ASSERT(3, ({int a1;int b_1;a1=0;b_1=3; a1+b_1; }));

    ASSERT(2, ({int ans = 0;for (int i = 0; i < 5; ++i) {if (ans == 2)break;ans = ans + 1;}ans; }));
    ASSERT(3, ({ int ans = 0;for (int i = 0;i < 4;++i){if (i == 0) continue; ans = ans + 1;}ans; }));
    ASSERT(3, ({ int ans = 0; int i = 0; while (i < 4){if (i == 3) break;++i;ans = ans + 1;}ans; }));
    ASSERT(5, ({ int ans = 0; int i = 5; while (i >= 0){if (--i == 3) continue; ans = ans + 1;}ans; }));
    ASSERT(5, ({int ans = 0;for (int i = 0; i < 4; ++i) {if (i == 0) continue;while (++i < 10) {ans = ans + i;if (ans == 5) break;}}ans; }));
    ASSERT(1920, ({int ans = 0;for (int i = 0; i < 50; ++i) {if (i == 5) continue;for (int j = 0; j < 10; ++j) {if (j / 4 == 2) break;ans = ans + i - 20;}}ans; }));

    ASSERT(0, f1(0));
    ASSERT(-1, f1(1));
    ASSERT(2, f1(2));
    ASSERT(2, f1(3));
    ASSERT(100, f1(4));
    ASSERT(100, f1(100));
    ASSERT(100, f1(-5));
    ASSERT(0, f2(0));
    ASSERT(1, f2(1));
    ASSERT(2, f2(2));
    ASSERT(100, f2(4));
    ASSERT(0, f3(0));
    ASSERT(1, f3(1));
    ASSERT(2, f3(2));
    ASSERT(3, f3(4));
    ASSERT(8, f3(9));

    int a = 10;
    switch (a)
        ++a;
    ASSERT(10, a);
    switch (a)
    default:
        ++a;
    ASSERT(11, a);
    switch (a)
    case 11:
        ++a;
    ASSERT(12, a);
    switch (a)
    case 1:
        ++a;
    ++a;
    ASSERT(13, a);

    ok();
    return 0;
}
