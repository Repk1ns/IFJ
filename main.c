#include <stdlib.h>
#include "lexical_analysis.h"


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
  struct Symbol symbol;
  do {
    symbol = getNextSymbol(stdin);
    switch (symbol.type){
      case _int:
      case _whitespace: {
        printf("type: %d, data: %d\n", symbol.type, symbol.data.int_data);
        break;
      }
      case _double: {
        printf("type: %d, data: %f\n", symbol.type, symbol.data.dbl_data);
        break;
      }
      default: {
        printf("type: %d, data: %s\n", symbol.type, symbol.data.str_data);
        break;
      }
    }
    printf("--------------------------------------\n");
  } while (symbol.type != _eof);
}
