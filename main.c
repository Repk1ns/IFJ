#include <stdlib.h>
#include "lexical_analyse.h"
#include "syntax_analyse.h"
#include "symtable.h"

int main() {
  //lexikalni analyzatore, dej mi dalsi symbol:
  struct Symbol symbol;
  do {
    symbol = getNextSymbol(stdin);
    printf("type: %d, data: %s\n", symbol.type, symbol.data.str_data);
    //printf("type: %d\n", symbol.type);
  } while (symbol.type == _eof);
  
  //printf("type: %d, value: %d", symbol.type, symbol.data.int_data);
  //tady zpracuji symbol
}
