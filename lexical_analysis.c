/* IFJ project 2019/2020: IFJcode19
* lexical analysis.c
* Author = Martina Tuckova (xtucko00)
*          David Spavor (xspavo00)
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
bool _ZeroFlag = false;
int _NumberOfSpaces = 0;
int _NumberOfPops = 0;
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
  char hexa[3]; //pomocne pole pre hexa cislo
  char help[2];
  

  buffer[0] = '\0'; // clear buffer from mess 
 
// switch case for incoming characters
  while (while_condition) {
    switch (state) {
      case S: { // start position

        if(_NumberOfPops > 0) 
        {
          state = QDedent;
          break;
        }

        if (_IndentChar == '\0')
        {
          character = getchar();
          if(character != '0') _ZeroFlag = false;
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
            int Top = sLexTop(LexStack);
          if(_FirstToken)
          {
            
            _FirstToken = false;
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
                state = QDedent;
      
                break;
              }
              
            }
            else exit(INTERNAL_ERROR);

            //if(_NumberOfSpaces != 0) sPush(LexStack, 0, _NumberOfSpaces);
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
          _NumberOfSpaces = 0;
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
      case QDedent: { // # single line comment 
        
        symbol.type = _dedent;
        _NumberOfPops --;
        return symbol;
        break;
      }
      case Q1: { // # single line comment 
        character = getchar();
        if(character == EOF) { // F
          state = F;
          break;
        } else if(character == EOL) { // S
          _FirstToken = true;   
          _NumberOfSpaces = 0;       
          state = S;
          symbol.type = _eol;
          return symbol;
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
          //symbol.data.str_data = malloc(1024);
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
          //strcpy(symbol.data.str_data,"//");
        } else {
          ungetc(character, input); // simple division
          symbol.type = _division;
           //strcpy(symbol.data.str_data,"/");
        }
        //symbol.type = _operator;
        return symbol;
      }
      case Q23: { // operator less or eq
        character = getchar();
        if(character == '=') {
          symbol.type = _less_or_equal;
          //strcpy(symbol.data.str_data,"<=");
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
          //strcpy(symbol.data.str_data,">=");
        } else {
          ungetc(character, input);
          symbol.type = _greater;
          //strcpy(symbol.data.str_data,">");
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
          //strcpy(symbol.data.str_data,"=");
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
          //symbol.data.str_data = malloc(1024);
          strcpy(symbol.data.str_data,buffer);
          return symbol;
        } else if (character == '\\') {
          sprintf(help, "%c", character);
          strcat(buffer, help);
          state = Q17;
          break;
         } //ak sa jedna o medzeru, musim vypisat jeho ascii hodnotu
          else if(character == ' ') {
          //asci hodnota medzery
          strcat(buffer, "\\032");
          break;
          //escape pre mriezku
        } else if(character == '#') {
          strcat(buffer, "\\035");
          break;
        }  
        else if(character == EOF || character == EOL) {
          state = Fx;
          break;
        } else { 
          sprintf(help, "%c", character);
          strcat(buffer, help);
          break;
        }
      }
      case Q17: { // hexa number and new line - part /x 
        character = getchar();
        if(character == 'x') {
          state = Q18;
          break;
          //ak sa jedna o newline, priradime ascii hodnotu a vratime sa spat do stavu pre stringy
        } else if(character == 'n') {
          strcat(buffer, "010");
          state = Q16;
          break;
          //escape /t
        } else if(character == 't') {
          strcat(buffer, "009");
          state = Q16;
          break;
          //escape sekvence pre uvodzovku "
        } else if(character == '\"') {
          strcat(buffer, "034");
          state = Q16;
          break;
          //escape pre apostrof
        } else if(character == '\'') {
          strcat(buffer, "039");
          state = Q16;
        break;
        //escape pre backslash
        } else if(character == '\\') {
          strcat(buffer, "092");
          state = Q16;
          break;
        break;
        } else if(character == EOL || character == EOF) {
          state = Fx;
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
          //ulozenie prveho hexa cisla
          hexa[0] = character;
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
          //ulozenie druheho hexa cisla
          hexa[1] = character;
          hexa[2] = '\0';
          //convert hexadecimal to decimal
          int decimal = hexadecimalToDecimal(hexa);
          //osetrit vkladanie nuly, ked je cislo mensie ako 100
          if(decimal < 100)
          {
            strcat(buffer,"0");
          }
          //convert int do char*
          sprintf(hexa, "%d", decimal);
          //strcat for converted decimal number
          strcat(buffer,hexa);

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
          state = Q14;
          break;
        } else if(isalpha(character) || character == '_') {
          state = Fx;
          break;
        } else {
          ungetc(character, input);
          if(_ZeroFlag == false)
          {
            symbol.type = _int;
            symbol.data.int_data = 0;
          }
          else
          {
            state = Fx;
            break;
          }
          _ZeroFlag = true;
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
        } else if(character == '+' || character == '-' || 
                  character == '/' || character == '*' || 
                  character == '\n' || character == ' ' || 
                  character == '(' || character == ')'  ) {
          ungetc(character, input);
          symbol.data.dbl_data = combineDouble(symbol.data.int_data, decimal);
          return symbol;
        }
        else
        {
          state = Fx;
          break;
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
** found on: https://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c?rq=1 by Eamon Nerbonne
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
  while(true)
  {
    
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
    sLexPop(LexStack);
    _NumberOfPops++;
    Top = sLexTop(LexStack);
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


// Function to convert hexadecimal to decimal  + upravy pre podporu malych pismen
//source: https://www.geeksforgeeks.org/program-for-hexadecimal-to-decimal/
int hexadecimalToDecimal(char *hexVal) 
{    
    int len = strlen(hexVal); 
      
    // Initializing base value to 1, i.e 16^0 
    int base = 1; 
      
    int dec_val = 0; 
      
    // Extracting characters as digits from last character 
    for (int i=len-1; i>=0; i--) 
    {    
        // if character lies in '0'-'9', converting  
        // it to integral 0-9 by subtracting 48 from 
        // ASCII value. 
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
                  
            // incrementing base by power 
            base = base * 16; 
        } 
  
        // if character lies in 'A'-'F' , converting  
        // it to integral 10 - 15 by subtracting 55  
        // from ASCII value 
        else if ((hexVal[i]>='A' && hexVal[i]<='F')  ) 
        { 
            dec_val += (hexVal[i] - 55)*base; 
          
            // incrementing base by power 
            base = base*16; 
        } 
        else if (hexVal[i]>='a' && hexVal[i]<='f')
        {
           dec_val += (hexVal[i] - 87)*base; 
          
            // incrementing base by power 
            base = base*16; 
        }
    } 
      
    return dec_val; 
} 