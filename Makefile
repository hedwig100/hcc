CFLAGS=-std=c11 -g -static 
SRCS=$(wildcard src/*.c) 
OBJS=$(SRCS:.c=.o)

hcc: $(OBJS)
		$(CC) -o hcc $(OBJS) $(LDFLAGS)

$(OBJS): src/hcc.h

test: hcc
		./test.sh 

clean:
		rm -f hcc *.o *~ tmp* 

.PHONY: test clean