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

assert 0 0 
assert 42 42 
assert 21 "5+20-4"
assert 24 "4 - 10 + 30"
assert 11 "5+2*3"
assert 15 '5*(9-6)'
assert 33 "(5 - 2) * 10 + 9 / (4 - 1)"
echo OK 