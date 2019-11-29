//IFJ PROJECT 2019/20
//Generator
//Autor - Vojtech Jurka (xjurka08)

#ifndef GENERATOR_H
#define GENERATOR_H

#include "lexical_analysis.h"

// typy jednotlivych instrukci
#define INSTRUCTION_COUNT 56

const char* instructions [INSTRUCTION_COUNT];

#define I_MOVE          0
#define I_CREATEFRAME   1
#define I_PUSHFRAME     2
#define I_POPFRAME      3
#define I_DEFVAR        4
#define I_CALL          5
#define I_RETURN        6
#define I_PUSHS         7
#define I_POPS          8
#define I_CLEARS        9
#define I_ADD           10
#define I_SUB           11
#define I_MUL           12
#define I_DIV           13
#define I_IDIV          14
#define I_ADDS          15
#define I_SUBS          16
#define I_MULS          17
#define I_DIVS          18
#define I_IDIVS         19
#define I_LT            20
#define I_GT            21
#define I_EQ            22
#define I_LTS           23
#define I_GTS           24
#define I_EQS           25
#define I_AND           26
#define I_OR            27
#define I_NOT           28
#define I_ANDS          29
#define I_ORS           30
#define I_NOTS          31
#define I_INT2FLOAT     32
#define I_FLOAT2INT     33
#define I_INT2CHAR      34
#define I_STRI2INT      35
#define I_INT2FLOATS    36
#define I_FLOAT2INTS    37
#define I_INT2CHARS     38
#define I_STRI2INTS     39
#define I_READ          40
#define I_WRITE         41
#define I_CONCAT        42
#define I_STRLEN        43
#define I_GETCHAR       44
#define I_SETCHAR       45
#define I_TYPE          46
#define I_LABEL         47
#define I_JUMP          48
#define I_JUMPIFEQ      49
#define I_JUMPIFNEQ     50
#define I_JUMPIFEQS     51
#define I_JUMPIFNEQS    52
#define I_EXIT          53
#define I_BREAK         54
#define I_DPRINT        55

//typy prexifu u promennych a konstant

#define PREFIX_COUNT 8

const char* prefixes [PREFIX_COUNT];

#define P_GF        0
#define P_LF        1
#define P_TF        2
#define P_INT       3
#define P_BOOL      4
#define P_FLOAT     5
#define P_STRING    6
#define P_NIL       7

#define P_NULL      -1
#define P_LABEL     -2



typedef struct
{
    int instType;  // typ instrukce

    int prefix1; // prefix 1
    union Data data1; // data 1

    int prefix2; // prefix 2
    union Data data2; // data 2

    int prefix3; // prefix 3
    union Data data3; // data 3

} tInstr;

typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;

typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
} tListOfInstr;

//fce pro praci se seznamem instrukci
void listInit(tListOfInstr *L);
void listFree(tListOfInstr *L);
void listInsertLast(tListOfInstr *L, tInstr I);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);
void *listGetPointerLast(tListOfInstr *L);
tInstr *listGetData(tListOfInstr *L);

//fce pro vypis vsech instrukci ze seznamu
void printInstructions(tListOfInstr *L);

#endif // GENERATOR_H
