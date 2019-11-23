#include <stdlib.h>
#include "stack.h"
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h" 
#include "error_codes.h"


// these instructions are prepared for you, enjoyy :) 

// #define INSTRUCTION_COUNT 56
/*
const char* instructions [INSTRUCTION_COUNT] = {
  "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN", "PUSHS", "POPS", "CLEARS",
  "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS",
  "LT", "GT", "EQ", "LTS", "GTS", "EQS", "AND", "OR", "NOT", "ANDS", 
  "ORS", "NOTS", "INT2FLOAT", "FLOAT2INT", "INT2CHAR", "STRI2INT", "INT2FLOATS", "FLOAT2INT", "INT2CHARS", "STRI2INTS", 
  "READ", "WRITE", "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "TYPE", "LABEL", "JUMP", "JUMPIFEQ", 
  "JUMPIFNEQ", "JUMPIFEQS", "JUMPIFNEQS", "EXIT", "BREAK", "DPRINT"
}; */

int main() {

  //pointer to symtable
  SymTable_t *ST;
  //int (*arr)[M] = malloc(sizeof(int[N][M]));
  tStack LexStack;
  sInit(&LexStack);
  sPush(&LexStack, NULL, 0);
  //result of syntax analysis
  int result;
  //inicialization
  ST = SymTableInit(SIZE_OF_SYMTABLE);
  
  if(ST == NULL) return INTERNAL_ERROR;
  //calling parser
  result = Parse(ST, &LexStack);

  //which error?
  printf("result: %d \n", result);

  sDispose(&LexStack);
  //delete symtable
  SymTableDelete(ST);

  
  
}