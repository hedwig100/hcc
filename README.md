# hcc
Hcc is a toy C compiler. Hcc can compile itself though only subset of C is suppported. 

<br>

## Features
### Supported
- local,global variable declaration,definition,initialization
- `int,void`,pointer,array,`struct`,`enum`
- various operation (`+`,`-`,`*`,`%`,`&&`,`>>`,etc.)
- `char*`,`char`,`int`,array literal
- `if`,`for`,`while`,`do ... while`,`switch`,`case`,`default`,
- `[]`,`.`,`->`,`++`,`--`,`&`,`*`
- function call(with more 6 parameters,variable length arguments),definition
- simple cast
- `typedef`

etc.

### Not Supported yet
- `union`,`unsigned`,`long`,`short`,`_Bool`,`float`
- preprocess (macro expansion)
- bitfield
- functional pointer
- struct initalization,struct as a function parameter,
- static,const,inline
- valid name resolution
- goto

etc.

<br>

## Setup

```
git clone https://github.com/hedwig100/hcc && cd hcc
docker build -t hcc .
docker run -it --name hcc hcc /bin/bash
```

<br>

## Example
### self compile

```
make hcc3
diff hcc2 hcc3 -s
```

Output:
```
Files hcc2 and hcc3 are identical
```

### fibonacci

```
./example.sh fib
```

Output: 
```
fib(0): 0 
fib(1): 1 
fib(2): 1 
fib(3): 2 
fib(4): 3 
fib(5): 5 
fib(6): 8 
fib(7): 13 
fib(8): 21 
fib(9): 34 
fib(10): 55 
fib(11): 89 
fib(12): 144 
fib(13): 233 
fib(14): 377 
fib(15): 610 
fib(16): 987 
fib(17): 1597 
fib(18): 2584 
fib(19): 4181 
fib(20): 6765 
fib(21): 10946 
fib(22): 17711 
fib(23): 28657 
fib(24): 46368
```

### hash

```
./example.sh hash
```

Output:
```
start! number of data: 0
       INSERT 0!
       number of data: 1
       INSERT 1!
       number of data: 2
       INSERT 2!
       number of data: 3
       INSERT 3!
       number of data: 4
       INSERT 4!
       number of data: 5
       INSERT 5!
       number of data: 6
       INSERT 6!
       number of data: 7
       INSERT 7!
       number of data: 8
       INSERT 8!
       number of data: 9
       INSERT 9!
       number of data: 10
       DELETE 0!
       number of data: 9
       DELETE 2!
       number of data: 8
       DELETE 4!
       number of data: 7
       DELETE 6!
       number of data: 6
       DELETE 8!
       number of data: 5
finished!
```

<br>

## Reference

- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#) written in Japanese.
- [chibicc](https://github.com/rui314/chibicc)
- [The syntax of C in Backus-Naur Form](https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm)
- [compiler explorer](https://godbolt.org/)
