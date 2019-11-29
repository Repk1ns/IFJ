#include <stdio.h>
#include <stdlib.h>
#include "generator.h"
#include "error_codes.h"

//typy instrukci
const char* instructions [INSTRUCTION_COUNT] = {
  "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN", "PUSHS", "POPS", "CLEARS",
  "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS",
  "LT", "GT", "EQ", "LTS", "GTS", "EQS", "AND", "OR", "NOT", "ANDS",
  "ORS", "NOTS", "INT2FLOAT", "FLOAT2INT", "INT2CHAR", "STRI2INT", "INT2FLOATS", "FLOAT2INTS", "INT2CHARS", "STRI2INTS",
  "READ", "WRITE", "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "TYPE", "LABEL", "JUMP", "JUMPIFEQ",
  "JUMPIFNEQ", "JUMPIFEQS", "JUMPIFNEQS", "EXIT", "BREAK", "DPRINT"
};


//typy prexifu u promennych a konstant
const char* prefixes [PREFIX_COUNT] = {
  "GF", "LF", "TF", "int", "bool", "float", "string", "nil"
};

void listInit(tListOfInstr *L)
// funkce inicializuje seznam instrukci
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}

void listFree(tListOfInstr *L)
// funkce dealokuje seznam instrukci
{
  tListItem *ptr;
  while (L->first != NULL)
  {
    ptr = L->first;
    L->first = L->first->nextItem;
    // uvolnime celou polozku
    free(ptr);
  }
}

void listInsertLast(tListOfInstr *L, tInstr I)
// vlozi novou instruci na konec seznamu
{
  tListItem *newItem;
  newItem = malloc(sizeof (tListItem));
  if(newItem == NULL)
  {
    fprintf(stderr, "ERROR of allocation!\n");
    exit(INTERNAL_ERROR);
  }
  newItem->Instruction = I;
  newItem->nextItem = NULL;
  if (L->first == NULL)
     L->first = newItem;
  else
     L->last->nextItem=newItem;
  L->last=newItem;
}

void listFirst(tListOfInstr *L)
// zaktivuje prvni instrukci
{
  L->active = L->first;
}

void listNext(tListOfInstr *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  L->active = L->active->nextItem;
}

void listGoto(tListOfInstr *L, void *gotoInstr)
// nastavime aktivni instrukci podle zadaneho ukazatele
{
  L->active = (tListItem*) gotoInstr;
}

void *listGetPointerLast(tListOfInstr *L)
// vrati ukazatel na posledni instrukci
{
  return (void*) L->last;
}

tInstr *listGetData(tListOfInstr *L)
// vrati aktivni instrukci
{
  if (L->active == NULL)
  {
    printf("Chyba, zadna instrukce neni aktivni");
    return NULL;
  }
  else return &(L->active->Instruction);
}

void printInstructions(tListOfInstr *L)
{
    printf(".IFJcode19\n");
    listFirst(L);
    while(L->active != NULL)//opakuj pro vsechny instrukce
    {

        printf("%s", instructions[L->active->Instruction.instType]);//vytskni operacni kod instrukce

        if(L->active->Instruction.prefix1 != P_NULL)//pokud existuje prvni operand instrukce
        {
            if(L->active->Instruction.prefix1 == P_INT)//pokud je prvni operand instrukce konstanta int, zapiseme ji jako %d
            {
                printf(" %s@%d ", prefixes[L->active->Instruction.prefix1], L->active->Instruction.data1.int_data);
            }
            else if(L->active->Instruction.prefix1 == P_FLOAT)//pokud je prvni operand instrukce konstanta int, zapiseme ji jako %a
            {
                printf(" %s@%a", prefixes[L->active->Instruction.prefix1], L->active->Instruction.data1.dbl_data);
            }
            else if(L->active->Instruction.prefix1 == P_NIL)//pokud je prvni operand instrukce konstanta nil, zapiseme ji jako "nil@nil"
            {
                printf(" nil@nil");
            }
            else if(L->active->Instruction.prefix1 == P_LABEL) //pokud je operand navesti
            {
                printf(" %s", L->active->Instruction.data1.str_data);
            }
            else//Jinak to bude konstanta string, bool nebo promenna. Ty se vsechny zapisuji jako "prefix@nejaky_string".
            {
                printf(" %s@%s", prefixes[L->active->Instruction.prefix1], L->active->Instruction.data1.str_data);
            }
        }

        if(L->active->Instruction.prefix2 != P_NULL)//pokud existuje druhy operand instrukce
        {
            if(L->active->Instruction.prefix2 == P_INT)//pokud je druhy operand instrukce konstanta int, zapiseme ji jako %d
            {
                printf(" %s@%d", prefixes[L->active->Instruction.prefix2], L->active->Instruction.data2.int_data);
            }
            else if(L->active->Instruction.prefix2 == P_FLOAT)//pokud je druhy operand instrukce konstanta int, zapiseme ji jako %a
            {
                printf(" %s@%a", prefixes[L->active->Instruction.prefix2], L->active->Instruction.data2.dbl_data);
            }
            else if(L->active->Instruction.prefix2 == P_NIL)//pokud je druhy operand instrukce konstanta nil, zapiseme ji jako "nil@nil"
            {
                printf(" nil@nil");
            }
            else//Jinak to bude konstanta string, bool nebo promenna. Ty se vsechny zapisuji jako "prefix@nejaky_string".
            {
                printf(" %s@%s", prefixes[L->active->Instruction.prefix2], L->active->Instruction.data2.str_data);
            }
        }

        if(L->active->Instruction.prefix3 != P_NULL)//pokud existuje prvni operand instrukce
        {
            if(L->active->Instruction.prefix3 == P_INT)//pokud je prvni operand instrukce konstanta int, zapiseme ji jako %d
            {
                printf(" %s@%d", prefixes[L->active->Instruction.prefix3], L->active->Instruction.data3.int_data);
            }
            else if(L->active->Instruction.prefix3 == P_FLOAT)//pokud je prvni operand instrukce konstanta int, zapiseme ji jako %a
            {
                printf(" %s@%a", prefixes[L->active->Instruction.prefix3], L->active->Instruction.data3.dbl_data);
            }
            else if(L->active->Instruction.prefix3 == P_NIL)//pokud je prvni operand instrukce konstanta nil, zapiseme ji jako "nil@nil"
            {
                printf(" nil@nil");
            }
            else//Jinak to bude konstanta string, bool nebo promenna. Ty se vsechny zapisuji jako "prefix@nejaky_string".
            {
                printf(" %s@%s", prefixes[L->active->Instruction.prefix3], L->active->Instruction.data3.str_data);
            }
        }
        printf("\n");

        listNext(L);//presun se na dalsi instrukci

    }
}
