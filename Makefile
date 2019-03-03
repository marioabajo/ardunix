NAME=main

#CFLAGS=-Os -fno-builtin -Wall -fstack-usage
CFLAGS=-fno-builtin -g -Wall -Wextra -fstack-usage

C_SRCS=$(wildcard *.c)
C_OBJS=$(C_SRCS:.c=.o)

.PHONY: all clean distclean

all: $(C_OBJS)
	g++ -g -c -o ardunix.o -x c++ - < examples/ardunix.ino
	$(LINK.cc) $(C_OBJS) ardunix.o -o $(NAME)

clean:
	@- $(RM) $(C_OBJS) ardunix.o $(NAME)

distclean: clean
