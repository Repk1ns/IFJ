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
 /*
Take the next item that the head points to and save it
Free the head item
Set the head to be the next item that we've stored on the side
*/
void SymTableDelete(SymTable_t *ST)
{
    unsigned i=0;
    SymTableItem_t *nod = NULL ; 
    //prechadzame celu tabulku
    while(i <  ST->SymTableSize)
    {
        //ak tabulka nie je prazdna -> narazili sme na ukazetel na item
        if(ST->SymTableArray[i] != NULL)
        {
            //prechadzame pole pointrov dovtedy, dokym vsetky su null
            while(ST->SymTableArray[i]!=NULL)
            {
                //ulozime su dalsi prvok
                nod=ST->SymTableArray[i]->SymItemNext;
                //odstranime aktulany prvok
                free(ST->SymTableArray[i]);
                //dalsi prvok sa stane aktualnym
                ST->SymTableArray[i]=nod;
            }
        }    
        i++;
    }
    free(ST);
}

//hashovacia funkcia na rozptylenie itemov v tabulke
unsigned int htab_hash_function(const char *str, int size) 
{ 
    unsigned int h=0; 
    const unsigned char *p;
    for(p=(const unsigned char*) str; *p!='\0'; p++) 
    h = 65599*h + *p; 
    return h%size; 
}