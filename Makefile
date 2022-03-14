CFLAGS=-std=c11 -g
SRCS=$(wildcard src/*.c) 
OBJS=$(SRCS:.c=.o)
HOSTS_ASM=$(SRCS:.c=.s)

TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.exe)
TESTOBJS2=$(TESTSRCS:.c=2.exe)

hcc: $(OBJS)
	$(CC) -o hcc $(OBJS)

$(OBJS): src/hcc.h

test/%.exe: hcc test/%.c
	$(CC) -o test/$*.i -E -P -C test/$*.c
	./hcc test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test: $(TESTOBJS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

# for self-hosting
hcc2: $(HOSTS_ASM)
	$(CC) -o hcc2 $(HOSTS_ASM)

src/%.s: hcc src/%.c
	$(CC) -o src/$*.i -E -P -C -D_HOST src/$*.c
	./hcc src/$*.i > src/$*.s

test/%2.exe: hcc2 test/%.c
	$(CC) -o test/$*.i -E -P -C -D_HOST test/$*.c
	./hcc2 test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test2: $(TESTOBJS2)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

clean:
	rm -f src/*.o hcc tmp.s tmp cc.o test/*.exe test/*.s test/*.i

.PHONY: test clean