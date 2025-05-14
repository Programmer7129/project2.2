# TARGET library
CC      := gcc
AR      := ar rcs
CFLAGS  := -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=700
lib := libuthread.a
SRCS    := queue.c uthread.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean

all: $(lib)

## TODO: Phase 1

$(lib): $(OBJS)
	$(AR) $@ $^

# Compile .c files into .o
%.o: %.c queue.h uthread.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(lib)
