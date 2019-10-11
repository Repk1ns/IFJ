#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "lexical_analyse.h"

const char* keywords [KEYWORD_COUNT] = {
  "def", "else", "if", "None", "pass", "return", "while"
};
const char* functions [FUNCTION_COUNT] = {
  "print", "inputs", "inputi", "inputf", "len", "substr", "ord", "chr"
};
const char* instructions [INSTRUCTION_COUNT] = {
  "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN", "PUSHS", "POPS", "CLEARS",
  "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS",
  "LT", "GT", "EQ", "LTS", "GTS", "EQS", "AND", "OR", "NOT", "ANDS", 
  "ORS", "NOTS", "INT2FLOAT", "FLOAT2INT", "INT2CHAR", "STRI2INT", "INT2FLOATS", "FLOAT2INT", "INT2CHARS", "STRI2INTS", 
  "READ", "WRITE", "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "TYPE", "LABEL", "JUMP", "JUMPIFEQ", 
  "JUMPIFNEQ", "JUMPIFEQS", "JUMPIFNEQS", "EXIT", "BREAK", "DPRINT"
};

struct Symbol getNextSymbol(FILE* input) {
  struct Symbol symbol;
  symbol.data.dbl_data = 0; 
  bool while_condition = true; // opakovat dokud
  int state = S; // stav, ktery se bude provadet
  int character; // prave nacteny znak
  char IFJbuffer[10];
  char help[2];
  char buffer[MAX_ID_LENGTH];
  int whitespace = 1; 
  char* tmp_str = '\0';
  symbol.data.int_data = '\0';
  symbol.data.str_data = '\0';
  symbol.data.dbl_data = '\0';

  while (while_condition) {
    switch (state) {
      case S: {
        character = getchar();
        if(character == '#') {
          state = Q1;
          break;
        } else if(character == '"') {
          state = Q2;
          break;
        } else if (character == EOF){
          state = F;
          break;
        } else if (character == '_' || isalpha(character)){
          buffer[0] = character;
          buffer[1] = '\0';
          state = Q7;
          break;
        } else if(character == '+') {
          symbol.type = _operator;
          symbol.data.str_data = "+";
          return symbol;
        } else if(character == '-') {
          symbol.type = _operator;
          symbol.data.str_data = "-";
          return symbol;
        } else if(character == '*') {
          symbol.type = _operator;
          symbol.data.str_data = "*";
          return symbol;
        } else if(character == '(') {
          symbol.type = _operator;
          symbol.data.str_data = "(";
          return symbol;
        } else if(character == ')') {
          symbol.type = _operator;
          symbol.data.str_data = ")";
          return symbol;
        } else if(character == '/') {
            state = Q22;
            break;
        } else if(character == ':') {
          symbol.type = _operator;
          symbol.data.str_data = ":";
          return symbol;
        } else if(character == '<') {
            state = Q23;
            break;
        } else if(character == '>') {
            state = Q24;
            break;
        } else if(character == '=') {
            state = Q25;
            break;
        } else if(character == '!') {
          state = Q26;
          break;
        } else if(character == '.') {
          state = Q27;
          break;
        } else if(character == '\'') {
          state = Q16;
          break;
        } else if(character == '0') {
          state = Q21;
          break;
        } else if(character > 48 && character < 58) {
          state = Q9;
          break;
        } else if(character == ' ') {
          state = Q20;
          break;
        }
        else
        {
          //tu nebolo osetrene, co ma nastat ak nie je zadany ani jeden pozadovany charakter, treba este osetrit puzzlebean
          printf("There is not such a character\n");
          break;
        }
        

      }
      case Q1: { // # single line comment
        character = getchar();
        if(character == EOF) { // F
          state = F;
          break;
        } else if(character == EOL) { // S
          state = S;
          break;
        } else {
          break;
        }
      }
      case Q2: { // """ """ multiline comment
        character = getchar();
        if(character == '"') { // Q3
          state = Q3;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q3: { // multiline comment
        character = getchar();
        if(character == '"') { 
          state = Q4;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q4: { // multiline comment
        character = getchar();
        if(character == '"') { 
          state = Q5;
          break;
        } else if (character == EOF){
          state = Fx;
          break;
        } else {
          break;
        }
      }
      case Q5: { // multiline comment
        character = getchar();
        if(character == '"') { 
          state = Q6;
          break;
        } else if (character == EOF){
          state = Fx;
          break;
        } else {
          state = Q4;
          break;
        }
      }
      case Q6: { // multiline comment
        character = getchar();
        if(character == '"') { 
          state = S;
          break;
        } else if (character == EOF){ // = namiesto ==
          state = EOF;
          break;
        } else {
          state = Q4;
          break;
        }
      }
      case Q7: { // multiline comment
        character = getchar();
        if (character == '_' || isalnum(character)){
          sprintf(help, "%c", character);
          strcat(buffer, help);
          break;
        } else {
          symbol.data.str_data = buffer;
          ungetc(character, input);
          state = Q8;
          break;
        }
      }
      case Q8: { // compare, keywords
        for (int i = 0; i < KEYWORD_COUNT; i++){
          if (strcmp(buffer, keywords[i]) == 0){
            symbol.type = _keyword;
            return symbol;
          }
        } // compare instructions
        for (int i = 0; i < INSTRUCTION_COUNT; i++){
          if (strcmp(buffer, instructions[i]) == 0){
            symbol.type = _instruction;
            return symbol;
          }
        } // compare functions
        for (int i = 0; i < FUNCTION_COUNT; i++){
          if (strcmp(buffer, functions[i]) == 0){
            symbol.type = _func;
            return symbol;
          }
        } // jinak identifikator
        symbol.type = _id;
        return symbol;
      }
      case Q22: {
        character = getchar();
        if(character == '/') {
          symbol.data.str_data = "//";
        } else {
          ungetc(character, input);
          symbol.data.str_data = "/";
        }
        symbol.type = _operator;
        return symbol;
      }
      case Q23: {
        character = getchar();
        if(character == '=') {
          symbol.data.str_data = "<=";
        } else {
          ungetc(character, input);
          symbol.data.str_data = "<";
        }
        symbol.type = _operator;
        return symbol;
      }
      case Q24: {
        character = getchar();
        if(character == '=') {
          symbol.data.str_data = ">=";
        } else {
          ungetc(character, input);
          symbol.data.str_data = ">";
        }
        symbol.type = _operator;
        return symbol;
      }
      case Q25: {
        character = getchar();
        if(character == '=') {
          symbol.data.str_data = "==";
        } else {
          ungetc(character, input);
          symbol.data.str_data = "=";
        }
        symbol.type = _operator;
        return symbol;
      }
      case Q26: {
        character = getchar();
        if(character == '=') {
          symbol.type = _operator;
          symbol.data.str_data = "!=";
          return symbol;
        } else {
          ungetc(character, input);
          state = Fx;
          break;
        }
      }
      case Q27: {
        for(int i = 0; i < 9; i++) {
          IFJbuffer[i] = getchar();
          if(IFJbuffer[i] == EOF) {
            state = Fx;
            break;
          }
        }
        IFJbuffer[9] = '\0';
        if(strcmp(IFJbuffer, "IFJcode19") == 0) {
          symbol.type = _ifjcode;
          return symbol;
        } else {
          state = Fx;
          break;
        }
      }
      case Q16: {
        character = getchar();
        if(character == '\'') {
          symbol.type = _string;
          return symbol;
        } else if (character == '\\') {
          sprintf(help, "%c", character);
          strcpy(symbol.data.str_data, help);
          state = Q17;
          break;
        } else if(character == EOF || character == EOL) {
          state = Fx;
          break;
        } else { //TODO is this really working?
          sprintf(help, "%c", character);
          strcpy(symbol.data.str_data, help);
          break;
        }
      }
      case Q17: {
        character = getchar();
        if(character == 'x') {
          state = Q18;
          sprintf(help, "%c", character);
          strcpy(symbol.data.str_data, help);
          break;
        } else if(character == EOL || character == EOF) {
          state = Fx;
          break;
        } else {
          state = Q16;
          break;
        }
      }
      case Q18: { // string hexa cislo
        character = getchar(); 
        if(ishex(character)) {
          sprintf(help, "%c", character);
          strcpy(symbol.data.str_data, help);
          state = Q19;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q19: { // string hexa cislo
        character = getchar();
        if(ishex(character)) {
          sprintf(help, "%c", character);
          strcpy(symbol.data.str_data, help);
          state = Q16;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q21: { // prisla nula na vstup
        character = getchar();
        if(character == '.') {
          sprintf(help, "%c", character);
          strcpy(tmp_str, help);
          state = Q10;
          break;
        } else if(isalpha(character) || character == '_') {
          state = Fx;
          break;
        } else {
          ungetc(character, input);
          symbol.type = _int;
          symbol.data.int_data = '0';
          return symbol;
        }
      }
      case Q9: { // prisla 1 az 9 ze stavu S
        character = getchar();
        if(character == '.') {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          state = Q10;
          break;
        } else if(character == 'e' || character == 'E') {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          state = Q11;
          break;
        } else if(isdigit(character)) {
           sprintf(help, "%d", character);
           strcat(tmp_str, help);
           symbol.data.dbl_data = atof(tmp_str);
           symbol.data.int_data = atoi(tmp_str);
          break;
        } 
        else {
          ungetc(character, input);
          symbol.type = _int;
          return symbol;
        } 
      }
      case Q10: { // stav kdy nam prijde desetinna tecka
        character = getchar();
        if(isdigit(character)) {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          state = Q14;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q14: { // stav kdy po desetinne tecce prislo cislo
        character = getchar();
        if (isdigit(character)) {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          break;
        } else if(character == 'e' || character == 'E') {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          state = Q11;
          break;
        } else {
          ungetc(character, input);
          symbol.type = _double;
          return symbol;
        }
      }
      case Q11: { // stav kdy prislo e nebo E po nejakem cisle
        if(isdigit(character)) {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          state = Q12;
          break;
        } else if(character == '+' || character == '-') {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          state = Q13;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q12: { // prislo cislo po e E
        character = getchar();
        if(isdigit(character)) {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          break;
        } else if(isalpha(character) || character == '_') {
          state = Fx;
          break;
        } else {
          ungetc(character, input);
          symbol.type = _double;
          return symbol;
        }
      }
      case Q13: { // prislo znamenko + nebo - po e E
        character = getchar();
        if(isdigit(character)) {
          sprintf(help, "%c", character);
          strcat(tmp_str, help);
          symbol.data.dbl_data = atof(tmp_str);
          state = Q12;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q20: { // prisel whitespace
        character = getchar();
        if(character == '#') {
          state = Q1;
          break;
        } else if (character == ' ') {
          whitespace++;
          break;
        } else {
          ungetc(character, input);
          symbol.type = _whitespace;
          symbol.data.int_data = whitespace;
          return symbol;
        }
      }
      case F: {
        symbol.type = _eof;
        while_condition = false;
        break;
      }
      case Fx: {
        //tady vypisu chybu a exitnu program
      }
    }
  }

  //symbol.type = _int;
  //symbol.data.int_data = 5;


  return symbol;
}

bool ishex(int n) {
  if((n < 71 && n > 64) || (n < 103 && n > 96) || isdigit(n)) {
    return true;
  }
  return false;
}