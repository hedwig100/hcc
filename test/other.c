#include "test.h"

/*
    block comment
*/

// line comment

int a;
int main() {
    ASSERT(0, ({int a;a=0;{int a;a=3;a=a+1;} a; }));
    ASSERT(4, ({a=0;int a;a=4;{int a;a=6;} a; }));
    ASSERT(0, ({int i;i = 0;int ans;ans = 0;for (i=0;i<10;i=i+1){int ans;ans = 4;ans = ans+1;} ans; }));
    ASSERT(0, ({ ({int i;i = 0;i; }); }));
    ASSERT(25, ({int a;a=20;int b;b = ({int a;a=10;a-5;}); b+a; }));
    ASSERT(20, ({int b;b=({int ans;ans=0;int i;for(i=0;i<5;i=i+1){ans=ans+i;} ans;}); b*2; }));
    ok();
    return 0;
}