.PHONY: all clean

CC := /opt/buildroot-gcc342/bin/mipsel-linux-gcc
DIR := $(shell pwd)

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
SRC := $(patsubst %.o, %.c,$(OBJS))
CFLAGS := -O2
LDFLAGS := 

all : memtool

memtool : $(OBJS)
	$(CC) -o memtool $(LDFLAGS) $^
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o;
	rm -rf main;
	rm -rf *.so;

