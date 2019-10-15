CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g 
OFLAGS=-O2

default: ifj

ifj: main.o lexical_analysis.o syntax_analysis.o symtable.o
	$(CC) $(CFLAGS) $(OFLAGS) -o ifj main.o lexical_analysis.o syntax_analysis.o symtable.o -I. -lm

clean:
	-rm -f *.o ifj
