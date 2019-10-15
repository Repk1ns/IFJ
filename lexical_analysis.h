/* IFJ project 2019/2020: IFJcode19
* Lexical analysis
* Author = Martina Tuckova (xtucko00)
*/

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS

#include <stdio.h>
#include <stdbool.h>

#define EOL '\n'
#define MAX_ID_LENGTH 1024

union Data {
  int int_data;
  char* str_data;
  double dbl_data;
};

enum Type {
  _int,
  _string,
  _none,
  _func,
  _keyword,
  _double,
  _id,
  _eof,
  _eol,
  _operator,
  _ifjcode,
  _whitespace,
};


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
  F,
  Fx,
};

#define KEYWORD_COUNT 7
#define FUNCTION_COUNT 8

struct Symbol getNextSymbol(FILE*);
int intcat(int, int);
int asciiToNumber(int);
double myPow(double, int, char);
int intlen(int);
double combineDouble(int, int);
bool ishex(int);


#endif



