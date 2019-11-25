#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"
#include "expression.h"
#include "stack.h"
#include "error_codes.h"



Symbol_t Actual_Token;
tStack Stack;


int Precedence_table[Table_size][Table_size] =
{
              //|+- |* /| r | ( | ) | i | $ |
                { R , P , R , P , R , P , R }, //       "+,-"
                { R , R , R , P , R , P , R }, //       "*, /, //"
                { P , P , X , P , R , P , R }, // r ->  "<, <=, >, >=, ==, !="
                { P , P , P , P , E , P , X }, //       "("
                { R , R , R , X , R , X , R }, //       ")"
                { R , R , R , X , R , X , R }, // i ->  "ID,INT,FLOAT,STRING"
                { P , P , P , P , X , P , K }  //       "$"
};



Precedence_table_symbol get_precedence_table_symbol(Symbol_t Actual_Token)
{
    switch(Actual_Token.type)
    {
        case _plus:
            return SYMBOL_PLUS_MINUS;
        case _minus:
            return SYMBOL_PLUS_MINUS;
        case _multiplication:
            return SYMBOL_MUL_DIV;
        case _division:
            return SYMBOL_MUL_DIV;
        case _wholenumber_division:
            return SYMBOL_MUL_DIV;

        case _equal:
            return SYMBOL_RELATIONAL_OPERATOR;
        case _not_equal:
            return SYMBOL_RELATIONAL_OPERATOR;
        case _less_or_equal:
            return SYMBOL_RELATIONAL_OPERATOR;
        case _greater_or_equal:
            return SYMBOL_RELATIONAL_OPERATOR;
        case _less:
            return SYMBOL_RELATIONAL_OPERATOR;
        case _greater:
            return SYMBOL_RELATIONAL_OPERATOR;

        case _left_bracket:
            return SYMBOL_LEFT_BRACKET;
        case _right_bracket:
            return SYMBOL_RIGHT_BRACKET;

        case _id:
            return SYMBOL_ID_INT_FLOAT_STRING;
        case _int:
            return SYMBOL_ID_INT_FLOAT_STRING;
        case _double:
            return SYMBOL_ID_INT_FLOAT_STRING;
        case _string:
            return SYMBOL_ID_INT_FLOAT_STRING;

        default:
            return SYMBOL_DOLLAR;
    }
}


Precedence_table_rule get_precedence_table_rule(int counter, tStackItem* op1, tStackItem* op2, tStackItem* op3)
{
    if(counter == 1)
    {
        Precedence_table_symbol pom = op1->stackSymbol;
        if(pom == SYMBOL_ID_INT_FLOAT_STRING)
        {
            return RULE_TERMINAL;
        }

        return NO_RULE;
    }

    if(counter == 3)
    {
        if(op1->stackSymbol == SYMBOL_RIGHT_BRACKET && op3->stackSymbol == SYMBOL_LEFT_BRACKET)
        {
            return RULE_LB_RB;
        }

        if(op1->stackSymbol == SYMBOL_NTERMINAL && op3->stackSymbol == SYMBOL_NTERMINAL)
        {
            switch(op2->typ)
            {
                case _plus:
                    return RULE_ADD;
                case _minus:
                    return RULE_SUB;
                case _multiplication:
                    return RULE_MUL;
                case _division:
                    return RULE_DIV;
                case _wholenumber_division:
                    return RULE_W_DIV;
                case _equal:
                    return RULE_EQUAL;
                case _not_equal:
                    return RULE_NOT_EQUAL;
                case _less_or_equal:
                    return RULE_LESS_OR_EQUAL;
                case _greater_or_equal:
                    return RULE_GREATER_OR_EQUAL;
                case _less:
                    return RULE_LESS;
                case _greater:
                    return RULE_GREATER;
                default:
                    return NO_RULE;
            }
        }

        return NO_RULE;   
    }

    return NO_RULE;
}

int reduction()
{
    tStackItem* op1 = NULL;
    tStackItem* op2 = NULL; 
    tStackItem* op3 = NULL;
    tStackItem* pom = sTop(&Stack);
    Precedence_table_rule rule;

    int count = 0;
    int found;

    while(pom != NULL)
    {
        if(pom->stackSymbol == SYMBOL_SHIFT)
        {
            found = 1;
            return count;
        }
        else
        {
            found = 0;
            count++;
        }
    pom = pom->nextItem;
    }

    if(found == 1 && count == 1)
    {
        op1 = Stack.topItem;
    }
    else if(found == 1 && count == 3)
    {
        op1 = Stack.topItem;
        op2 = Stack.topItem->nextItem;
        op3 = Stack.topItem->nextItem->nextItem;
    }
    else
    {
        return SYNTAX_ERROR;
    }

    rule = get_precedence_table_rule(count, op1, op2, op3);

    if(rule == NO_RULE)
    {
        return SYNTAX_ERROR;
    }
    else
    {
        int i = 0;
        while(i < count)
        {
            sTopPop(&Stack);
        }
        sPush(&Stack, SYMBOL_NTERMINAL, Actual_Token.type);
    }
    return IT_IS_OKAY;
}
int Expression(Symbol_t* token)
{

    Actual_Token = getNextSymbol(stdin,NULL);


    sInit(&Stack);

    int row, column;

    sPush(&Stack, SYMBOL_DOLLAR, Actual_Token.type);  // vlozeni $ na Stack
    

    do
    {
        row = (TopTerminal(&Stack))->stackSymbol;
        column = get_precedence_table_symbol(Actual_Token);

        switch(Precedence_table[row][column])
        {
            case K:
                break;
            case X:
                return SYNTAX_ERROR;
            case E:
                sPush(&Stack, column, Actual_Token.type);
                Actual_Token = getNextSymbol(stdin, NULL);
                break;

            case P:
                push_TopTerminal(&Stack, SYMBOL_SHIFT, Actual_Token.type);
                sPush(&Stack, column, Actual_Token.type);
                Actual_Token = getNextSymbol(stdin, NULL);
                break;


            case R:
                reduction();
                break;
            default:
                return SYNTAX_ERROR;
        }


    }while(row != SYMBOL_DOLLAR && column != SYMBOL_DOLLAR);

    sDispose(&Stack);
    *token = Actual_Token;
    return IT_IS_OKAY;

    
    /*Precedence_table_symbol vystup;  DEBUG
    vystup = get_precedence_table_symbol(Actual_Token);         -> DEBUG
    printf("vystup z expressionu je:  %d\n ", vystup);*/ 

}