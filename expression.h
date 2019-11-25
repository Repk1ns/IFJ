#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include <stdbool.h>
#include <stdlib.h>
/*
typedef enum {
    SYMBOL_PLUS_MINUS,              // 0
    SYMBOL_MUL_DIV,                 // 1
    SYMBOL_RELATIONAL_OPERATOR,     // 2
    SYMBOL_LEFT_BRACKET,            // 3
    SYMBOL_RIGHT_BRACKET,           // 4
    SYMBOL_ID_INT_FLOAT_STRING,     // 5
    SYMBOL_DOLLAR,                  // 6
    SYMBOL_SHIFT,                   // 7 <
    SYMBOL_NTERMINAL                // NO Terminal - E
} Precedence_table_symbol;

typedef enum {
    RULE_ADD,               // E -> E + E
    RULE_SUB,               // E -> E - E
    RULE_MUL,               // E -> E * E
    RULE_DIV,               // E -> E / E
    RULE_W_DIV,             // E -> E // E
    RULE_EQUAL,             // E -> E = E
    RULE_NOT_EQUAL,         // E -> E != E
    RULE_LESS,              // E -> E < E
    RULE_LESS_OR_EQUAL,     // E -> E <= E
    RULE_GREATER,           // E -> E > E
    RULE_GREATER_OR_EQUAL,  // E -> E >= E
    RULE_LB_RB,             // E -> (E)
    RULE_TERMINAL,          // E -> i
    NO_RULE                 // RULE DOESN'T EXISTS
} Precedence_table_rule;*/

int Expression(Symbol_t);
int reduction();


#endif