/* IFJ project 2019/2020: IFJcode19
* expression.c
* Author = Vojtech Mimochodek (xmimoc01)
*/
#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "expression.h"
#include "stack.h"
#include "error_codes.h"
#include "generator.h"
#include "syntax_analysis.h"


/*
Globalni promenne pro nacitany token a stack
*/
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


/*
Vyhledani symbolu v tabulce
*/
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
        case _keyword:
            if (strcmp(Actual_Token.data.str_data, "None") != 0){
                return SYNTAX_ERROR;
            }
            return SYMBOL_ID_INT_FLOAT_STRING;
        default:
            return SYMBOL_DOLLAR;
    }
}
/*
Vyhledavani pravidla podle poctu operandu
*/
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
        // E -> (E)
        if(op1->stackSymbol == SYMBOL_RIGHT_BRACKET && op3->stackSymbol == SYMBOL_LEFT_BRACKET)
        {
            return RULE_LB_RB;
        }

        if(op1->stackSymbol == SYMBOL_NTERMINAL && op3->stackSymbol == SYMBOL_NTERMINAL)
        {
            switch(op2->intdata)
            {
                case _plus:
                    generateInstruction(I_ADDS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_ADD;
                case _minus:
                    generateInstruction(I_SUBS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_SUB;
                case _multiplication:
                    generateInstruction(I_MULS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_MUL;
                case _division:
                    generateInstruction(I_DIVS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_DIV;
                case _wholenumber_division:
                    generateInstruction(I_IDIVS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_W_DIV;
                case _equal:
                    generateInstruction(I_EQS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_EQUAL;
                case _not_equal:
                    generateInstruction(I_EQS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    generateInstruction(I_NOTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_NOT_EQUAL;
                case _less_or_equal:
                    generateInstruction(I_GTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    generateInstruction(I_NOTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_LESS_OR_EQUAL;
                case _greater_or_equal:
                    generateInstruction(I_LTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    generateInstruction(I_NOTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_GREATER_OR_EQUAL;
                case _less:
                    generateInstruction(I_LTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_LESS;
                case _greater:
                    generateInstruction(I_GTS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    return RULE_GREATER;
                default:
                    return NO_RULE;
            }
        }

        return NO_RULE;
    }

    return NO_RULE;
}
/*
Spocitani poctu symbolu na stacku po symbol SHIFT <
*/
int Items_to_pop()
{
    int counter = 0;
    tStackItem* pom = sTop(&Stack);
    while(pom != NULL)
    {
        if(pom->stackSymbol == SYMBOL_SHIFT)
        {
            return counter;
        }
        pom = pom->nextItem;
        counter++;
    }
    return counter;
}
/*
Funkce pro zredukovani podle pravidla
*/
int reduction()
{
    tStackItem* op1 = NULL;
    tStackItem* op2 = NULL;
    tStackItem* op3 = NULL;
    Precedence_table_rule rule;

    int count = Items_to_pop();


    if(count == 1) // nalezeni jednoho operandu - E -> i
    {
        op1 = Stack.topItem;
    }
    else if(count == 3) // nalezeni 3 operandu - vyhledam pravidla
    {
        op1 = Stack.topItem;
        op2 = Stack.topItem->nextItem;
        op3 = Stack.topItem->nextItem->nextItem;
    }
    else
    {
        return SYNTAX_ERROR;
    }

    rule = get_precedence_table_rule(count, op1, op2, op3); // vyhledani pravidel

    if(rule == NO_RULE)
    {
        return SYNTAX_ERROR;
    }
    else
    {
        int i = 0;
        while(i <= count) // popnuti symbolu ze stacku kolik jich bylo po symbol SHIFT <
        {
            sTopPop(&Stack);
            i++;
        }
        sPush(&Stack, SYMBOL_NTERMINAL, Actual_Token.type);
    }
    return IT_IS_OKAY;
}
int Expression(Symbol_t* token, bool preLoadToken, void *ST_global, void * ST_local, bool IsItDef)
{
   generateInstruction(I_CLEARS, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);

   bool null_division = 0;

   SymTableItem_t *item;

    if(preLoadToken == false)
    {
        Actual_Token = getNextSymbol(stdin,NULL);
        //skontrolujeme, ci sa nejedna o : tak je syntax error
        if(Actual_Token.type == _operator) return SYNTAX_ERROR;

    }
    else
    {
        Actual_Token = *token;
        if(Actual_Token.type == _eol || Actual_Token.type == _eof) return SYNTAX_ERROR;
    }



    //ak mame identifikator, skontrolujeme, ci je definovany
    if(Actual_Token.type == _id)
    {
        //ak sa nachadzame  v definici funkcie, tak hladame v lokalnej tabulke symbolov
        item = SemanticCheck(ST_global, ST_local, IsItDef);
        //ak sme nenasli, semanticka chyba, id nie je definovane
        if(item == NULL) return SEMANTIC_ERROR;
    }
    //printf("PRELOAD %d type: %d data: %s \n", preLoadToken,Actual_Token.type,Actual_Token.data.str_data);

    sInit(&Stack);

    int row, column;

    sPush(&Stack, SYMBOL_DOLLAR, Actual_Token.type);  // vlozeni $ na Stack

    do
    {
        row = (TopTerminal(&Stack))->stackSymbol; // Nejvrchnejsi terminal

        column = get_precedence_table_symbol(Actual_Token); // Symbol na vstupu

        switch(Precedence_table[row][column])
        {
            /* Stav K - Konec
            row = $ && column = $
            */
            case K:
                break;
            /* Stav X - Error
            pri chybe
            */
            case X:
                sDispose(&Stack);
                return SYNTAX_ERROR;
                break;
            /* Stav E - Equal (=)
            push symbolu ze vstupu na stack
            */
            case E:
                sPush(&Stack, column, Actual_Token.type);
                Actual_Token = getNextSymbol(stdin, NULL);
                if(Actual_Token.type == _division || Actual_Token.type == _wholenumber_division)
                {
                    null_division = 1;
                }
                else if(Actual_Token.data.int_data == '0')
                {
                    if(null_division == 1)
                    {
                        sDispose(&Stack);
                        return DIVBYZERO_ERROR;
                    }
                }
                else
                {
                    null_division = 0;
                }
                if(Actual_Token.type == _id)
                {
                    item = SemanticCheck(ST_global, ST_local, IsItDef);
                    //ak sme nenasli, semanticka chyba, id nie je definovane
                    if(item == NULL)
                    {
                        sDispose(&Stack);
                        return SEMANTIC_ERROR;
                    }
                }
                break;
            /*Stav P - Shift
            push nejvrchnějšího terminalu na stack
            push symbolu ze vstupu na stack
            */
            case P:

                //generovani kodu, pokud narazime ve vyrazu na operand, vlozime ho v 3AC na datovy zasobnik
                if(Actual_Token.type == _id)
                {
                    if(IsItDef)
                    {
                        generateInstruction(I_PUSHS, P_LF, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    }
                    else
                    {
                        generateInstruction(I_PUSHS, P_GF, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                    }
                }

                if(Actual_Token.type == _int)
                {
                    generateInstruction(I_PUSHS, P_INT, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                }

                if(Actual_Token.type == _double)
                {
                    generateInstruction(I_PUSHS, P_FLOAT, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                }

                if(Actual_Token.type == _string)
                {
                    generateInstruction(I_PUSHS, P_STRING, Actual_Token.data, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
                }

                push_TopTerminal(&Stack, SYMBOL_SHIFT, Actual_Token.type);
                sPush(&Stack, column, Actual_Token.type);
                Actual_Token = getNextSymbol(stdin, NULL);
                if(Actual_Token.type == _division || Actual_Token.type == _wholenumber_division)
                {
                    null_division = 1;
                }
                else if(Actual_Token.data.int_data == '0')
                {
                    if(null_division == 1)
                    {
                        sDispose(&Stack);
                        return DIVBYZERO_ERROR;
                    }
                }
                else
                {
                    null_division = 0;
                }
                if(Actual_Token.type == _id)
                {
                    item = SemanticCheck(ST_global, ST_local, IsItDef);
                    if(item == NULL)
                    {
                        sDispose(&Stack);
                        return SEMANTIC_ERROR;
                    }
                }
                break;

            /* Stav R - reduce
            vyhledani nejvrchnejsiho symbolu <
            umazani leve strany pravidla po symbol <
            vyhledani pravidel
            push prave strany pravidla
            */
            case R:
                if(reduction() != 0)
                {
                    sDispose(&Stack);
                    return SYNTAX_ERROR;
                }
                break;
            default:
                {
                sDispose(&Stack);
                return SYNTAX_ERROR;
                }
        }
    }while(!(row == SYMBOL_DOLLAR && column == SYMBOL_DOLLAR));


    sDispose(&Stack);
    *token = Actual_Token;
    union Data pomocna_dataExp;
    sprintf(pomocna_dataExp.str_data, "%%expret");
    generateInstruction(I_POPS, P_GF, pomocna_dataExp, P_NULL, Actual_Token.data, P_NULL, Actual_Token.data);
    return IT_IS_OKAY;
}

void * SemanticCheck(void *ST_global,void *ST_local, bool IsItDef)
{
    SymTableItem_t *item;
    if(IsItDef == true)
    {
        item = SymTableSearch(ST_local, Actual_Token.data.str_data, SIZE_OF_SYMTABLE_LOCAL);
        //skusime, ci nahodou nemame globalnu premennu
        if(item == NULL) item = SymTableSearch(ST_global, Actual_Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
    }
    else
    {
        item = SymTableSearch(ST_global, Actual_Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
    }
    return item;
    //ak sme nenasli, semanticka chyba, id nie je definovane
}
