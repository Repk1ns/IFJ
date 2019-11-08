#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lexical_analysis.h"
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

void SymTableInsert(SymTable_t *ST, Symbol_t token)
{
    int index;
    //iterator v zretazenom zozname
    SymTableItem_t* item;
    SymTableItem_t* iterator;

    //generuj index vygenerovanym klucom
    index = SymTableHashFunction(token.data.str_data ,SIZE_OF_SYMTABLE);
    printf("index: %d \n", index);

    //ak na danom indexe nie je ziadny item
    if(ST->SymTableArray[index] == NULL)
    {
        //vytvorenie itemu
        item = NewItem(token);
        if(item == NULL)
        {
                fprintf(stderr, "ERROR of allocation!\n");
                //vratit ERRORCODE
        }
        else
        {
            //priradenie daneho itemu do tabulky
            ST->SymTableArray[index] = item;
        }
      
    }
    //v tabulke uz na danom indexe je symbol, vytvarame zoznam
    else 
    {
        //iterator ukazuje na dany item
        iterator = ST->SymTableArray[index] ;
        //prechadzaj zoznam dokym neprides na koniec
        while(iterator->SymItemNext != NULL)
        {
            //porovnanie key iteratora a prvku, ze ci nahodou sa tam uz dany prvok nenachadza
            //posun iterator na dalsi prvok
            iterator = iterator->SymItemNext;
        }

        //teraz som na konci linearneho zonzamu, pripojim tam novy item
        item = NewItem(token);
        if(item == NULL)
        {
                fprintf(stderr, "ERROR of allocation!\n");
                //vratit ERRORCODE
        }
        else
        {
            //priradenie daneho itemu na koniec zoznamu
            iterator->SymItemNext = item;
        }


    }
    
   
}

//pomocna funckia na vytvorenie itemu
SymTableItem_t* NewItem(Symbol_t token)
{
        SymTableItem_t* item = malloc(sizeof(SymTableItem_t) + SIZE_OF_KEY) ;

        if( item == NULL)
        {
            return NULL;
        }
        else
        {
            //priradenie tokenu do itemu
            item->SymData = token;
            //nastavenie dalsieho ukazatela na null
            item->SymItemNext = NULL;
    
        }

        return item;

}

bool SymTableSearch(SymTable_t *ST, char  *Key)
{
    int index;
    bool foundFlag;
    SymTableItem_t* iterator;
   
    //generuj index predanym klucom
    index = SymTableHashFunction(Key,SIZE_OF_SYMTABLE);
    //na indexe nie je ziadny prvok
    if(ST->SymTableArray[index] == NULL)
    {
        //nenasli sme prvok
        foundFlag = false;
        
    }
    else
    {
         //iterator ukazuje na dany item
        iterator = ST->SymTableArray[index] ;
        foundFlag = false;
        //prechadzaj zoznam dokym neprides na koniec alebo si nenasiel prvok
        //skonci na poslednom prvku, avsak uz ho neporovna
        while(iterator->SymItemNext != NULL && !foundFlag)
        {
            //porovnanie key iteratora a prvku, ze ci nahodou sa tam uz dany prvok nenachadza
            foundFlag = CompareKeys(iterator->SymData.data.str_data, Key);
            //posun iterator na dalsi prvok
            iterator = iterator->SymItemNext;
        }

        //porovnanie posledneho prvku ak sme nenasli
        if(!foundFlag) foundFlag = CompareKeys(iterator->SymData.data.str_data, Key);
           



    }
    return foundFlag;
    

}

bool CompareKeys(char* Key1, char* Key2)
{
     if(strcmp(Key1, Key2) == 0)
    {
        return true;
    }
    else
    {
        return  false;
    }

}

//hashovacia funkcia na rozptylenie itemov v tabulke
unsigned int SymTableHashFunction(const char *str, int size) 
{ 
    unsigned int h=0; 
    const unsigned char *p;
    for(p=(const unsigned char*) str; *p!='\0'; p++) 
    h = 65599*h + *p; 
    return h%size; 
}


    
