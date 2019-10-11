#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

//inicializacia tabulky
SymTable_t* SymTableInit(size_t size)
{
    SymTableItem_t item; 
    SymTable_t *ST = (SymTable_t *)malloc(sizeof(SymTable_t) + sizeof(item) * size); 
    if(ST == NULL) 
    {
        fprintf(stderr, "ERROR! Memory cannot be allocated!\n");
        return NULL;
    }
    ST->SymTableSize = size;
    ST->SymTableItemCount = 0;
    
    for(unsigned i = 0; i < size; i++)
	{ 
		ST->SymTableArray[i] = NULL;
    }
    return ST;
}