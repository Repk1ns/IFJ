//Symtable.h
#include <string.h> // size_t 

//zabranenie viaceremu vlozeniu hlavickoveho suboru
#ifndef SYMTABLE_H
#define SYMTABLE_H


//typ unia pre data identifikatora (moze byt int, string alebo double cislo)
typedef union SymData {
  int _int_data;
  char* _str_data;
  double _dbl_data;
} SymData_t;

//typ symbolu
typedef enum SymType {
  __int,
  __string,
  __none,
  __func,
  __keyword,
  __double,
  __id,
  __eof,
  __instruction,
  __operator,
  __ifjcode,
  __whitespace,
} SymType_t;

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
     
    //typ polozky
    SymType_t SymItemType;
    //data polozky
    SymData_t SymItemData;
    //ukazovatel na dalsiu polozku
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
void SymTableSearch(SymTable_t *ST);
//vlozenie do tabulky
void SymTableInsert(SymTable_t *ST);
//hashovacia funkcia
unsigned int SymTableHashFunction(const char *str, int SymTableSize); 


#endif
