#!/bin/bash 
assert() {
    expected="$1" 
    input="$2" 

    ./9cc "$input" > tmp.s 
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

assert 0 "0;" 
assert 42 "42;" 
assert 21 "5+20-4;"
assert 24 "4 - 10 + 30;"
assert 11 "5+2*3;"
assert 15 '5*(9-6);'
assert 33 "(5 - 2) * 10 + 9 / (4 - 1);"
assert 7 "-3+10;" 
assert 4 "-3*(-5+9)/(-3);"

assert 0 "0==1;" 
assert 1 "23==23;"
assert 0 "25==23;"
assert 1 "25!=23;"
assert 0 "23!=23;"
assert 1 "20<23;"
assert 0 "1<1;"
assert 0 "6<1;"
assert 1 "6<=9;"
assert 0 "10<=9;"
assert 1 "10>9;"
assert 0 "9>9;"
assert 0 "10>17;"
assert 1 "10>=9;"
assert 1 "9>=9;"
assert 0 "8>=9;"
assert 1 "(1 == 2) <= 1;"
assert 0 "(5 >= 3) != 1;"
assert 1 "(-6 < 10)==1;"
assert 1 "(-6 < 10)==1;"

assert 9 "a=10;a-1;"
assert 2 "b=-14;b+16;"
assert 24 "c=-6;c*(-4);"
assert 3 "d=(3 < 5);d+2;"
assert 4 "e=-134;e+138;"
assert 15 "f=(10 - 5*3); f+20;"
assert 17 "g=1;h=1;i=12;j=-15;i+5;"
assert 4 "k=1;l=1;m=12;n=-15;n+19;"
assert 21 "o=16/1;p=(1-3)*8;q=12;r=-15;o+5;"
assert 6 "s=1;t=1;u=(10-3)*3*2;v=-15;u/7;"
assert 3 "u=1;v=1;w=12;w-9;"
assert 9 "x=1;y=1;z=12;x+8;"

echo OK 