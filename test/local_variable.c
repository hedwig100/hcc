#include "test.h"

int main() {
    ASSERT(9, ({int a;a=10;a-1; }));
    ASSERT(2, ({int b;b=-14;b+16; }));
    ASSERT(24, ({int c;c=-6;c*(-4); }));
    ASSERT(3, ({int d;d=(3 < 5); d+2; }));
    ASSERT(4, ({int e;e = -134;e + 138; }));
    ASSERT(15, ({int f;f=(10 - 5*3); f+20; }));
    ASSERT(17, ({int g;int h;int i;int j;g=1;h=1;i=12;j=-15; i+5; }));
    ASSERT(4, ({int k;int l;int m;int n;k=1;l=1;m=12;n=-15; n+19; }));
    ASSERT(21, ({int o;int p;int q;int r;o=16/1;p=(1-3)*8;q=12;r=-15; o+5; }));
    ASSERT(6, ({int s;int t;int u;int v;s=1;t=1;u=(10-3)*3*2;v=-15;u/7; }));
    ASSERT(3, ({int u;int v;int w;u=1;v=1;w=12;w-9; }));
    ASSERT(9, ({int x;int y;int z;x=1;y=1;z=12; x+8; }));
    ASSERT(20, ({int a;int b;int c;a=26;b=a-46;c=b/2; c+30; }));
    ASSERT(0, ({int d;int e;int f;int g;int h;d=-2;e=d*45;f=e*3;g=f+e;h=g+d; h+362; }));
    ASSERT(8, ({int x;int y;int z;x=2;y=x+3;z=y*2;z-2; }));
    ASSERT(7, ({int hoge;int fuga;hoge=3;fuga=hoge*3; fuga-2; }));
    ASSERT(12, ({int abc;int ABC;int CC;abc=398;ABC=-352;CC=abc+ABC; CC-34; }));
    ASSERT(10, ({int pwoef;int goiao;int garhi;pwoef=34;goiao=-354+pwoef;garhi=goiao/32; garhi+20; }));
    ASSERT(100, ({int ieii;int ge;int A;ieii = 143;ge = -43;A = ieii + ge;A; }));
    ASSERT(3, ({int a1;int b_1;a1=0;b_1=3; a1+b_1; }));
    ASSERT(20, ({ int _a1 = 0; char *__ok;char __ng = 20;__ng; }));
    ok();
    return 0;
}
