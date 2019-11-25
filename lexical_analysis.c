/* IFJ project 2019/2020: IFJcode19
* Lexical analysis
* Author = Martina Tuckova (xtucko00)
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "stack.h"
#include "lexical_analysis.h"
#include "error_codes.h"

const char* keywords [KEYWORD_COUNT] = {
  "def", "else", "if", "None", "pass", "return", "while"
};
const char* functions [FUNCTION_COUNT] = {
  "print", "inputs", "inputi", "inputf", "len", "substr", "ord", "chr"
};

bool _FirstToken = false;
int _NumberOfSpaces = 0;
int _IndentChar = '\0';

// buffer for var names, strings, etc.
char buffer[MAX_ID_LENGTH];



Symbol_t getNextSymbol(FILE* input, void *LexStack) {
  Symbol_t symbol;
  
  symbol.data.dbl_data = 0.0; 
  bool while_condition = true;
  int state = S; // start 
  int character; // char that you get from input
  int decimal = 0;
  int exponent = 0;
  char exponent_sign = '+';
  char IFJbuffer[10]; // buffer for .IFJcode19
  char help[2];
  
  //char* tmp_str = '\0';
  buffer[0] = '\0'; // clear buffer from mess 
  //tStackItem *item;

  
  

  //item = sTop(LexStack);
  //printf("TOP OF STACK %d \n", item->intdata );
// switch case for incoming characters
  while (while_condition) {
    switch (state) {
      case S: { // start position
        if (_IndentChar == '\0')
          {
            character = getchar();
          }
          else
          {
            character = _IndentChar;
          }
          _IndentChar = '\0';

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
          //vieme ak _FirstToken je true, tak tuto pushujeme na zasobnik medzery
          if(_FirstToken)
          {
            
            _FirstToken = false;
            int Top = sLexTop(LexStack);
            //ak je viac medzier ako je na tope zasbniku tak generujeme indent
            if(Top != INTERNAL_ERROR  )
            {
              if((_NumberOfSpaces > Top))
              {
                symbol = GenerateIndent(LexStack, character);
                return symbol;
              }
              else if (_NumberOfSpaces < Top)
              {
                symbol = GenerateDedent(LexStack, character, Top);
                return symbol;
              }
              
            }
            else exit(INTERNAL_ERROR);

            if(_NumberOfSpaces != 0) sPush(LexStack, 0, _NumberOfSpaces);
            _NumberOfSpaces = 0;
          }
          state = Q7;
          break;
        } else if(character == '+') {
          symbol.type = _plus;
          strcpy(symbol.data.str_data,"+");
          return symbol;
        } else if(character == '-') {
          symbol.type = _minus;
          strcpy(symbol.data.str_data,"-");
          return symbol;
        } else if(character == '*') {
          symbol.type = _multiplication;
         strcpy(symbol.data.str_data,"*");
          return symbol;
        } else if(character == '(') {
          symbol.type = _left_bracket;
          strcpy(symbol.data.str_data,"(");
          return symbol;
        } else if(character == ')') {
          symbol.type = _right_bracket;
          strcpy(symbol.data.str_data,")");
          return symbol;
        } else if(character == '/') {
            state = Q22;
            break;
        } else if(character == ':') {
          symbol.type = _operator;
          strcpy(symbol.data.str_data,":");
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
          buffer[0] = '\0';
          state = Q16;
          break;
        } else if(isdigit(character)) {
          symbol.data.int_data = asciiToNumber(character);
          symbol.type = _int;
          if (character == '0'){
            state = Q21;
          } else {
            state = Q9;
          }
          break;
        } else if(character == EOL) {
           
          if(_FirstToken == false) 
          {
            _FirstToken = true; //TRUEEEE
            symbol.type = _eol;
            return symbol;
          }
          _FirstToken = true; //TRUEEEE
          state = S;
          break;
          
        } else if(isspace(character)) {
          if (character == ' ' || character == '\t'){
            //ak je medzera nie je na zactiaku riadku, tak ostatne medzery ignorujeme
            if(_FirstToken == false)
            {
              state = S;
            }
            //tu medzera je na zaciatku riadku
            else
            {
              _NumberOfSpaces++;
            }
            
            //symbol.data.int_data ++;
          }
          
          state = S;
          break;
        } else if (character == ','){ 
            symbol.type = _comma;
            return symbol;
          break;
        } else if(character == EOF) {
          state = F;
          break;
        } else {
          state = Fx;
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
      case Q2: { // """ """ multiline comment, quote - second part 
        character = getchar();
        if(character == '"') { // Q3
          state = Q3;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q3: { // multiline comment, quote - third part
        character = getchar();
        if(character == '"') { 
          state = Q4;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q4: { // multiline comment, quote end - first part
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
      case Q5: { // multiline comment, quote end - second part
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
      case Q6: { // multiline comment, quote end - third part
        character = getchar();
        if(character == '"') { 
          state = S;
          break;
        } else if (character == EOF){
          state = EOF;
          break;
        } else {
          state = Q4;
          break;
        }
      }
      case Q7: { // state for incoming identifs, keywords, functions
        character = getchar();
        if (character == '_' || isalnum(character)){
          sprintf(help, "%c", character);
          strcat(buffer, help);
          break;
        } else {
          strcpy(symbol.data.str_data,buffer);
          ungetc(character, input);
          state = Q8;
          break;
        }
      }
      case Q8: { // compare keywords
        for (int i = 0; i < KEYWORD_COUNT; i++){
          if (strcmp(buffer, keywords[i]) == 0){
            symbol.type = _keyword;
            return symbol;
          }
        } // compare functions
        for (int i = 0; i < FUNCTION_COUNT; i++){
          if (strcmp(buffer, functions[i]) == 0){
            symbol.type = _func;
            return symbol;
          }
        } // everything else is identifikator = variable
        symbol.type = _id;
        return symbol;
      }
      case Q22: { //operator for division
        character = getchar();
        if(character == '/') { // wholenumber division
          symbol.type = _wholenumber_division;
          strcpy(symbol.data.str_data,"//");
        } else {
          ungetc(character, input); // simple division
          symbol.type = _division;
           strcpy(symbol.data.str_data,"/");
        }
        //symbol.type = _operator;
        return symbol;
      }
      case Q23: { // operator less or eq
        character = getchar();
        if(character == '=') {
          symbol.type = _less_or_equal;
          strcpy(symbol.data.str_data,"<=");
        } else {
          ungetc(character, input);
          symbol.type = _less;
          //symbol.data.str_data = "<";
        }
        //symbol.type = _operator;
        return symbol;
      }
      case Q24: { // operator greater than or eq
        character = getchar();
        if(character == '=') {
          symbol.type = _greater_or_equal;
          strcpy(symbol.data.str_data,">=");
        } else {
          ungetc(character, input);
          symbol.type = _greater;
          strcpy(symbol.data.str_data,">");
        }
        //symbol.type = _operator;
        return symbol;
      }
      case Q25: { // equal or assignment
        character = getchar();
        if(character == '=') {
          symbol.type = _equal;
          strcpy(symbol.data.str_data,"==");
        } else {
          ungetc(character, input);
          symbol.type = _equal;
          strcpy(symbol.data.str_data,"=");
        }
        //symbol.type = _operator;
        return symbol;
      }
      case Q26: { // not equal
        character = getchar();
        if(character == '=') {
          symbol.type = _not_equal;
          strcpy(symbol.data.str_data,"!=");
          return symbol;
        } else { // exclamation mark alone is an error
          ungetc(character, input);
          state = Fx;
          break;
        }
      }
      case Q27: { // there has to be .ifjcode19
        for(int i = 0; i < 9; i++) {
          IFJbuffer[i] = getchar();
          if(IFJbuffer[i] == EOF) {
            state = Fx;
            break;
          }
        }
        IFJbuffer[9] = '\0'; // compare buffer to .IFJcode19
        if(strcmp(IFJbuffer, "IFJcode19") == 0) {
          symbol.type = _ifjcode;
          return symbol;
        } else {
          state = Fx;
          break;
        }
      }
      case Q16: { // main state for string
        character = getchar();
        if(character == '\'') {
          symbol.type = _string;
          strcpy(symbol.data.str_data,buffer);
          return symbol;
        } else if (character == '\\') {
          sprintf(help, "%c", character);
          strcat(buffer, help);
          state = Q17;
          break;
        } else if(character == EOF || character == EOL) {
          state = Fx;
          break;
        } else { 
          sprintf(help, "%c", character);
          strcat(buffer, help);
          break;
        }
      }
      case Q17: { // hexa number - part /x
        character = getchar();
        if(character == 'x') {
          state = Q18;
          sprintf(help, "%c", character);
          strcat(buffer, help);
          break;
        } else if(character == EOL || character == EOF) {
          state = Fx;
          break;
          
        } 
        else if(character == '\"' || character == '\'')
        {
          sprintf(help, "%c", character);
          strcat(buffer, help);
          state = Q16;
          break;
        }
        else {
        state = Q16;
        break;
        }
      }
      case Q18: { // test if the first part is suitable for hexa number
        character = getchar(); 
        if(ishex(character)) {
          sprintf(help, "%c", character);
          strcat(buffer, help);
          state = Q19;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q19: { // test if the second part is suitable for hexa number
        character = getchar();
        if(ishex(character)) {
          sprintf(help, "%c", character);
          strcat(buffer, help);
          state = Q16;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q21: { // incoming character is 0 
        character = getchar();
        if(character == '.') {
          symbol.type = _double;
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
      case Q9: { // incoming character is a digit from 1 to 9
        character = getchar();
        if(character == '.') {
          symbol.type = _double;
          state = Q10;
          break;
        } else if(character == 'e' || character == 'E') {
          symbol.type = _double;
          symbol.data.dbl_data = (double) symbol.data.int_data;
          state = Q11;
          break;
        } else if(isdigit(character)) {
          symbol.data.int_data = intcat(symbol.data.int_data, asciiToNumber(character));
          break;
        } else {
          ungetc(character, input);
          return symbol;
        } 
      }
      case Q10: { // incoming character is a decimal point
        character = getchar();
        if(isdigit(character)) {
          decimal = intcat(decimal, asciiToNumber(character));
          state = Q14;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q14: { // after the decimal point came a number
        character = getchar();
        if (isdigit(character)) {
          decimal = intcat(decimal, asciiToNumber(character));
          break;
        } else if(character == 'e' || character == 'E') {
          symbol.data.dbl_data = combineDouble(symbol.data.int_data, decimal);
          state = Q11;
          break;
        } else {
          ungetc(character, input);
          symbol.data.dbl_data = combineDouble(symbol.data.int_data, decimal);
          return symbol;
        }
      }
      case Q11: { // after the number came 'e' or 'E'
        character = getchar();
        if(isdigit(character)) {
          exponent = asciiToNumber(character);
          state = Q12;
          break;
        } else if(character == '+' || character == '-') {
          exponent_sign = character;
          state = Q13;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q12: { // after 'e'/'E' came a number
        character = getchar();
        if(isdigit(character)) {
          exponent = intcat(exponent, asciiToNumber(character));
          break;
        } else if(isalpha(character) || character == '_') {
          state = Fx;
          break;
        } else {
          ungetc(character, input);
          symbol.data.dbl_data = myPow(symbol.data.dbl_data, exponent, exponent_sign);
          return symbol;
        }
      }
      case Q13: { // after 'e'/'E' came plus or minus sign
        character = getchar();
        if(isdigit(character)) {
          exponent = intcat(exponent, asciiToNumber(character));
          state = Q12;
          break;
        } else {
          state = Fx;
          break;
        }
      }
      case Q20: { // state for whitespace
        character = getchar();
        if(character == '#') {
          state = Q1;
          break;
        } else if (isspace(character)) {
          if (character == ' '){
            symbol.data.int_data ++;
          }
          break;
        } else if(character == EOL) {
          symbol.type = _eol;
          //symbol.data.str_data = "-";
          return symbol;
        } else {
          ungetc(character, input);
          return symbol;
        }
      }
      case F: { // eof state
        symbol.type = _eof;
        while_condition = false;
        break;
      }
      case Fx: {
        fprintf(stderr, "ERROR: wrong character input!\n");
        exit(LEXICAL_ERROR);
      }
    }
  }
  return symbol;
}


/*
** Function that checks if an incoming character might be a hexa number
** that means: is digit, or is 'a' to 'f', or is 'A' to 'F'
*/
bool ishex(int n) {
  if((n < 71 && n > 64) || (n < 103 && n > 96) || isdigit(n)) {
    return true;
  }
  return false;
}


/*
** Function for concatening int numbers
*/
int intcat(int number1, int number2){
  return (number1 * 10) + number2;
}


/*
** Function that translates ascii symbol to numbers in decimal notation
*/
int asciiToNumber(int character){
  return character - 48;
}

/*
** Function that helps me find out the length of a number
** found on: 
*/
int intlen(int number){
  if (number < 0)           return 0;
  if (number >= 1000000000) return 10;
  if (number >= 100000000)  return 9;
  if (number >= 10000000)   return 8;
  if (number >= 1000000)    return 7;
  if (number >= 100000)     return 6;
  if (number >= 10000)      return 5;
  if (number >= 1000)       return 4;
  if (number >= 100)        return 3;
  if (number >= 10)         return 2;
  return 1;
}

/*
** Function that calculates the exponential number
** this is not square!
*/
double myPow(double base, int exp, char sign){  

  if(sign == '+') {
    return base * (pow((double)10, (double)exp));
  }
  return base / (pow((double)10, (double)exp));
}

/*
** Function where you get from two int numbers a double
*/
double combineDouble(int whole, int decimal){
  double help = (double) intlen(decimal);
  return whole + (decimal * (pow((double)10, (help * (-1)))));
}

Symbol_t GenerateIndent( void * LexStack, char  actualChar)
{
  Symbol_t symbol;
  _IndentChar = actualChar;
  symbol.type = _indent;
  sPush(LexStack, 0, _NumberOfSpaces);
  _NumberOfSpaces = 0;
  return symbol;
}

Symbol_t GenerateDedent(void * LexStack, char  actualChar, int Top)
{
  int found = false;
  Symbol_t symbol;
  while(Top != _NumberOfSpaces)
  {
    sLexPop(LexStack);
    Top = sLexTop(LexStack);
    if(Top == _NumberOfSpaces) 
    {
      found = true;
      break;
    }
    else if (Top == 0)
    {
      found = false;
      _FirstToken = false;
      break;
    }
  }
  //ak sme nasli pocet medzier v stacku, generujeme dedent
  if(found)
  {
    _IndentChar = actualChar;
    symbol.type = _dedent;
    _NumberOfSpaces = 0;
  }
  else 
  {
    symbol.type =_null;
  }
  return symbol;
}