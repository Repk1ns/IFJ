#include <stdio.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h"



// parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
// funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
int parse(SymTable_t *ST)
{
    

    Symbol_t symbol;
    do {
        symbol = getNextSymbol(stdin);
        switch (symbol.type){
        case _int:
        case _whitespace: {
            printf("type: %d, data: %d\n", symbol.type, symbol.data.int_data);
            break;
        }
        case _double: {
            printf("type: %d, data: %f\n", symbol.type, symbol.data.dbl_data);
            break;
        }
        default: {
            printf("type: %d, data: %s\n", symbol.type, symbol.data.str_data);
            break;
        }
        }
        printf("--------------------------------------\n");
    } while (symbol.type != _eof);


    //testing hash table:

    //inserting items
    Symbol_t testsymbol;

    testsymbol.data.str_data = "totojetest1";
    testsymbol.type =  6;
    SymTableInsert(ST, testsymbol);


    testsymbol.data.str_data = "totojetest2";
    testsymbol.type =  6;
    SymTableInsert(ST, testsymbol);

    //serach for item
    bool found = SymTableSearch(ST, "totojetest3");
    printf("is item in table? :%d\n", found);

    
    
    return 1;
}