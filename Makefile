APP=v
INCLUDES=-Iinclude/
LIBS=-lncursesw
CFLAGS=-Wall -std=c11 -O2
BUILDDIR=build/
CC=gcc

SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:src/%.c=build/%.o)

.PHONY = all clean

all: build/$(APP)

build/$(APP): $(OBJS)
	 	 $(CC) $(CFLAGS) $(INCLUDES) -o $@ -static $^ $(LIBS)

build/%.o: src/%.c
		@mkdir -p $(BUILDDIR)
		$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
		rm -rf build/
