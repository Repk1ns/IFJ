CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g
OFLAGS=-O2

default: ifj

ifj: main.o lexical_analyse.o syntax_analyse.o symtable.o
	$(CC) $(CFLAGS) $(OFLAGS) -o ifj main.o lexical_analyse.o syntax_analyse.o symtable.o -I.

clean:
	-rm -f *.o ifj
