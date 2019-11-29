//Symtable.h
#include <string.h> // size_t 

//zabranenie viaceremu vlozeniu hlavickoveho suboru
#ifndef SYMTABLE_H
#define SYMTABLE_H

//velkost globalnej hashovacej tabulky
#define SIZE_OF_SYMTABLE_GLOBAL 7901
//velkost lokalnej hashovacej tabulky
#define SIZE_OF_SYMTABLE_LOCAL 313
//neuplna deklarace struktur
typedef struct SymTableItem SymTableItem_t;
typedef struct SymTable SymTable_t;
typedef struct SymTableIterator SymTableIterator_t;


enum TypeOfId {
    _IDvariable,
    _IDfunction
};

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
    //typ polozky, ci je to funckia alebo premmenna
    int Type;
    //pocet parametrov
    int NumberOfParameters;
    //nazov funkcie alebo premmenej
    Symbol_t SymData;
    //ukazovatel na dalsi symbol
    SymTableItem_t *SymItemNext;

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
SymTableItem_t* SymTableSearch(SymTable_t *ST, const char  *Key, int sizeOfSymtable);
//pomocna funkcia na vytvorenie noveho itemu
SymTableItem_t* NewItem(Symbol_t data, int type, int numberOfParameters);
//vlozenie do tabulky
void SymTableInsert(SymTable_t *ST, Symbol_t token, int type, int numberOfParameters, int sizeOfSymtable);
//hashovacia funkcia
unsigned int SymTableHashFunction(const char *str, int SymTableSize); 
//funckia na porovnanie dvoch klucov
bool CompareKeys(const char* Key1, const char* Key2);



#endif
