/* IFJ project 2019/2020: IFJcode19
* lexical analysis.h
* Authors = Martina Tuckova (xtucko00)
*           David Spavor (xspavo00)
*/

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include <stdio.h>
#include <stdbool.h>


#define EOL '\n'
#define MAX_ID_LENGTH 10024

union Data {
  int int_data;
  char str_data[MAX_ID_LENGTH];
  double dbl_data;
};

enum Type {
  _int,                  //  0
  _string,               //  1
  _none,                 //  2
  _func,                 //  3
  _keyword,              //  4
  _double,               //  5
  _id,                   //  6
  _eof,                  //  7
  _eol,                  //  8
  _operator,             //  9
  _plus,                 //  10
  _minus,                //  11 
  _multiplication,       //  12
  _division,             //  13
  _wholenumber_division, //  14       
  _equal,                //  15
  _not_equal,            //  16
  _less_or_equal,        //  17
  _less,                 //  18
  _greater_or_equal,     //  19
  _greater,              //  20
  _left_bracket,         //  21
  _right_bracket,        //  22
  _ifjcode,              //  23
  _indent,               //  24
  _dedent,               //  25
  _comma,                //  26
  _null,                 //  27
};

typedef struct Symbol Symbol_t;
struct Symbol {
  enum Type type;
  union Data data;
};

enum State {
  S,
  Q1,
  Q2,
  Q3,
  Q4,
  Q5,
  Q6,
  Q7,
  Q8,
  Q9,
  Q10,
  Q11,
  Q12,
  Q13,
  Q14,
  Q15,
  Q16,
  Q17,
  Q18,
  Q19,
  Q20,
  Q21,
  Q22,
  Q23,
  Q24,
  Q25,
  Q26,
  Q27,
  QDedent,
  F,
  Fx,
};

#define KEYWORD_COUNT 7
#define FUNCTION_COUNT 8

Symbol_t getNextSymbol(FILE*, void *LexStack);
int intcat(int, int);
int asciiToNumber(int);
double myPow(double, int, char);
int intlen(int);
double combineDouble(int, int);
bool ishex(int);
Symbol_t GenerateIndent(void * LexStack, char actualChar);
Symbol_t GenerateDedent(void * LexStack, char actualChar, int Top);


#endif




