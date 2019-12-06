/* IFJ project 2019/2020: IFJcode19
* main.c
* Authors = Martina Tuckova (xtucko00) 
*           David Spavor (xspavo00) 
*           Vojtech Jurka (xjurka08)
*           Vojtech Mimochodek (xmimoc01)
*/
#include <stdlib.h>
#include "stack.h"
#include "lexical_analysis.h"
#include "symtable.h"
#include "expression.h"
#include "syntax_analysis.h" 
#include "error_codes.h"
#include "generator.h"


int main() {

  //list of instructions
  tListOfInstr IL;
  listInit(&IL);

  //pointer to symtable
  SymTable_t *ST;
  //int (*arr)[M] = malloc(sizeof(int[N][M]));
  tStack LexStack;
  sInit(&LexStack);
  sPush(&LexStack, 0, 0);
  //result of syntax analysis
  int result;
  //inicialization
  ST = SymTableInit(SIZE_OF_SYMTABLE_GLOBAL);
  
  if(ST == NULL) return INTERNAL_ERROR;
  //calling parser
  result = Parse(ST, &LexStack, &IL);


  if(result == IT_IS_OKAY)
  {
    printInstructions(&IL);
  }

  sDispose(&LexStack);
  //delete symtable
  SymTableDelete(ST);
  listFree(&IL);

  
  return result;
}
