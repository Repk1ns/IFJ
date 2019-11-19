#include <stdlib.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h" 


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
  //result of syntax analysis
  int result;
  //inicialization
  ST = SymTableInit(SIZE_OF_SYMTABLE);

  //calling parser
  result = Parse(ST);

  //which error?
  printf("result: %d \n", result);

  //delete symtable
  SymTableDelete(ST);

  
  
}