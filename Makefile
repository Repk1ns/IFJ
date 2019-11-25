CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g 
OFLAGS=-O2

default: ifj

ifj: main.o lexical_analysis.o expression.o stack.o
	$(CC) $(CFLAGS) $(OFLAGS) -o ifj main.o lexical_analysis.o expression.o stack.o -I. -lm

clean:
	-rm -f *.o ifj
