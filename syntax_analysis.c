/* IFJ project 2019/2020: IFJcode19
* syntax_analysis.c
* Authors = David Spavor (xspavo00)
*           Vojtech Jurka (xjurka08)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h"
#include "error_codes.h"
#include "stack.h"
#include "generator.h"
#include "expression.h"

//globalni seznam instrukci
tListOfInstr *_IL;
//globalna tabulka symbolov
SymTable_t *_ST = NULL;
//lokalna tabulka symbolov
SymTable_t *_STlocal = NULL;
//stack
tStack *LexStack;
//globalna premmenna _Tokenu
Symbol_t _Token;
Symbol_t _PreviousToken;


bool _ProgRuleCalled = false;
bool _IndentFlag = false;
bool _DefFlag = false;
int _ActualIndent = 0;
int _ActualNumberOfParams = 0;
int _NumberOfNotDefFun = 0;

union Data pomocna_data; //pomocna struktura pro generaci kodu
union Data pomocna_data1; //druha pomocna struktura pro generaci kodu
union Data pomocna_data2; //treti pomocna struktura pro generaci kodu
int condCounter = 0;
int elseCounter = 0;
int typCondCounter = 0;
bool inWhile = false;
bool firstDef = true;
bool firstStat = true;
bool ExpressionUsed = false;
tListItem *post; //globalni ukazatel na pozici v seznamu instrukci pro pripadne vkladani defvar mimo cyklus


// parse fukncia
// funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
int Parse(SymTable_t *ST, void *Stack, void *List)
{
    int Result = IT_IS_OKAY;
    _IL = List;
    LexStack = Stack;
    _ST = ST;

    // do {
    //     _Token = getNextSymbol(stdin, LexStack);
    //     switch (_Token.type){
    //     case _int:
    //     {
    //         printf("type: %d, data: %d\n", _Token.type, _Token.data.int_data);
    //         break;
    //     }
    //     case _double: {
    //         printf("type: %d, data: %f\n", _Token.type, _Token.data.dbl_data);
    //         break;
    //     }
    //     case _indent:
    //     case _dedent:
    //     {
    //         printf("type: %d\n", _Token.type);
    //         break;
    //     }
    //     default: {

    //         printf("type: %d, data: %s\n", _Token.type, _Token.data.str_data);
    //         break;
    //     }
    //     }
    //     printf("--------------------------------------\n");
    // } while (_Token.type != _eof);


    Result = ProgRule(Result);
    //ostala nam funkcia, ktora nebola definovana
    if(_NumberOfNotDefFun != 0) Result = SEMANTIC_ERROR;


    return Result;
}

int ProgRule(int Result)
{

    //nacitanie pociatocneho _Tokenu ak sme volame prvy krat prog rule
    if(!_ProgRuleCalled)
    {

        _Token = getNextSymbol(stdin, LexStack);
    }
    _ProgRuleCalled = false;

    //ak je _Token vstavana funkcia alebo _Token def
    if(_Token.type == _keyword && (strcmp(_Token.data.str_data,"def") == 0))
    {
        if(Result == IT_IS_OKAY)
        {
            if(firstDef == true)
            {
                sprintf(pomocna_data.str_data, "%%main");
                generateInstruction(I_JUMP, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                firstDef = false;
            }

            Result = DefRule(Result);
        }


        //zavolanie pravidla prog pre prejdenie celeho suboru az po eof, ale iba ak je vsetko v poriadku
        if(Result == IT_IS_OKAY)
        {
            Result = ProgRule(Result);
        }

    }
    if(_Token.type == _eof)
    //alebo je _Token EOF tak koncim program
    {
        if(firstStat && !firstDef)
        {
            sprintf(pomocna_data.str_data, "%%main");
            generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            firstStat = false;
        }
        Result = IT_IS_OKAY;
    }
    //inak simulujem pravidlo STAT
    else
    {
        //STAT
        if(Result == IT_IS_OKAY)
        {
            if(firstStat && !firstDef)
            {
                sprintf(pomocna_data.str_data, "%%main");
                generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                firstStat = false;
            }

            Result = StatRule(Result);
        }
        //zavolanie pravidla prog pre prejdenie celeho suboru az po eof, ale iba ak je vsetko v poriadku
        if(Result == IT_IS_OKAY)
        {
            Result = ProgRule(Result);
        }
    }

    return Result;
}

int StatRule(int Result)
{

    switch (_Token.type)
    {
        //ak _Token je identifikator simulujem pravidlo ID
        case _id:
            //ulozime si identifikator do tabulky symbolov
            //SymTableInsert(_ST,_Token, _IDvariable, ID_PARAMETERS );
            //SymTableItem_t* item =SymTableSearch(_ST, "dado");
            Result = IdRule(Result);

            if(Result == IT_IS_OKAY)
            {
                //ak som v indente tak tam mozu byt iba statementy
                if(_IndentFlag)
                {
                    _Token = getNextSymbol(stdin, LexStack);

                    if(_Token.type == _dedent)
                    {
                        _ActualIndent--;
                        if(_ActualIndent == 0) _IndentFlag = false;

                        break;
                    }
                    else if(_Token.type == _null)
                    {
                        Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        Result = StatRule(Result);

                    }

                }

            }
        break;
        //ak je _Token _func, simulujem pravidla pre vstavane funkcie
        case _func:
            Result = BuiltInFuncRule(Result);
            if(Result == IT_IS_OKAY)
            {
                //ak som v indente tak tam mozu byt iba statementy
                if(_IndentFlag)
                {
                     _Token = getNextSymbol(stdin, LexStack);

                    if(_Token.type == _dedent)
                    {
                        _ActualIndent--;
                        if(_ActualIndent == 0) _IndentFlag = false;

                        break;
                    }
                    else if(_Token.type == _null)
                    {
                        Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        Result = StatRule(Result);
                    }
                }
            }
        break;
        //ak je _Token _keyword, simulujem pravidla pre keywordy
        case _keyword:
            Result = KeywordsRule(Result);
            if(Result == IT_IS_OKAY )
            {
                if(_IndentFlag)
                {
                    _Token = getNextSymbol(stdin, LexStack);
                    if(_Token.type == _dedent)
                    {
                        _ActualIndent--;
                       if(_ActualIndent == 0)  _IndentFlag = false;

                        break;
                    }
                    else if(_Token.type == _null)
                    {
                        Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        Result = StatRule(Result);
                    }
                }
            }
        break;
        //ak je _Token eol, pokracujem dalej
        case _eol:
            // _Token = getNextSymbol(stdin, LexStack);
              if(_IndentFlag)
                {
                    _Token = getNextSymbol(stdin, LexStack);
                    if(_Token.type == _dedent)
                    {
                        _ActualIndent--;
                        Result = IT_IS_OKAY;
                        if(_ActualIndent == 0) _IndentFlag = false;


                        break;
                    }
                        else if(_Token.type == _null)
                    {
                        Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        Result = StatRule(Result);
                    }
                }
        break;
         case _eof:
         break;
    default:
        Result = SYNTAX_ERROR;
        break;
    }

    return Result;
}

int IdRule(int Result)
{
    SymTableItem_t* prevDefFun = NULL;
    Symbol_t identifier;
    //zapamatanie si identifikatora
    identifier.type = _Token.type;
    strcpy(identifier.data.str_data, _Token.data.str_data);
    Symbol_t identifier0 = identifier; //promenna pro zachovani tokenu pro generator (co bylo pred =)
    SymTableItem_t* globalDef = NULL;//pomocne promenne pro generator pro DEFVAR
    SymTableItem_t* localDef = NULL;

    //ak nie sme v definicii funkcie ulozime si indetifikator do globanej tabulky symbolov ako typ premennej
    if(_DefFlag == false)
    {
        //musime si osetrit, ci nahodou sa nejedna o redefiniciu funkcie
        prevDefFun = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);

        globalDef = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);//zjisteni zda jiz byla promenna v programu deklarovana globalne

        //ak sme nasli

    }
    //teraz sme v definici funkcie, cize pouzivame lokalnu tabulky symbolov
    else
    {
        localDef = SymTableSearch(_STlocal, _Token.data.str_data, SIZE_OF_SYMTABLE_LOCAL);//zjisteni zda jiz byla promenna v programu deklarovana lokalne

        SymTableInsert(_STlocal,_Token,_IDvariable, ID_PARAMETERS, NO_PARAM, SIZE_OF_SYMTABLE_LOCAL);
        //osetrenie aby sme nedefinovali uz definovanu globalnu premennu
        globalDef = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
        if(globalDef != NULL) 
        {
            if(globalDef->Type != _IDfunction)
            {
                Result = SEMANTIC_ERROR; // nastava redefinicia globalnej premennej
            }
        }
    }

    if(Result == IT_IS_OKAY)
    {
        //dalsi symbol musi byt rovna sa
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _equal)
        {
        //osetrime, ci sa nejedna o redefiniciu funkcie
            if(_DefFlag == false)
            {
                if(prevDefFun!= NULL)
                {
                    //ak identifikator je typ funkcie, jedna sa o redefiniciou a je to error
                    if(prevDefFun->Type == _IDfunction) Result = SEMANTIC_ERROR;
                }
                if(Result == IT_IS_OKAY)
                {
                    SymTableInsert(_ST,identifier,_IDvariable, ID_PARAMETERS,NO_PARAM, SIZE_OF_SYMTABLE_GLOBAL);
                }
            }

            if(Result == IT_IS_OKAY)
            {
                //teraz sa musime rozhodnut, ci volame funckiu alebo priradzujeme vyraz -> prednacitame si jeden token
                _Token = getNextSymbol(stdin,LexStack);
                if(_Token.type == _id)
                {
                    identifier.type = _Token.type;
                    strcpy(identifier.data.str_data, _Token.data.str_data);
                }
                else
                {
                    identifier = _Token;
                }


                //teraz riesime globalny kontext
                if(_DefFlag == false)
                {
                    //vieme, ze ak volame funkciu, tak nasledujuci identifikator musi byt v TS a musi to byt typ funkcia
                    if(_Token.type == _id) prevDefFun = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                    //ak sme identifikator nasli a je uz predtym definovany ako _IDfunction tak sa jedna o volanie funkcie
                    if((prevDefFun != NULL) && (prevDefFun->Type == _IDfunction))
                    {
                        Result = FuncCallRule(Result,prevDefFun,false);

                        generateInstruction(I_CALL, P_LABEL, identifier.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                        if(globalDef == NULL)
                        {
                            if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                            {
                                tInstr I;

                                I.instType = I_DEFVAR;

                                I.prefix1 = P_GF;
                                I.data1 = identifier0.data;


                                I.prefix2 = P_NULL;
                                I.data2 = pomocna_data;

                                I.prefix3 = P_NULL;
                                I.data3 = pomocna_data;

                                listPostInsert(_IL, I, post);
                            }
                            else
                            {
                                generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                            }
                        }
                        sprintf(pomocna_data.str_data, "%%retval");
                        generateInstruction(I_MOVE, P_GF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);


                        _ActualNumberOfParams = 0;
                    }
                    //ak sme identifikator nasli ale je to typ _ID variable, volame PSA
                    else if ((prevDefFun != NULL) && (prevDefFun->Type == _IDvariable))
                    {

                        //PSA musite povedat, ze sme nacitali jeden jej token, cize ona jedno nacitanie tokenu preskoci
                        //tu sa bude musiet riesit semantika identifikatorov vo vyraze, ci su definovane
                        //ja mu predam pointer na tabulku symbolov, ci uz lokalnu alebo globalnu
                        if(_Token.type == _func)
                        {
                            Result = BuiltInFuncRule(Result);

                            if(globalDef == NULL)
                            {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_GF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                            }

                            sprintf(pomocna_data.str_data, "%%retval");
                            generateInstruction(I_MOVE, P_GF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);

                        }
                        else
                        {

                            if(ExpressionUsed == false)
                            {
                                sprintf(pomocna_data.str_data, "%%expret");

                                tInstr I;

                                I.instType = I_DEFVAR;

                                I.prefix1 = P_GF;
                                I.data1 = pomocna_data;


                                I.prefix2 = P_NULL;
                                I.data2 = pomocna_data;

                                I.prefix3 = P_NULL;
                                I.data3 = pomocna_data;

                                listPostInsert(_IL, I, NULL);

                                ExpressionUsed = true;
                            }

                            Result = Expression(&_Token, true, _ST, _STlocal, false, &typCondCounter);

                            if(globalDef == NULL)
                            {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_GF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                            }

                            sprintf(pomocna_data.str_data, "%%expret");
                            generateInstruction(I_MOVE, P_GF, identifier0.data, P_GF, pomocna_data, P_NULL, pomocna_data);

                        }
                        //skontrolujeme result, ako token mi ma vrati EOL
                    }
                    //ak sme token nenasli a je to identifikator jedna sa nedefinovanu funkciu/premennu
                    else if (prevDefFun == NULL && _Token.type == _id)
                    {

                        Result = SEMANTIC_ERROR;

                    }
                    //ak token je vstavana funckia, volame pravidlo pre vstavane funckie
                    else if(identifier.type == _func)
                    {
                        prevDefFun = NULL;
                        Result = BuiltInFuncRule(Result);

                        if(globalDef == NULL)
                        {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_GF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                        }

                        sprintf(pomocna_data.str_data, "%%retval");
                        generateInstruction(I_MOVE, P_GF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);


                    }
                    //inak proste volame PSA a ta nam povie co je zle
                    else
                    {

                        if(ExpressionUsed == false)
                        {
                            sprintf(pomocna_data.str_data, "%%expret");

                            tInstr I;

                            I.instType = I_DEFVAR;

                            I.prefix1 = P_GF;
                            I.data1 = pomocna_data;


                            I.prefix2 = P_NULL;
                            I.data2 = pomocna_data;

                            I.prefix3 = P_NULL;
                            I.data3 = pomocna_data;

                            listPostInsert(_IL, I, NULL);

                            ExpressionUsed = true;
                        }

                        Result = Expression(&_Token, true, _ST,_STlocal, false, &typCondCounter);

                        if(globalDef == NULL)
                        {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_GF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                        }

                        sprintf(pomocna_data.str_data, "%%expret");
                        generateInstruction(I_MOVE, P_GF, identifier0.data, P_GF, pomocna_data, P_NULL, pomocna_data);

                    }


                }
                //teraz sme v lokalnom kontexte
                else
                {
                    //hladame v lokalnej tabulke symbolov
                    prevDefFun = SymTableSearch(_STlocal, identifier.data.str_data, SIZE_OF_SYMTABLE_LOCAL);
                    //ak sme identifikator nasli ale je to typ _ID variable alebo _ID_parameter, volame PSA
                    if ((prevDefFun != NULL) && ((prevDefFun->Type == _IDvariable) || prevDefFun->Type == _IDparameter))
                    {

                        if(ExpressionUsed == false)
                        {
                            sprintf(pomocna_data.str_data, "%%expret");

                            tInstr I;

                            I.instType = I_DEFVAR;

                            I.prefix1 = P_GF;
                            I.data1 = pomocna_data;


                            I.prefix2 = P_NULL;
                            I.data2 = pomocna_data;

                            I.prefix3 = P_NULL;
                            I.data3 = pomocna_data;

                            listPostInsert(_IL, I, NULL);

                            ExpressionUsed = true;
                        }

                        //musime povedat, ze sme prednacitali jeden token
                        Result = Expression(&_Token,true,_ST, _STlocal, true, &typCondCounter);

                        if(localDef == NULL)
                        {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_LF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_LF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }

                        }

                        sprintf(pomocna_data.str_data, "%%expret");
                        generateInstruction(I_MOVE, P_LF, identifier0.data, P_GF, pomocna_data, P_NULL, pomocna_data);

                    }
                    //ak token je vstavana funckia, volame pravidlo pre vstavane funkcie
                    else if(identifier.type == _func)
                    {
                        Result = BuiltInFuncRule(Result);

                        if(localDef == NULL)
                        {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_LF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_LF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                        }

                        sprintf(pomocna_data.str_data, "%%retval");
                        generateInstruction(I_MOVE, P_LF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);

                    }
                    //identifikator sme nenasli
                    else
                    {
                        //skusime ho vyhladat v globalnej tabulke symbolov, ci sa nahodou nejedna o volanie funkcie pre funkciu, ktora uz je definovana
                        prevDefFun = SymTableSearch(_ST, identifier.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                        //ak sme nasli, jedna sa o volanie funkcie, porovname pocet parametrov
                        if(prevDefFun != NULL)
                        {
                            //identifikator sme nasli
                            //ak je to typ funkcia, volame pravidlo pre volanie funkcii
                            if(prevDefFun->Type == _IDfunction)
                            {
                                Result = FuncCallRule(Result,prevDefFun,false);

                                generateInstruction(I_CALL, P_LABEL, identifier.data, P_NULL, pomocna_data, P_NULL, pomocna_data);

                                if(localDef == NULL)
                                {
                                    if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                    {
                                        tInstr I;

                                        I.instType = I_DEFVAR;

                                        I.prefix1 = P_LF;
                                        I.data1 = identifier0.data;


                                        I.prefix2 = P_NULL;
                                        I.data2 = pomocna_data;

                                        I.prefix3 = P_NULL;
                                        I.data3 = pomocna_data;

                                        listPostInsert(_IL, I, post);
                                    }
                                    else
                                    {
                                        generateInstruction(I_DEFVAR, P_LF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                    }
                                }

                                sprintf(pomocna_data.str_data, "%%retval");
                                generateInstruction(I_MOVE, P_LF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);

                                if(Result == IT_IS_OKAY)
                                {
                                    //ak sa pocet parametrov nerovna jedna sa o chybu poctu parametrov
                                    if(prevDefFun->NumberOfParameters != _ActualNumberOfParams) Result = SEMANTIC_PARAMS_ERROR;
                                }
                                //vynulujeme pocet aktualnych parametrov
                                _ActualNumberOfParams = 0;
                            }
                            //inak je to globalna premenna, tak volame PSA
                            else Result = Expression(&_Token, true,_ST, _STlocal, true, &typCondCounter);

                            if(ExpressionUsed == false)
                            {
                                sprintf(pomocna_data.str_data, "%%expret");

                                tInstr I;

                                I.instType = I_DEFVAR;

                                I.prefix1 = P_GF;
                                I.data1 = pomocna_data;


                                I.prefix2 = P_NULL;
                                I.data2 = pomocna_data;

                                I.prefix3 = P_NULL;
                                I.data3 = pomocna_data;

                                listPostInsert(_IL, I, NULL);
                                ExpressionUsed = true;
                            }



                            if(globalDef == NULL)
                            {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_GF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_GF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                            }

                            sprintf(pomocna_data.str_data, "%%expret");
                            generateInstruction(I_MOVE, P_GF, identifier0.data, P_GF, pomocna_data, P_NULL, pomocna_data);

                        }
                        //ak je token identifikator, teraz predpokladame, ze sa jedna o volanie funkcie, ak sa nejedna, urcite to je chyba
                        else if(_Token.type == _id)
                        {
                            //ulozime si identifikator volanej funkcie
                            Symbol_t idCallFun;
                            idCallFun.type = _Token.type;
                            strcpy(idCallFun.data.str_data,_Token.data.str_data);
                            //osetrime syntax volania
                            Result = FuncCallRule(Result,NULL,false);

                            generateInstruction(I_CALL, P_LABEL, idCallFun.data, P_NULL, pomocna_data, P_NULL, pomocna_data);

                            if(localDef == NULL)
                            {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_LF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_LF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                            }

                            sprintf(pomocna_data.str_data, "%%retval");
                            generateInstruction(I_MOVE, P_LF, identifier0.data, P_TF, pomocna_data, P_NULL, pomocna_data);

                            //ak je vsetko v poriadku tak si ulozime volanu funkciu do tabulky symbolov pre pozdejsie porovnanie poctu parametrov
                            if(Result == IT_IS_OKAY)
                            {
                                SymTableInsert(_ST, idCallFun, _IDfunction, _ActualNumberOfParams,NO_PARAM, SIZE_OF_SYMTABLE_GLOBAL);
                                //pripocitame si pocet kontrol
                                _NumberOfNotDefFun++;
                            }
                            //vynulujeme a pokracujeme dalej
                            _ActualNumberOfParams = 0;

                        }
                        //token nie je identifikator a nenasli sme ho, volame PSA a ta nam povie ako sme na tom
                        else
                        {
                            //musime povedat, ze sme prednacitali jeden token

                            if(ExpressionUsed == false)
                            {
                                sprintf(pomocna_data.str_data, "%%expret");

                                tInstr I;

                                I.instType = I_DEFVAR;

                                I.prefix1 = P_GF;
                                I.data1 = pomocna_data;


                                I.prefix2 = P_NULL;
                                I.data2 = pomocna_data;

                                I.prefix3 = P_NULL;
                                I.data3 = pomocna_data;

                                listPostInsert(_IL, I, NULL);
                                ExpressionUsed = true;
                            }

                            Result = Expression(&_Token, true,_ST, _STlocal, true, &typCondCounter);

                            if(localDef == NULL)
                            {
                                if(inWhile) //pripad kdyz jsem v cyklu while, definuju promennou mimo cyklus
                                {
                                    tInstr I;

                                    I.instType = I_DEFVAR;

                                    I.prefix1 = P_LF;
                                    I.data1 = identifier0.data;


                                    I.prefix2 = P_NULL;
                                    I.data2 = pomocna_data;

                                    I.prefix3 = P_NULL;
                                    I.data3 = pomocna_data;

                                    listPostInsert(_IL, I, post);
                                }
                                else
                                {
                                    generateInstruction(I_DEFVAR, P_LF, identifier0.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                                }
                            }

                            sprintf(pomocna_data.str_data, "%%expret");
                            generateInstruction(I_MOVE, P_LF, identifier0.data, P_GF, pomocna_data, P_NULL, pomocna_data);
                        }



                    }
                }
            }


        }
        else
        {
            //jedna sa o volanie funkcie, potrebujeme ju vyhladat
            prevDefFun = SymTableSearch(_ST, identifier.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
            //TODO DOROBIT VOLANIE FUNKCIE
            //ak dalsi token je EOL mame nedefinovanu premmennu, inak je to SYNTAX ERROR

            //ak bola predtym definicia a plati, ze to bola funckia, jedna sa o volanie funkcie
            if(prevDefFun != NULL )
            {
                //ak sme v definicie funkcie, musime sa spytat, ci sa nahodou nejedna o globalnu premennu

                Result = FuncCallRule(Result,prevDefFun,true);

                generateInstruction(I_CALL, P_LABEL, identifier.data, P_NULL, pomocna_data, P_NULL, pomocna_data);

                if(Result == IT_IS_OKAY)
                {
                    //ak sa pocet parametrov nerovna jedna sa o chybu poctu parametrov
                    if(prevDefFun->NumberOfParameters != _ActualNumberOfParams) Result = SEMANTIC_PARAMS_ERROR;
                }
                //vynulujeme pocet aktualnych parametrov
                _ActualNumberOfParams = 0;




            }
            //volame funkciu avsak este nebola definovana
            else
            {
                //ak sme v definicii funkcie, ulozime si volanu funckie pre buduce osetrenie
                if(_DefFlag)
                {
                    Result = FuncCallRule(Result,NULL,true);

                    generateInstruction(I_CALL, P_LABEL, identifier.data, P_NULL, pomocna_data, P_NULL, pomocna_data);

                    if(Result == IT_IS_OKAY)
                    {
                        SymTableInsert(_ST, identifier, _IDfunction, _ActualNumberOfParams,NO_PARAM, SIZE_OF_SYMTABLE_GLOBAL);
                        //pripocitame si pocet kontrol
                        _NumberOfNotDefFun++;
                    }
                }
                //sme v globalnom kontexte, volanie funkcie bez toho aby sa nasla je chyba
                else
                {
                    //zistenie, ci sa jedna o aky typ chyby sa jedna
                    _Token = getNextSymbol(stdin,LexStack);
                    if(_Token.type == _eol || _Token.type == _eof ||
                        _Token.type == _operator)
                        Result = SYNTAX_ERROR;
                    else Result = SEMANTIC_ERROR;
                }

            }





            //Result = IT_IS_OKAY;
        }
    }

    //printf("RESULT ID2 %d \n", _Result);
    return Result;
}

int FuncCallRule(int Result,SymTableItem_t *item, bool CalledAlone)
{
    //vynulujeme premennu s parametrami
    _ActualNumberOfParams = 0;
    //simulujeme pravidlo pre volanie funckie

    generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    //ocakavame ako dalsi token zatvorku
    if(!CalledAlone) _Token = getNextSymbol(stdin,LexStack);
    if(_Token.type == _left_bracket)
    {
        //osetrime parametre
        Result = ParamsRule(Result,INF_PARAMETERS);
        if(Result == IT_IS_OKAY)
        {
            //porovnavame parametre iba ak sme v globalnom kontexte
            if(_DefFlag == false && item != NULL)
            {
                //porovname pocet parametrov z predtym definovanou funkciou
                if(item->NumberOfParameters == _ActualNumberOfParams)
                {
                    //ako dalsi token ocakavme eol, ten nam musi vrati paramsrule
                    if(_Token.type == _eol)
                    {
                        Result = IT_IS_OKAY;
                    }
                }
                //semanticka chyba poctu parametrov
                else
                {
                    Result = SEMANTIC_PARAMS_ERROR;
                }
            }



        }


    }
    //neprisla zatvorka, syntax chyba
    else
    {

        Result = SYNTAX_ERROR;

    }

    return Result;
}



int BuiltInFuncRule(int Result)
{
    Result = SYNTAX_ERROR;


    if(strcmp(_Token.data.str_data, "inputs") == 0 ||
       strcmp(_Token.data.str_data, "inputi") == 0 ||
       strcmp(_Token.data.str_data, "inputf") == 0 )
    {
            generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            sprintf(pomocna_data.str_data, "%%retval");
            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);
            if(strcmp(_Token.data.str_data, "inputs") == 0)
            {
                sprintf(pomocna_data1.str_data, "string");
            }
            if(strcmp(_Token.data.str_data, "inputi") == 0)
            {
                sprintf(pomocna_data1.str_data, "int");
            }
            if(strcmp(_Token.data.str_data, "inputf") == 0)
            {
                sprintf(pomocna_data1.str_data, "float");
            }
            generateInstruction(I_READ, P_LF, pomocna_data, P_LABEL, pomocna_data1, P_NULL, pomocna_data);
            generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);


        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 0

            Result = ParamsRule(Result,0);
        }

    }
    else if(strcmp(_Token.data.str_data, "len") == 0 ||
            strcmp(_Token.data.str_data, "chr") == 0)
    {
        if(strcmp(_Token.data.str_data, "len") == 0)
        {
            _Token = getNextSymbol(stdin, LexStack);
            if(strcmp(_Token.data.str_data, "(") == 0)
            {
                //zavolame pravidlo pre parametre, vieme, ze ich ma byt 1

                generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);


                Result = ParamsRule(Result,1);
                _ActualNumberOfParams = 0;

                generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                sprintf(pomocna_data.str_data, "%%retval");
                generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);

                typCondCounter++;
                //kontrola typu (str)
                sprintf(pomocna_data.str_data, "%%typ");
                sprintf(pomocna_data1.str_data, "%%1");
                sprintf(pomocna_data2.str_data, "string");

                generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

                sprintf(pomocna_data1.str_data, "%%len%d", typCondCounter);
                generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
                pomocna_data.int_data = 4;
                generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);


                //samotna fce
                sprintf(pomocna_data.str_data, "%%retval");
                sprintf(pomocna_data1.str_data, "%%1");
                generateInstruction(I_STRLEN, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);
                generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            }


        }
        if(strcmp(_Token.data.str_data, "chr") == 0)
        {
            _Token = getNextSymbol(stdin, LexStack);
            if(strcmp(_Token.data.str_data, "(") == 0)
            {
                //zavolame pravidlo pre parametre, vieme, ze ich ma byt 1

                generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);


                Result = ParamsRule(Result,1);
                _ActualNumberOfParams = 0;

                generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                sprintf(pomocna_data.str_data, "%%retval");
                generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);

                typCondCounter++;
                //kontrola typu (int)
                sprintf(pomocna_data.str_data, "%%typ");
                sprintf(pomocna_data1.str_data, "%%1");
                sprintf(pomocna_data2.str_data, "int");

                generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

                sprintf(pomocna_data1.str_data, "%%chr%d", typCondCounter);
                generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
                pomocna_data.int_data = 4;
                generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);

                //samotna fce TODO kontrola intervalu 0-255
                sprintf(pomocna_data.str_data, "%%retval");
                sprintf(pomocna_data1.str_data, "%%1");
                generateInstruction(I_INT2CHAR, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);
                generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            }


        }

    }
    else if(strcmp(_Token.data.str_data, "ord") == 0 )
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 2

            Result = ParamsRule(Result,2);
            _ActualNumberOfParams = 0;

            generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            sprintf(pomocna_data.str_data, "%%retval");
            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);

            typCondCounter++;
            //kontrola typu (str)
            sprintf(pomocna_data.str_data, "%%typ");
            sprintf(pomocna_data1.str_data, "%%1");
            sprintf(pomocna_data2.str_data, "string");

            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%ord%d", typCondCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
            pomocna_data.int_data = 4;
            generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);

            typCondCounter++;
            //kontrola typu (int)
            sprintf(pomocna_data.str_data, "%%typ");
            sprintf(pomocna_data1.str_data, "%%2");
            sprintf(pomocna_data2.str_data, "int");


            generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%ord%d", typCondCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
            pomocna_data.int_data = 4;
            generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);


            //samotna fce TODO osetrit indexaci
            sprintf(pomocna_data.str_data, "%%retval");
            sprintf(pomocna_data1.str_data, "%%1");
            sprintf(pomocna_data2.str_data, "%%2");
            generateInstruction(I_STRI2INT, P_LF, pomocna_data, P_LF, pomocna_data1, P_LF, pomocna_data2);
            generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

        }
    }
    else if(strcmp(_Token.data.str_data, "substr") == 0 )
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 3

            Result = ParamsRule(Result,3);
            _ActualNumberOfParams = 0;

            generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            sprintf(pomocna_data.str_data, "%%retval");
            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);


            typCondCounter++;
            //kontrola typu (str )
            sprintf(pomocna_data.str_data, "%%typ");
            sprintf(pomocna_data1.str_data, "%%1");
            sprintf(pomocna_data2.str_data, "string");

            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%substr%d", typCondCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
            pomocna_data.int_data = 4;
            generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);

            typCondCounter++;
            //kontrola typu (int)
            sprintf(pomocna_data.str_data, "%%typ");
            sprintf(pomocna_data1.str_data, "%%2");
            sprintf(pomocna_data2.str_data, "int");

            generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%substr%d", typCondCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
            pomocna_data.int_data = 4;
            generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);

            typCondCounter++;
            //kontrola typu (int)
            sprintf(pomocna_data.str_data, "%%typ");
            sprintf(pomocna_data1.str_data, "%%3");
            sprintf(pomocna_data2.str_data, "int");

            generateInstruction(I_TYPE, P_LF, pomocna_data, P_LF, pomocna_data1, P_NULL, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%substr%d", typCondCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_LF, pomocna_data, P_STRING, pomocna_data2);
            pomocna_data.int_data = 4;
            generateInstruction(I_EXIT, P_INT, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);


            //samotna fce TODO kontrola rozsahu pole a presahu delky str
            //(specifickej while cyklus)

            condCounter++;

            sprintf(pomocna_data.str_data, "%%cond");
            if(condCounter == 1)
            {
                tInstr I;

                I.instType = I_DEFVAR;

                I.prefix1 = P_GF;
                I.data1 = pomocna_data;


                I.prefix2 = P_NULL;
                I.data2 = pomocna_data;

                I.prefix3 = P_NULL;
                I.data3 = pomocna_data;

                listPostInsert(_IL, I, NULL);
            }
            sprintf(pomocna_data.str_data, "%%buff");
            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

            sprintf(pomocna_data.str_data, "%%whileA%d", condCounter);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

            sprintf(pomocna_data.str_data, "%%cond");
            sprintf(pomocna_data1.str_data, "%%3");
            pomocna_data2.int_data = 0;
            generateInstruction(I_GT, P_GF, pomocna_data, P_LF, pomocna_data1, P_INT, pomocna_data2);


            generateInstruction(I_NOT, P_GF, pomocna_data, P_GF, pomocna_data, P_NULL, pomocna_data);
            sprintf(pomocna_data2.str_data, "true");
            sprintf(pomocna_data1.str_data, "%%whileB%d", condCounter);
            generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_GF, pomocna_data, P_BOOL, pomocna_data2);


            //telo while cyklu
            sprintf(pomocna_data.str_data, "%%buff");
            sprintf(pomocna_data1.str_data, "%%1");
            sprintf(pomocna_data2.str_data, "%%2");
            generateInstruction(I_GETCHAR, P_LF, pomocna_data, P_LF, pomocna_data1, P_LF, pomocna_data2);

            sprintf(pomocna_data1.str_data, "%%retval");
            generateInstruction(I_CONCAT, P_LF, pomocna_data1, P_LF, pomocna_data1, P_LF, pomocna_data);

            pomocna_data.int_data = 1;
            generateInstruction(I_ADD, P_LF, pomocna_data2, P_LF, pomocna_data2, P_INT, pomocna_data);

            sprintf(pomocna_data1.str_data, "%%3");
            generateInstruction(I_SUB, P_LF, pomocna_data1, P_LF, pomocna_data1, P_INT, pomocna_data);
            //telo while cyklu

            sprintf(pomocna_data2.str_data, "%%whileA%d", condCounter);
            sprintf(pomocna_data1.str_data, "%%whileB%d", condCounter);
            generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);






            generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

        }
    }
     else if(strcmp(_Token.data.str_data, "print") == 0 )
    {
        generateInstruction(I_CREATEFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);


        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            Result = ParamsRule(Result,INF_PARAMETERS);

            generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            sprintf(pomocna_data.str_data, "%%retval");
            generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);

            int i = 1;
            sprintf(pomocna_data.str_data, "%%%d", i);
            generateInstruction(I_WRITE, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

            for(i=2; i <= _ActualNumberOfParams; i++)
            {
                sprintf(pomocna_data1.str_data, "\\032");
                sprintf(pomocna_data.str_data, "%%%d", i);
                generateInstruction(I_WRITE, P_STRING, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_WRITE, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
            }
            sprintf(pomocna_data.str_data, "\\010");
            generateInstruction(I_WRITE, P_STRING, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

            _ActualNumberOfParams = 0;

            generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

        }
    }
    else
    {
        Result = SEMANTIC_ERROR;
    }

    return Result;

}

int DefRule(int Result)
{
    Result = SYNTAX_ERROR;
    _ActualNumberOfParams = 0;
    Symbol_t Identifier;

    _Token = getNextSymbol(stdin, LexStack);
    //ak to je identifikator
    if(_Token.type == _id)
    {
        //zapamatanie si identifikatora
        Identifier.type = _Token.type;
        //Identifier.data.str_data = malloc(1024);
        strcpy(Identifier.data.str_data, _Token.data.str_data);

        generateInstruction(I_LABEL, P_LABEL, Identifier.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
        generateInstruction(I_PUSHFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
        sprintf(pomocna_data.str_data, "%%retval");
        generateInstruction(I_DEFVAR, P_LF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
        generateInstruction(I_MOVE, P_LF, pomocna_data, P_NIL, pomocna_data, P_NULL, pomocna_data);

        _Token = getNextSymbol(stdin, LexStack);
        //musi byt zatvorka
        if(strcmp(_Token.data.str_data,"(") == 0)
        {
            //viem, ze som v definicii funkcie, cize vytvaram si lokalnu tabulku symbolov
            _STlocal = SymTableInit(SIZE_OF_SYMTABLE_LOCAL);
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            //musime si ich ukladat do tabulky symbolov
            Result = ParamsRule(Result,DEF_PARAMETERS);
            //printf("actual number of params: %d\n", _ActualNumberOfParams);

            //teraz musi byt dvojbodka
            if(_Token.type == _operator)
            {
                if(Result == IT_IS_OKAY)
                {
                    //musime osetrit, ci nie je redefinicia funkcie
                    SymTableItem_t *item = SymTableSearch(_ST, Identifier.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                    //ak sme nenasli identifikator, vsetko je v poriadku
                    if(item == NULL)
                    {
                        //vlozime si indetifikator do tabulky symbolov a zapamatame si pocet paramaetrov
                        SymTableInsert(_ST, Identifier, _IDfunction, _ActualNumberOfParams,NO_PARAM, SIZE_OF_SYMTABLE_GLOBAL);

                    }
                    //nasli sme identifikator, bud je redefinicia funkcie alebo sa jedna o volanu funkciu pred definiciou
                    else
                    {
                        //zistime, o so sa nam jedna
                        //ak sa jedna o funkciu,
                        if(item->Type == _IDfunction)
                        {
                            //musime osetrit pocet parametrov
                            if(item->NumberOfParameters == _ActualNumberOfParams)
                            {
                                //ak sedia pocet parametrov, vsekto je v poriadku a snnizime pocet volani nedefinovanych funckii
                                _NumberOfNotDefFun--;
                            }
                            else
                            {
                                Result = SEMANTIC_PARAMS_ERROR;
                            }


                        }
                        //inak to je redefinicia
                        else
                        {
                            Result = SEMANTIC_ERROR;
                        }
                        _ActualNumberOfParams = 0;
                    }
                    if(Result == IT_IS_OKAY)
                    {
                        _ActualNumberOfParams = 0;
                        _Token = getNextSymbol(stdin, LexStack);
                        //ak je vsetko v poriadku tak po dvojbodke ocavame _Token EOL
                        if(_Token.type == _eol)
                        {
                            //_Token = getNextSymbol(stdin, LexStack);
                            Result = IndentRule(Result);
                            //musi prist indent
                            if(_Token.type == _indent)
                            {
                                //som v indente, idu stat rule
                                _IndentFlag = true;
                                //som v definicii funkcie, moze tam byt aj return
                                _DefFlag = true;
                                _ActualIndent++;
                                //dasli _Token mozu byt statementy alebo return
                                _Token = getNextSymbol(stdin, LexStack);


                                Result = StatRule(Result);
                                //lokalnu TS uz nepotrebujeme
                                SymTableDelete(_STlocal);
                                _STlocal = NULL;

                                //a povieme, ze uz nie sme v definicii funkcie
                                _DefFlag = false;

                            }
                            generateInstruction(I_POPFRAME, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                            generateInstruction(I_RETURN, P_NULL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                        }
                    }

                }
            }
            else
            {
                Result = SYNTAX_ERROR;
            }
        }
    }

    //free(Identifier.data.str_data);
    return Result;
}

//pravidlo pre parametre vstavanych a aj novo definonavynch funkcii- osetruje pocet parametrov a rozhoduje aj o semnatike
int ParamsRule(int Result,int numberOfParams)
{

    //Result = SYNTAX_ERROR;
    _PreviousToken = _Token;
    _Token = getNextSymbol(stdin, LexStack);
    //char data[MAX_ID_LENGTH];


    if(_Token.type == _int ||_Token.type == _double || _Token.type == _string || _Token.type == _id )
    {
        //vieme, ze v definicii funkcie mozu byt parametre iba identifikatori
        if(numberOfParams == DEF_PARAMETERS)
        {
            if(_Token.type == _id)
            {
                //ak som v definicii funkci, musim su ulozit parametre funkcie do lokalnej tabulky symobolov
                //priradzujem aj kolky parameter to je a ze je to typ _IDparameter
                SymTableInsert(_STlocal, _Token, _IDparameter, ID_PARAMETERS, _ActualNumberOfParams, SIZE_OF_SYMTABLE_LOCAL);
                _ActualNumberOfParams++;

                sprintf(pomocna_data1.str_data, "%%%d", _ActualNumberOfParams);//vytvoreni stringu pro promennou parametru z docasneho ramce

                generateInstruction(I_DEFVAR, P_LF, _Token.data, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_MOVE, P_LF, _Token.data, P_LF, pomocna_data1, P_NULL, pomocna_data);

                Result = ParamsRule(Result,numberOfParams);
            }
            else
            {
                Result = SEMANTIC_OTHERS_ERROR;
            }
        }
        //nie som v definicii funkcie, moze tu byt hocico
        else
        {
            _ActualNumberOfParams++;

            sprintf(pomocna_data.str_data, "%%%d", _ActualNumberOfParams);//vytvoreni stringu pro promennou parametru z docasneho ramce
            generateInstruction(I_DEFVAR, P_TF, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);



            if(_Token.type == _int)
            {
                generateInstruction(I_MOVE, P_TF, pomocna_data, P_INT, _Token.data, P_NULL, pomocna_data);
            }

            if(_Token.type == _double)
            {
                generateInstruction(I_MOVE, P_TF, pomocna_data, P_FLOAT, _Token.data, P_NULL, pomocna_data);
            }

            if(_Token.type == _string)
            {
                generateInstruction(I_MOVE, P_TF, pomocna_data, P_STRING, _Token.data, P_NULL, pomocna_data);
            }

            if(_Token.type == _id)
            {
                if(SymTableSearch(_ST, (_Token.data.str_data), SIZE_OF_SYMTABLE_GLOBAL) != NULL)
                {
                    generateInstruction(I_MOVE, P_TF, pomocna_data, P_GF, _Token.data, P_NULL, pomocna_data);
                }
                else
                {
                    generateInstruction(I_MOVE, P_TF, pomocna_data, P_LF, _Token.data, P_NULL, pomocna_data);
                }
            }

            Result = ParamsRule(Result,numberOfParams);
        }

    }
    else if(strcmp(_Token.data.str_data, ")") == 0)
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol || _Token.type == _eof )
        {
            if((_ActualNumberOfParams == numberOfParams) && (_PreviousToken.type != _comma))
            {
                Result = IT_IS_OKAY;
            }
            else if  ((numberOfParams == INF_PARAMETERS) && (_PreviousToken.type != _comma))
            {
                Result = IT_IS_OKAY;
            }
            else if  ((numberOfParams == DEF_PARAMETERS) && (_PreviousToken.type != _comma))
            {
                Result = IT_IS_OKAY;
            }

            else
            {
                if(_PreviousToken.type == _comma)
                {
                    Result = SYNTAX_ERROR;
                }
                else Result = SEMANTIC_PARAMS_ERROR;
            }
        }
        else if((strcmp(_Token.data.str_data, ":") == 0) && (numberOfParams == DEF_PARAMETERS) )
        {
            if(_PreviousToken.type == _comma)
            {
                Result = SYNTAX_ERROR;
            }
            else Result = IT_IS_OKAY;
        }

    }
    else if(_Token.type == _comma)
    {
        Result = ParamsRule(Result,numberOfParams);
    }
    else if((_Token.type == _keyword) && (strcmp(_Token.data.str_data, "None") == 0))
    {
        Result = SEMANTIC_EXPR_ERROR;
    }
    else
    {
        Result = SYNTAX_ERROR;
    }




    return Result;
}




int KeywordsRule(int Result)
{
    Result = 0;
    //simulujem pravidlo pre if
    if(strcmp(_Token.data.str_data,"if") == 0)
    {
       Result = IfRule(Result);
    }
    //simulujem pravidlo pre while
    else if(strcmp(_Token.data.str_data,"while") == 0)
    {
        if(ExpressionUsed == false)//defvar nesmim pouzivat v tele cyklu
        {
            sprintf(pomocna_data.str_data, "%%expret");

            tInstr I;

            I.instType = I_DEFVAR;

            I.prefix1 = P_GF;
            I.data1 = pomocna_data;


            I.prefix2 = P_NULL;
            I.data2 = pomocna_data;

            I.prefix3 = P_NULL;
            I.data3 = pomocna_data;

            listPostInsert(_IL, I, NULL);
            ExpressionUsed = true;
        }

        Result = WhileRule(Result);
    }
    //simulujem pravidlo pre pass a None
    else if(strcmp(_Token.data.str_data,"pass") == 0 || (strcmp(_Token.data.str_data,"None") == 0))
    {
        //ak je to None a nasledujci _Token je pass je vsetko v poriadku
        _Token = getNextSymbol(stdin, LexStack);
        //ak je _Token eof, to znamena ze pass je poslendy prikaz
        if(_Token.type == _eol || _Token.type == _eof)
        {
            Result = IT_IS_OKAY;
        }
        else
        {
            Result = SYNTAX_ERROR;
        }

    }
    else if(strcmp(_Token.data.str_data,"return") == 0)
    {
        //otestujeme, ci volame return v definicii funkcie
        if(_DefFlag == true && _IndentFlag == true)
        {

            if(ExpressionUsed == false)
            {
                sprintf(pomocna_data.str_data, "%%expret");

                tInstr I;

                I.instType = I_DEFVAR;

                I.prefix1 = P_GF;
                I.data1 = pomocna_data;


                I.prefix2 = P_NULL;
                I.data2 = pomocna_data;

                I.prefix3 = P_NULL;
                I.data3 = pomocna_data;

                listPostInsert(_IL, I, NULL);
                ExpressionUsed = true;
            }

            // po returne ocakavame identifikator
            // vieme, ze za returnom ocakavame vyraz, cize volame PSA
            //_Token = getNextSymbol(stdin, LexStack); //---> TOTO TU NEBUDE!
            Result = Expression(&_Token,false,_ST, _STlocal, true, &typCondCounter);

            sprintf(pomocna_data.str_data, "%%retval");
            sprintf(pomocna_data1.str_data, "%%expret");
            generateInstruction(I_MOVE, P_LF, pomocna_data, P_GF, pomocna_data1, P_NULL, pomocna_data);


        }
        else
        {
            Result = SYNTAX_ERROR;
        }

    }

    //inak chyba
    else
    {
        Result = SYNTAX_ERROR;
    }

    return Result;
}

//pravidlo pre if
int IfRule(int Result)
{
    condCounter++;

    if(ExpressionUsed == false)
    {
        sprintf(pomocna_data.str_data, "%%expret");

        tInstr I;

        I.instType = I_DEFVAR;

        I.prefix1 = P_GF;
        I.data1 = pomocna_data;


        I.prefix2 = P_NULL;
        I.data2 = pomocna_data;

        I.prefix3 = P_NULL;
        I.data3 = pomocna_data;

        listPostInsert(_IL, I, NULL);
        ExpressionUsed = true;
    }

    //teraz som v pravidle if, a viem, ze v _Tokene mam if, cize teraz volam PSA
    //ona vola getnextSYmbol() a ked skocni, vrati mi errorcode a nasledujuci _Token do pravidla, cize by to mala byt dvojbodka
    if (_DefFlag == true)
    {
        Result = Expression(&_Token,false, _ST, _STlocal, true, &typCondCounter);
    }
    else
    {
        Result = Expression(&_Token,false, _ST, _STlocal, false, &typCondCounter);
    }


    sprintf(pomocna_data.str_data, "%%cond");
    sprintf(pomocna_data1.str_data, "%%expret");
    if(condCounter == 1)
    {
        tInstr I;

        I.instType = I_DEFVAR;

        I.prefix1 = P_GF;
        I.data1 = pomocna_data;


        I.prefix2 = P_NULL;
        I.data2 = pomocna_data;

        I.prefix3 = P_NULL;
        I.data3 = pomocna_data;

        listPostInsert(_IL, I, NULL);
    }
    generateInstruction(I_MOVE, P_GF, pomocna_data, P_GF, pomocna_data1, P_NULL, pomocna_data);

    //typova kontrola pro vyraz
    //zjisteni typu
    generateInstruction(I_TYPE, P_GF, pomocna_data1, P_GF, pomocna_data, P_NULL, pomocna_data);

    typCondCounter++;
    int konecNum = typCondCounter;

    //kdyz je %cond int
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "int");
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    sprintf(pomocna_data2.str_data, "%%cond");
    generateInstruction(I_INT2FLOAT, P_GF, pomocna_data2, P_GF, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter++;

    //kdyz je %cond string
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "string");
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    //kdyz je %cond ''
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    sprintf(pomocna_data1.str_data, "%%cond");
    char *str = "";
    sprintf(pomocna_data2.str_data, "%s", str);
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);
    //priradim false a jdu na konec
    sprintf(pomocna_data2.str_data, "false");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);
    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);

    //label ifu v ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
    //priradim true
    sprintf(pomocna_data2.str_data, "true");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter = typCondCounter +2;

    //kdyz neni %cond bool (ted uz musi byt float)
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "bool");
    generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    //kdyz je %cond 0.0
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    sprintf(pomocna_data1.str_data, "%%cond");
    pomocna_data2.dbl_data = 0.0;
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_FLOAT, pomocna_data2);
    //priradim false a jdu na konec
    sprintf(pomocna_data2.str_data, "false");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);
    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);

    //label ifu v ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
    //priradim true
    sprintf(pomocna_data2.str_data, "true");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter = typCondCounter +2;

    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);


    //jedeme dal
    sprintf(pomocna_data.str_data, "%%cond");
    generateInstruction(I_NOT, P_GF, pomocna_data, P_GF, pomocna_data, P_NULL, pomocna_data);
    sprintf(pomocna_data1.str_data, "%%if%d", condCounter);
    sprintf(pomocna_data2.str_data, "true");
    generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_GF, pomocna_data, P_BOOL, pomocna_data2);


    if(Result!= IT_IS_OKAY) return Result;

    //zavolame dalsi _Token a zisitme, ci tam je ":"
    //_Token = getNextSymbol(stdin, LexStack); //->>>>> TOTO TU NEBUDE LEBO MI VRATI _Token PSA
    if(strcmp(_Token.data.str_data, ":") == 0)
    {

        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol)
        {

            //ak tu je dvojbodka volame pravidlo indent
            Result = IndentRule(Result);

            if(Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                _ActualIndent++;

                _Token = getNextSymbol(stdin, LexStack);
                //ak prisie indent, nasleduje znova sekvencia statementov
                Result = StatRule(Result);
                //ak je to v poriadku, osetrime dedent
                if(Result == IT_IS_OKAY)
                {
                    //volame else rule
                    Result = ElseRule(Result);
                }


            }

        }

    }
    else
    {
        Result = SYNTAX_ERROR;

    }

    return Result;
}

int ElseRule(int Result)
{

    Result = 0;
    //volame dalsi _Token
    _Token = getNextSymbol(stdin, LexStack);

    //ak to je else, simulujeme pravidlo pre else statemts
    if(strcmp(_Token.data.str_data, "else") == 0 )
    {
        elseCounter++;
        union Data elseData;
        sprintf(elseData.str_data, "%%else%d", elseCounter);
        sprintf(pomocna_data1.str_data, "%%if%d", condCounter);
        generateInstruction(I_JUMP, P_LABEL, elseData, P_NULL, pomocna_data, P_NULL, pomocna_data);
        generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);

        //nacitame dalsi _Token, co musi byt :
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, ":") == 0  )
        {
            // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
            _Token = getNextSymbol(stdin, LexStack);
            if(_Token.type == _eol)
            {
                //ak je to eol zavolame dalsi _Token a riesime statementy
                //_Token = getNextSymbol(stdin, LexStack);
                //ak tu je dvojbodka volame pravidlo indent
                Result = IndentRule(Result);

                if(Result == IT_IS_OKAY)
                {
                    _IndentFlag = true;
                    _ActualIndent++;
                    //ak prisie indent, nasleduje znova sekvencia statementov
                    _Token = getNextSymbol(stdin, LexStack);

                    Result = StatRule(Result);


                    generateInstruction(I_LABEL, P_LABEL, elseData, P_NULL, pomocna_data, P_NULL, pomocna_data);

                }
            }

        }
        else
        {
            Result = SYNTAX_ERROR;
        }

    }
    //ak else neexistuje, znova pokracuju pravidla programu
    else
    {
        Result = IT_IS_OKAY;
        _ProgRuleCalled = true;
        //nastavime si actualny _Token, aby sme v dalsom pravidle necitali dalej
        sprintf(pomocna_data1.str_data, "%%if%d", condCounter);
        generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);
    }
    //TODO
    return Result;
}

int IndentRule(int Result)
{

    _Token = getNextSymbol(stdin, LexStack);
    // _Token.type =_indent;
    // _Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci _Token indent
    if(_Token.type == _indent)
    {
        Result = IT_IS_OKAY;
    }
    //ak prisiel eol je to korektne a znova ocakavame indent
    else if(_Token.type == _eol)
    {
        Result = IndentRule(Result);
    }
    else
    {
        Result = SYNTAX_ERROR;
    }
    return Result;
}


int DedentRule(int Result)
{

    //_Token = getNextSymbol(stdin, LexStack);
    _Token.type =_dedent;
    _Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci _Token dedent
    if(_Token.type == _dedent)
    {
        Result = IT_IS_OKAY;
    }
    else
    {
        Result = SYNTAX_ERROR;
    }

    return Result;
}

int WhileRule(int Result)
{
    condCounter++;
    inWhile = true;
    post = _IL->last;//globalni ukazatel na pozici v seznamu instrukci pro pripadne vkladani defvar mimo cyklus

    sprintf(pomocna_data.str_data, "%%cond");
    if(condCounter == 1)
    {
        tInstr I;

        I.instType = I_DEFVAR;

        I.prefix1 = P_GF;
        I.data1 = pomocna_data;


        I.prefix2 = P_NULL;
        I.data2 = pomocna_data;

        I.prefix3 = P_NULL;
        I.data3 = pomocna_data;

        listPostInsert(_IL, I, NULL);
    }

    sprintf(pomocna_data.str_data, "%%whileA%d", condCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);


    Result = SYNTAX_ERROR;



    //zavolame PSA
    if (_DefFlag == true)
    {
        Result = Expression(&_Token,false, _ST, _STlocal, true, &typCondCounter);
    }
    else
    {
        Result = Expression(&_Token,false, _ST, _STlocal, false, &typCondCounter);
    }

    sprintf(pomocna_data.str_data, "%%cond");
    sprintf(pomocna_data1.str_data, "%%expret");

    generateInstruction(I_MOVE, P_GF, pomocna_data, P_GF, pomocna_data1, P_NULL, pomocna_data);

    //typova kontrola pro vyraz
    //zjisteni typu
    generateInstruction(I_TYPE, P_GF, pomocna_data1, P_GF, pomocna_data, P_NULL, pomocna_data);

    typCondCounter++;
    int konecNum = typCondCounter;

    //kdyz je %cond int
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "int");
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    sprintf(pomocna_data2.str_data, "%%cond");
    generateInstruction(I_INT2FLOAT, P_GF, pomocna_data2, P_GF, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter++;

    //kdyz je %cond string
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "string");
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    //kdyz je %cond ''
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    sprintf(pomocna_data1.str_data, "%%cond");
    char *str = "";
    sprintf(pomocna_data2.str_data, "%s", str);
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);
    //priradim false a jdu na konec
    sprintf(pomocna_data2.str_data, "false");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);
    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);

    //label ifu v ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
    //priradim true
    sprintf(pomocna_data2.str_data, "true");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter = typCondCounter +2;

    //kdyz neni %cond bool (ted uz musi byt float)
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    sprintf(pomocna_data1.str_data, "%%expret");
    sprintf(pomocna_data2.str_data, "bool");
    generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_STRING, pomocna_data2);

    //kdyz je %cond 0.0
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    sprintf(pomocna_data1.str_data, "%%cond");
    pomocna_data2.dbl_data = 0.0;
    generateInstruction(I_JUMPIFNEQ, P_LABEL, pomocna_data, P_GF, pomocna_data1, P_FLOAT, pomocna_data2);
    //priradim false a jdu na konec
    sprintf(pomocna_data2.str_data, "false");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);
    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);

    //label ifu v ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter+1);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);
    //priradim true
    sprintf(pomocna_data2.str_data, "true");
    generateInstruction(I_MOVE, P_GF, pomocna_data1, P_BOOL, pomocna_data2, P_NULL, pomocna_data2);

    //label ifu
    sprintf(pomocna_data.str_data, "%%type%d", typCondCounter);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data, P_NULL, pomocna_data, P_NULL, pomocna_data);

    typCondCounter = typCondCounter +2;

    sprintf(pomocna_data2.str_data, "%%konec%d", konecNum);
    generateInstruction(I_LABEL, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);


    //jedeme dal
    sprintf(pomocna_data.str_data, "%%cond");
    generateInstruction(I_NOT, P_GF, pomocna_data, P_GF, pomocna_data, P_NULL, pomocna_data);
    sprintf(pomocna_data2.str_data, "true");
    sprintf(pomocna_data1.str_data, "%%whileB%d", condCounter);
    generateInstruction(I_JUMPIFEQ, P_LABEL, pomocna_data1, P_GF, pomocna_data, P_BOOL, pomocna_data2);

    if(Result!= IT_IS_OKAY) return Result;
    //teraz nam PSA vrati _Token, cize ocakavme :
    if(strcmp(_Token.data.str_data, ":") == 0  )
    {
        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol)
        {

            //ak tu je dvojbodka volame pravidlo indent
            Result = IndentRule(Result);

            if(Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                _ActualIndent++;
                //ak prisie indent, nasleduje znova sekvencia statementov
                _Token = getNextSymbol(stdin, LexStack);

                Result = StatRule(Result);

                sprintf(pomocna_data2.str_data, "%%whileA%d", condCounter);
                sprintf(pomocna_data1.str_data, "%%whileB%d", condCounter);
                generateInstruction(I_JUMP, P_LABEL, pomocna_data2, P_NULL, pomocna_data, P_NULL, pomocna_data);
                generateInstruction(I_LABEL, P_LABEL, pomocna_data1, P_NULL, pomocna_data, P_NULL, pomocna_data);
                inWhile = false;


            }
        }
    }
    else
    {
        Result = SYNTAX_ERROR;
    }


    return Result;
}

void generateInstruction(int instType, int prefix1, union Data data1, int prefix2, union Data data2, int prefix3, union Data data3)
// vlozi novou instrukci do seznamu instrukci
{
    tInstr I;

    I.instType = instType;

    I.prefix1 = prefix1;
    I.data1 = data1;


    I.prefix2 = prefix2;
    I.data2 = data2;

    I.prefix3 = prefix3;
    I.data3 = data3;

   listInsertLast(_IL, I);
}

