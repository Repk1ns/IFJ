ifj: main.c lexical_analyse.c
	gcc -o ifj main.c lexical_analyse.c -I.

clean:
	-rm -f *.o ifj
