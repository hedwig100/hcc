CFLAGS=-std=c11 -g
SRCS=$(wildcard src/*.c) 
OBJS=$(SRCS:.c=.o)
HOSTS_ASM2=$(SRCS:.c=2.s)
HOSTS_ASM3=$(SRCS:.c=3.s)

TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.exe)
TESTOBJS2=$(TESTSRCS:.c=2.exe)
TESTOBJS3=$(TESTSRCS:.c=3.exe)

hcc: $(OBJS)
	$(CC) -o hcc $(OBJS)

$(OBJS): src/hcc.h

test/%.exe: hcc test/%.c
	$(CC) -o test/$*.i -E -P -C test/$*.c
	./hcc test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test1: $(TESTOBJS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

# for self-hosting (second generation)
hcc2: $(HOSTS_ASM2)
	$(CC) -o hcc2 $(HOSTS_ASM2)

src/%2.s: hcc src/%.c
	$(CC) -o src/$*.i -E -P -C -D_HOST src/$*.c
	./hcc src/$*.i > src/$*2.s

test/%2.exe: hcc2 test/%.c
	$(CC) -o test/$*.i -E -P -C -D_HOST test/$*.c
	./hcc2 test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test2: $(TESTOBJS2)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

# for self-hosting (third generation)
hcc3: $(HOSTS_ASM3)
	$(CC) -o hcc3 $(HOSTS_ASM3)

src/%3.s: hcc2 src/%.c
	$(CC) -o src/$*.i -E -P -C -D_HOST src/$*.c
	./hcc2 src/$*.i > src/$*3.s

test/%3.exe: hcc3 test/%.c
	$(CC) -o test/$*.i -E -P -C -D_HOST test/$*.c
	./hcc3 test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test3: $(TESTOBJS3)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

# check if binary is the same
testb: hcc2 hcc3
	diff hcc2 hcc3; if [ $$? -eq 0 ]; then echo "OK"; else echo "hcc2 and hcc3 aren't the same."; exit 1; fi 

test: test1 test2 test3 testb

clean:
	rm -f src/*.o src/*.s src/*.i hcc tmp.s tmp cc.o test/*.exe test/*.s test/*.i

.PHONY: test clean