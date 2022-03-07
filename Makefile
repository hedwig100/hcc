CFLAGS=-std=c11 -g
SRCS=$(wildcard src/*.c) 
OBJS=$(SRCS:.c=.o)

TESTSRCS=$(wildcard test/*.c)
TESTOBJS=$(TESTSRCS:.c=.exe)

hcc: $(OBJS)
	$(CC) -o hcc $(OBJS)

$(OBJS): src/hcc.h

test/%.exe: hcc test/%.c
	$(CC) -o test/$*.i -E -P -C test/$*.c
	./hcc test/$*.i > test/$*.s
	$(CC) -o $@ test/$*.s -xc test/common

test: $(TESTOBJS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

clean:
	rm -f src/*.o hcc tmp.s tmp cc.o test/*.exe test/*.s test/*.i

.PHONY: test clean