#include<stdio.h>

int foo() {
    printf("OK\n");
}

/*
make
cc -c testdata/test1/cc.c
./hcc "foo();return 0;” > tmp.s
cc -o tmp tmp.s cc.o
./tmp

>> 

OK
*/