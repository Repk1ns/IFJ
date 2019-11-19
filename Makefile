CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g 
OFLAGS=-O2
DEPS = lexical_analysis.h syntax_analysis.h symtable.h
OBJ = main.o lexical_analysis.o syntax_analysis.o symtable.o


ifj: $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $(OFLAGS) -o ifj $(OBJ) -I. -lm

clean:
	-rm -f *.o ifj