#include <stdio.h>
#include "symtable.h"
#include "syntax_analyse.h"



//parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
//funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
// int parse(SymTable_t *ST)
// {
//     ST = SymTableInit(10);
//     printf("%ld", ST->SymTableSize);
//     return 1;
// }