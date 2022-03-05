#!/bin/bash 
assert() {
    expected="$1" 
    input="$2" 

    ./hcc "$input" > tmp.s 
    cc -o tmp tmp.s 
    ./tmp 
    actual="$?" 

    if [ "$actual" = "$expected" ]; then 
        echo "$input => $actual" 
    else
        echo "$input => $expected expected, but got $actual" 
        exit 1
    fi 
}

assert 0 "main() {return 0;}" 
assert 42 "main() {return 42;}" 
assert 21 "main() {return 5+20-4;}"
assert 24 "main() {return 4 - 10 + 30;}"
assert 11 "main() {return 5+2*3;}"
assert 15 'main() {return 5*(9-6);}'
assert 33 "main() {return (5 - 2) * 10 + 9 / (4 - 1);}"
assert 7 "main() {return -3+10;}" 
assert 4 "main() {return -3*(-5+9)/(-3);}"

assert 0 "main() {return 0==1;}" 
assert 1 "main() {return 23==23;}"
assert 0 "main() {return 25==23;}"
assert 1 "main() {return 25!=23;}"
assert 0 "main() {return 23!=23;}"
assert 1 "main() {return 20<23;}"
assert 0 "main() {return 1<1;}"
assert 0 "main() {return 6<1;}"
assert 1 "main() {return 6<=9;}"
assert 0 "main() {return 10<=9;}"
assert 1 "main() {return 10>9;}"
assert 0 "main() {return 9>9;}"
assert 0 "main() {return 10>17;}"
assert 1 "main() {return 10>=9;}"
assert 1 "main() {return 9>=9;}"
assert 0 "main() {return 8>=9;}"
assert 1 "main() {return (1 == 2) <= 1;}"
assert 0 "main() {return (5 >= 3) != 1;}"
assert 1 "main() {return (-6 < 10)==1;}"
assert 1 "main() {return (-6 < 10)==1;}"

assert 9 "main() {a=10;return a-1;}"
assert 2 "main() {b=-14;return b+16;}"
assert 24 "main() {c=-6;return c*(-4);}"
assert 3 "main() {d=(3 < 5);return d+2;}"
assert 4 "main() {e=-134;return e+138;}"
assert 15 "main() {f=(10 - 5*3);return f+20;}"
assert 17 "main() {g=1;h=1;i=12;j=-15;return i+5;}"
assert 4 "main() {k=1;l=1;m=12;n=-15;return n+19;}"
assert 21 "main() {o=16/1;p=(1-3)*8;q=12;r=-15;return o+5;}"
assert 6 "main() {s=1;t=1;u=(10-3)*3*2;v=-15;return u/7;}"
assert 3 "main() {u=1;v=1;w=12;return w-9;}"
assert 9 "main() {x=1;y=1;z=12;return x+8;}"
assert 20 "main() {a=26;b=a-46;c=b/2;return c+30;}"
assert 0 "main() {d=-2;e=d*45;f=e*3;g=f+e;h=g+d;return h+362;}"
assert 8 "main() {x=2;y=x+3;z=y*2;return z-2;}"
assert 7 "main() {hoge=3;fuga=hoge*3;return fuga-2;}"
assert 12 "main() {abc=398;ABC=-352;CC=abc+ABC;return CC-34;}"
assert 10 "main() {pwoef=34;goiao=-354+pwoef;garhi=goiao/32;return garhi+20;}"
assert 100 "main() {ieii=143;ge=-43;A=ieii+ge;return A;}"
assert 0 "main() {a = 0;if (3 == 4) a = 1; return a;}"
assert 71 "main() {a = 34;b = a + 3; if (b - a == 3) b = b*2;return b-3;}"

assert 4 "main() {a=343;b=34;if (a > b) c = 4; else c = 45; return c;}"
assert 34 "main() {a=34;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43; return c;}"
assert 0 "main() {a=40;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43; return c;}"
assert 43 "main() {a=50;b=-35;if (a + b <= 0) c = 34; else if (a + b <= 10) c = 0; else c = 43; return c;}"
assert 10 "main() {i=0;while (i<10) i=i+1;return i;}"
assert 0 "main() {i=0;while (i>0) i=i-1;return i;}"
assert 66 "main() {c=2;for (i=0;i<32;i=i+1)c=c+2;return c;}"
assert 64 "main() {ze=1;for (j=10;j>=0;j=j-2)ze=ze*2;return ze;}"
assert 8 "main() {b=0;c=-4;for(i=0;i<4;i=i+1){b=b+1;c=c+2;} return b+c;}"
assert 0 "main() {b=0;c=-4;i=50;while(i<54){i=i+1;b=b+1;c=c+2;} return b-c;}"
assert 100 "main() {ans=0;for(i=0;i<10;i=i+1){for(j=0;j<10;j=j+1){ans=ans+1;}}return ans;}"
assert 24 "main() {ans=0;i=0;while(i<30){if(i<10){ans=ans*2;}else{ans=ans-50;}i=i+1;}return ans;}"
assert 3 "main() {a1=0;b_1=3;return a1+b_1;}"

assert 55 "fib(n) {if (n <= 1) return n; return fib(n-1)+fib(n-2);} main() {return fib(10);}"
assert 21 "fib(n) {if (n <= 1) return n; return fib(n-1)+fib(n-2);} main() {return fib(fib(6));}"
assert 55 "fib(n) {if (n <= 1) return n; return fib(n-1)+fib(n-2);} main() {return fib(fib(fib(5))+fib(5));}"
assert 1 "main() {a = 0;{b = 4;a=a+1;}return a;}"
assert 1 "main() {{}a=0;return a+1;}"
assert 7 "main() {a = 3;b = &a;c = *b;return c + 4;}"
assert 6 "main() {a = 10;b = &a;c = *b;return c - 4;}"

echo OK 