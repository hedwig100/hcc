#include<stdio.h>

int foo() {
    printf("OK\n");
}

/*
make
cc -c testdata/test1/cc.c
./hcc < testdata/test1/hcc.c > tmp.s
cc -o tmp tmp.s cc.o
./tmp

>> 

OK
*/