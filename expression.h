#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
#define Table_size 7

typedef enum {
	P,		//	POSUN - SHIFT
	E,		//	EQUAL
	R,		//	REDUCE
	X,		//	FAULT - ERROR
    K      //  END
} Precedence_table_operation;

typedef enum PTS {
    SYMBOL_PLUS_MINUS,              // 0
    SYMBOL_MUL_DIV,                 // 1
    SYMBOL_RELATIONAL_OPERATOR,     // 2
    SYMBOL_LEFT_BRACKET,            // 3
    SYMBOL_RIGHT_BRACKET,           // 4
    SYMBOL_ID_INT_FLOAT_STRING,     // 5
    SYMBOL_DOLLAR,                  // 6
    SYMBOL_SHIFT,                   // 7 <
    SYMBOL_NTERMINAL                // 8 NO Terminal - E
} Precedence_table_symbol;

typedef enum {
    RULE_ADD,               //(0) E -> E + E  
    RULE_SUB,               //(1) E -> E - E
    RULE_MUL,               //(2) E -> E * E
    RULE_DIV,               //(3) E -> E / E
    RULE_W_DIV,             //(4) E -> E // E
    RULE_EQUAL,             //(5) E -> E = E
    RULE_NOT_EQUAL,         //(6) E -> E != E
    RULE_LESS,              //(7) E -> E < E
    RULE_LESS_OR_EQUAL,     //(8) E -> E <= E
    RULE_GREATER,           //(9) E -> E > E
    RULE_GREATER_OR_EQUAL,  //(10) E -> E >= E
    RULE_LB_RB,             //(11) E -> (E)
    RULE_TERMINAL,          //(12) E -> i
    NO_RULE                 //(13) RULE DOESN'T EXISTS
} Precedence_table_rule;




int Expression(Symbol_t* token, bool preLoadToken, void *ST_global,void *ST_local, bool IsItDef);
int reduction();
void * SemanticCheck(void *ST_global,void *ST_local, bool IsItDef);


#endif