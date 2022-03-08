#include "test.h"

int f(int a[5]) {
    return *a + *(a + 4);
}

int g(int *a[2]) {
    return **a + **(a + 1);
}

int h(int **a) {
    return **a + *(*a - 1);
}

int sum(int a[2][2]) {
    int ans;
    ans = 0;
    int i;
    int j;
    i = 0;
    j = 0;
    for (i = 0; i < 2; i = i + 1) {
        for (j = 0; j < 2; j = j + 1) {
            ans = ans + a[i][j];
        }
    }
    return ans;
}

int main() {
    ASSERT(0, ({int a[10];*a=0; *a; }));
    ASSERT(14, ({int a[3];*a = 4;*(a + 1) = 4;*(a + 2) = 6;int i;int ans;ans = 0; for (i = 0;i < 3;i=i+1){ans = ans + *(a + i);}  ans; }));
    ASSERT(3, ({int a[2];*a=1;*(a+1)=2;int *p;p = &a; *p+*(p+1); }));
    ASSERT(5, ({int b[5];*b = 10;*(b + 4) = -5; f(b); }));
    ASSERT(8, ({int *b[2];int c;c = 4;*b = &c;*(b + 1) = &c; g(b); }));
    ASSERT(1, ({int *b[2];int c;int d;c = 4;d = -3;*b = &c; h(b); }));
    ASSERT(4, ({int a[5][3]; *(*(a+1)+2) = 4;  *(*(a+1)+2); }));
    ASSERT(9, ({int a[5][3][4]; *(*(*(a+1)+2)+2) = 9;  *(*(*(a+1)+2)+2); }));
    ASSERT(8, ({int a[5]; a[1]=4;a[2]=4;  a[1]+a[2]; }));
    ASSERT(1, ({int a[5];*(a+1)=1; *(a+*(a+1)); }));
    ASSERT(1, ({int a[5]; a[1]=2;a[a[1]]=4;a[a[a[1]]]=1;  a[a[1]+2]; }));
    ASSERT(3, ({int *a[5];int b;b=3;a[b]=&b; *a[b]; }));
    ASSERT(4, ({int a[2][2][2][2];a[0][0][0][0]=4;int i;i = 0;int j;j = 0; a[i][i+j][j*i][(j*1)+1-1]; }));
    ASSERT(4, ({int a[2][2];int i;int j;i = 0;j = 0;for(i=0;i<2;i=i+1){for(j=0;j<2;j=j+1){a[i][j] = 1;}}sum(a); }));
    ok();
    return 0;
}
