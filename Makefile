CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror -g 
OFLAGS=-O2
OBJ = generator.o stack.o lexical_analysis.o syntax_analysis.o symtable.o main.o 


ifj: $(OBJ)
	$(CC) $(CFLAGS) $(OFLAGS) -o ifj $(OBJ) -I. -lm

clean:
	-rm -f *.o ifj
