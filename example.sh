#!/bin/bash 

filename="$1"

cc -o example/$filename.i example/$filename.c -E -P 
./hcc2 example/$filename.i > example/$filename.s
cc -o example/$filename.o example/$filename.s
example/$filename.o