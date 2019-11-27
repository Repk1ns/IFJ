CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g 
OFLAGS=-O2
HEADERS = errorcode.h generator.h stack.h lexical_analysis.h symtable.h syntax_analysis.h
OBJ =  stack.o lexical_analysis.o generator.o syntax_analysis.o symtable.o main.o 

default: ifj

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(OFLAGS) -c -lm $< -o $@

ifj: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(OFLAGS) -lm -o $@

clean:
	-rm -f *.o ifj
