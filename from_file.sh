#!/bin/bash

input=$1
make
cc -c testdata/$input/cc.c
./hcc < testdata/$input/hcc.c > tmp.s
cc -o tmp tmp.s cc.o
./tmp