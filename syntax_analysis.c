#include <stdio.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h"
#include "error_codes.h"


//globalna premmenna tokenu
Symbol_t Token;
// parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
// funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
int Parse(SymTable_t *ST)
{
    

    // do {
    //     Token = getNextSymbol(stdin);
    //     switch (Token.type){
    //     case _int:
    //     case _whitespace: {
    //         printf("type: %d, data: %d\n", Token.type, Token.data.int_data);
    //         break;
    //     }
    //     case _double: {
    //         printf("type: %d, data: %f\n", Token.type, Token.data.dbl_data);
    //         break;
    //     }
    //     default: {
    //         printf("type: %d, data: %s\n", Token.type, Token.data.str_data);
    //         break;
    //     }
    //     }
    //     printf("--------------------------------------\n");
    // } while (Token.type != _eof);


    //testing hash table:

    //inserting items
    Symbol_t testsymbol;

    testsymbol.data.str_data = "totojetest1";
    testsymbol.type =  6;
    SymTableInsert(ST, testsymbol);

    //serach for item
    bool found = SymTableSearch(ST, "totojetest2");
    printf("is item in table? :%d\n", found);
    printf("---------------------------------------------\n");
    //vysledok parseru
    int result;
    result = ProgRule();

    
    return result;
}

int ProgRule()
{
    int result = -1;
    //nacitanie pociatocneho tokenu
    Token = getNextSymbol(stdin);

    //ak je token definicia funckie, simulujem pravidlo DEF
    if(Token.type == _keyword && (strcmp(Token.data.str_data,"def") == 0) )
    {
        printf("DEF!\n");
       
        //DEF
    }
    if(Token.type == _eof)
    //alebo je token EOF tak koncim program
    {
        result = IT_IS_OKAY;
    }
    //inak simulujem pravidlo STAT
    else
    {
        //STAT
        result = StatRule();
    }
    
    return result;
}

int StatRule()
{
    int result =0;
    switch (Token.type)
    {
        //ak token je identifikator simulujem pravidlo ID
        case _id:
        /* code */
        break;
        //ak token je _func, simulujem pravidla pre vstavane funkcie
        case _func:
        /* code */
        break;
        //ak je token _keyword, simulujem pravidla pre keywordy
        case _keyword:
            result = KeywordsRule();
        break;
    default:
        result = SYNTAX_ERROR;
        break;
    }

    return result;
}

int IdRule()
{
    return IT_IS_OKAY;
}

int FuncRule()
{
    return IT_IS_OKAY;
}

int KeywordsRule()
{
    int result = 0;
    //simulujem pravidlo pre if
    if(strcmp(Token.data.str_data,"if") == 0)
    {
       result = IfRule();
    }
    //simulujem pravidlo pre while
    else if(strcmp(Token.data.str_data,"while") == 0)
    {

    }
    //simulujem pravidlo pre pass
    else if(strcmp(Token.data.str_data,"pass") == 0)
    {

    }
    //simulujem pravidlo pre None
    else if(strcmp(Token.data.str_data,"None") == 0)
    {

    }
    //inak chyba
    else
    {
        result = SYNTAX_ERROR;
    }
    
     
       

    return result;
}

//pravidlo pre if 
int IfRule()
{
    int result = 0;
    //teraz som v pravidle if, a viem, ze v tokene mam if, cize teraz volam PSA
    //ona vola getnextSYmbol() a ked skocni, vrati mi errorcode a nasledujuci token do pravidla, cize by to mala byt dvojbodka
    result = Expression_analysis();
    if(result!= IT_IS_OKAY) return result;
    
    //zavolame dalsi token a zisitme, ci tam je ":"
    Token = getNextSymbol(stdin); //->>>>> TOTO TU NEBUDE LEBO MI VRATI TOKEN PSA
    if(strcmp(Token.data.str_data, ":") == 0)
    {

        // je tu dvojbodka, zavolame dalsi token, co by mal byt eol
        Token = getNextSymbol(stdin);
        if(Token.type == _eol)
        {

            //ak tu je dvojbodka volame pravidlo indent
            result = IndentRule();

            if(result == IT_IS_OKAY)
            {

                //ak prisie indent, nasleduje znova sekvencia statementov
                result = StatRule();
                //ak je to v poriadku, osetrime dedent
                if(result == IT_IS_OKAY)
                {

                    result = DedentRule();

                    if(result == IT_IS_OKAY)
                    {
                        //volame else rule
                        result = ElseRule();

                    }
                    else
                    {
                        result = SYNTAX_ERROR;
                    }
                
                } 
                else
                {
                    result = SYNTAX_ERROR;
                }
                
            }
            else
            {
                result = SYNTAX_ERROR;
            }
        
        }
        else
        {
            result = SYNTAX_ERROR;
        }
        
    }
    else
    {
        result = SYNTAX_ERROR;
    }
    
    return result;
}

int ElseRule()
{

    int result = 0;
    //volame dalsi token
    Token = getNextSymbol(stdin);
    //ak else neexistuje
    //TODO
    return result;
}

int IndentRule()
{
    int result = 0;
    //Token = getNextSymbol(stdin);
    Token.type =_indent;
    Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci token indent
    if(Token.type == _indent)
    {
        result = IT_IS_OKAY;
    }
    else
    {
        result = SYNTAX_ERROR;
    }
    return result;
}


int DedentRule()
{
    int result = 0;
    //Token = getNextSymbol(stdin);
    Token.type =_dedent;
    Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci token dedent
    if(Token.type == _dedent)
    {
        result = IT_IS_OKAY;
    }
    else
    {
        result = SYNTAX_ERROR;
    }
    
    return result;
}



int Expression_analysis()
{
    return IT_IS_OKAY;
}