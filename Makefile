NAME=main

CFLAGS=-Os -fno-builtin

C_SRCS=$(wildcard *.c)
C_OBJS=$(C_SRCS:.c=.o)

.PHONY: all clean distclean

all: $(C_OBJS)
	g++ -c -o ardunix.o -x c++ - < ardunix.ino
	$(LINK.cc) $(C_OBJS) ardunix.o -o $(NAME)

clean:
	@- $(RM) $(C_OBJS) ardunix.o

distclean: clean
