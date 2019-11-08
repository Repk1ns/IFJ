//Symtable.h
#include <string.h> // size_t 

//zabranenie viaceremu vlozeniu hlavickoveho suboru
#ifndef SYMTABLE_H
#define SYMTABLE_H

//velkost kluca na vyhladavanie
#define SIZE_OF_KEY 10
//velkost hashovacej tabulky
#define SIZE_OF_SYMTABLE 1
//neuplna deklarace struktur
typedef struct SymTableItem SymTableItem_t;
typedef struct SymTable SymTable_t;
typedef struct SymTableIterator SymTableIterator_t;

//typ hashovacia tabulka
struct SymTable
{
    size_t SymTableSize; // velkost tabulky
    size_t SymTableItemCount; // pocet itemov v tabulke
    SymTableItem_t *SymTableArray[]; // pole pointerov ukazujuce na itemy tabulky

};

//polozka tabulky
struct SymTableItem
{
    
    //Data polozky
    Symbol_t SymData;
    //ukazovatel na dalsi symbol
    SymTableItem_t *SymItemNext;
    //vyhladavaci kluc
    char SymKey[]; 

};

// iterátor
struct SymTableIterator
{ 
    // ukazatel na položku
    SymTableItem_t *SymIterator; 
    // na ktorom indexe v tabulke 
    int idx; 
}; 


//inicializacia tabulky, vracia mi typ tabulky
SymTable_t* SymTableInit(size_t size);
//vymazanie tabulky
void SymTableDelete(SymTable_t *ST);
//hladanie v tabulke podla key
bool SymTableSearch(SymTable_t *ST, char  *Key);
//pomocna funkcia na vytvorenie noveho itemu
SymTableItem_t* NewItem(Symbol_t data);
//vlozenie do tabulky
void SymTableInsert(SymTable_t *ST, Symbol_t token);
//hashovacia funkcia
unsigned int SymTableHashFunction(const char *str, int SymTableSize); 
//funckia na porovnanie dvoch klucov
bool CompareKeys(char* Key1, char* Key2);



#endif
