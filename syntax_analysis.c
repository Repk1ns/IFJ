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
//globalna premmenna _Tokenu
Symbol_t _Token;
Symbol_t _PreviousToken;
bool _ProgRuleCalled = false;
bool _IndentFlag = false;
bool _DefFlag = false;
int _ActualIndent = 0;
int _ActualNumberOfParams = 0;
int _Result = 0;
int _NumberOfNotDefFun = 0;
tStack *LexStack;

// parse fukncia
// funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
int Parse(SymTable_t *ST, void *Stack, void *List)
{
    
    _IL = List;
    LexStack = Stack;
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


    
    _ST = ST;
    _Result = ProgRule();
    //ostala nam funkcia, ktora nebola definovana
    if(_NumberOfNotDefFun != 0) _Result = SEMANTIC_ERROR;

    
    return _Result;
}

int ProgRule()
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
        if(_Result == IT_IS_OKAY)
        {
            _Result = DefRule();
        }


        //zavolanie pravidla prog pre prejdenie celeho suboru az po eof, ale iba ak je vsetko v poriadku
        if(_Result == IT_IS_OKAY) 
        {   
            _Result = ProgRule();
        }
    
    }
    if(_Token.type == _eof)
    //alebo je _Token EOF tak koncim program
    {
        _Result = IT_IS_OKAY;
    }
    //inak simulujem pravidlo STAT
    else
    {
        //STAT
        if(_Result == IT_IS_OKAY)
        {
            _Result = StatRule();
        }
        //zavolanie pravidla prog pre prejdenie celeho suboru az po eof, ale iba ak je vsetko v poriadku
        if(_Result == IT_IS_OKAY) 
        {   
            _Result = ProgRule();
        }
    }
    
    return _Result;
}

int StatRule()
{
    

    switch (_Token.type)
    {
        //ak _Token je identifikator simulujem pravidlo ID
        case _id:
            //ulozime si identifikator do tabulky symbolov
            //SymTableInsert(_ST,_Token, _IDvariable, ID_PARAMETERS );
            //SymTableItem_t* item =SymTableSearch(_ST, "dado");
            _Result = IdRule();

            if(_Result == IT_IS_OKAY)
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
                        _Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        _Result = StatRule();
                    }
                }
            }
        break;
        //ak je _Token _func, simulujem pravidla pre vstavane funkcie
        case _func:
            _Result = BuiltInFuncRule();
            if(_Result == IT_IS_OKAY)
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
                        _Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        _Result = StatRule();
                    }
                }
            }
        break;
        //ak je _Token _keyword, simulujem pravidla pre keywordy
        case _keyword:
            _Result = KeywordsRule();
            if(_Result == IT_IS_OKAY )
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
                        _Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        _Result = StatRule();
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
                        _Result = IT_IS_OKAY;
                        if(_ActualIndent == 0) _IndentFlag = false;
                        
                        
                        break;
                    }
                        else if(_Token.type == _null)
                    {
                        _Result = SYNTAX_ERROR;
                        break;
                    }
                    else
                    {
                        _Result = StatRule();
                    }
                }
        break;
         case _eof:
         break;
    default:
        _Result = SYNTAX_ERROR;
        break;
    }

    return _Result;
}

int IdRule()
{
    SymTableItem_t* prevDefFun;
    //ak nie sme v definicii funkcie ulozime si indetifikator do globanej tabulky symbolov ako typ premennej
    if(_DefFlag == false)
    {
        //musime si osetrit, ci nahodou sa nejedna o redefiniciu funkcie
        prevDefFun = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
        
        //ak sme nasli
       
    }
    //teraz sme v definici funkcie, cize pouzivame lokalnu tabulky symbolov
    else
    {
        SymTableInsert(_STlocal,_Token,_IDvariable, ID_PARAMETERS,SIZE_OF_SYMTABLE_LOCAL);
    }
    
    if(_Result == IT_IS_OKAY)
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
                if(prevDefFun->Type == _IDfunction) _Result = SEMANTIC_ERROR;
            }
                if(_Result == IT_IS_OKAY) SymTableInsert(_ST,_Token,_IDvariable, ID_PARAMETERS, SIZE_OF_SYMTABLE_GLOBAL);
            }

            if(_Result == IT_IS_OKAY)
            {
                //teraz sa musime rozhodnut, ci volame funckiu alebo priradzujeme vyraz -> prednacitame si jeden token
                _Token = getNextSymbol(stdin,LexStack);

                //teraz riesime globalny kontext
                if(_DefFlag == false)
                {
                    //vieme, ze ak volame funkciu, tak nasledujuci identifikator musi byt v TS a musi to byt typ funkcia
                    prevDefFun = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                    //ak sme identifikator nasli a je uz predtym definovany ako _IDfunction tak sa jedna o volanie funkcie
                    if((prevDefFun != NULL) && (prevDefFun->Type == _IDfunction))
                    {
                        _Result = FuncCallRule(prevDefFun);
                        _ActualNumberOfParams = 0;
                    }
                    //ak sme identifikator nasli ale je to typ _ID variable, volame PSA
                    else if ((prevDefFun != NULL) && (prevDefFun->Type == _IDvariable))
                    {
                        
                        //PSA musite povedat, ze sme nacitali jeden jej token, cize ona jedno nacitanie tokenu preskoci
                        //tu sa bude musiet riesit semantika identifikatorov vo vyraze, ci su definovane
                        //ja mu predam pointer na tabulku symbolov, ci uz lokalnu alebo globalnu
                        _Result = Expression(&_Token, true);
                        //skontrolujeme result, ako token mi ma vrati EOL
                    }
                    //ak sme token nenasli a je to identifikator jedna sa nedefinovanu funkciu/premennu
                    else if (prevDefFun == NULL && _Token.type == _id)
                    {
                        _Result = SEMANTIC_ERROR;
                    }
                    //ak token je vstavana funckia, volame pravidlo pre vstavane funckie
                    else if(_Token.type == _func)
                    {
                        _Result = BuiltInFuncRule();
                    }
                    //inak proste volame PSA a ta nam povie co je zle
                    else
                    {
                        _Result = Expression(&_Token, true);
                    }
                    
                    
                }
                //teraz sme v lokalnom kontexte
                else
                {   
                    //hladame v lokalnej tabulke symbolov
                    prevDefFun = SymTableSearch(_STlocal, _Token.data.str_data, SIZE_OF_SYMTABLE_LOCAL);
                    //ak sme identifikator nasli ale je to typ _ID variable, volame PSA
                    if ((prevDefFun != NULL) && (prevDefFun->Type == _IDvariable))
                    {
                        
                        //musime povedat, ze sme prednacitali jeden token
                        _Result = Expression(&_Token,true);
                        
                    }
                    //ak token je vstavana funckia, volame pravidlo pre vstavane funkcie
                    else if(_Token.type == _func)
                    {
                        _Result = BuiltInFuncRule();
                    }
                    //identifikator sme nenasli
                    else
                    {
                        //skusime ho vyhladat v globalnej tabulke symbolov, ci sa nahodou nejedna o volanie funkcie pre funkciu, ktora uz je definovana
                        prevDefFun = SymTableSearch(_ST, _Token.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                        //ak sme nasli, jedna sa o volanie funkcie, porovname pocet parametrov
                        if(prevDefFun != NULL)
                        {
                            _Result = FuncCallRule(prevDefFun);
                            if(_Result == IT_IS_OKAY)
                            {
                                //ak sa pocet parametrov nerovna jedna sa o chybu poctu parametrov
                                if(prevDefFun->NumberOfParameters != _ActualNumberOfParams) _Result = SEMANTIC_PARAMS_ERROR;
                            }
                            //vynulujeme pocet aktualnych parametrov
                            _ActualNumberOfParams = 0;

                        }
                        //ak je token identifikator, teraz predpokladame, ze sa jedna o volanie funkcie, ak sa nejedna, urcite to je chyba
                        else if(_Token.type == _id)
                        {
                            //ulozime si identifikator volanej funkcie
                            Symbol_t idCallFun;
                            idCallFun.type = _Token.type;
                            strcpy(idCallFun.data.str_data,_Token.data.str_data);
                            //osetrime syntax volania
                            _Result = FuncCallRule(NULL);

                            //ak je vsetko v poriadku tak si ulozime volanu funkciu do tabulky symbolov pre pozdejsie porovnanie poctu parametrov
                            if(_Result == IT_IS_OKAY)
                            {
                                SymTableInsert(_ST, idCallFun, _IDfunction, _ActualNumberOfParams, SIZE_OF_SYMTABLE_GLOBAL);
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
                            _Result = Expression(&_Token, true);
                        }
                        
                        
                        
                    }
                }
            }
            
            
        }
        else
        {
            //TODO DOROBIT VOLANIE FUNKCIE
            //ak dalsi token je EOL mame nedefinovanu premmennu, inak je to SYNTAX ERROR
            
            // if(_Token.type == _eol)
            // {
            //     _Result = SEMANTIC_ERROR;
            // }
            // else _Result = SYNTAX_ERROR;
        }
    }
    
    return _Result;
}

int FuncCallRule(SymTableItem_t *item)
{
    //vynulujeme premennu s parametrami
    _ActualNumberOfParams = 0;
    //simulujeme pravidlo pre volanie funckie
    //ocakavame ako dalsi token zatvorku
    _Token = getNextSymbol(stdin,LexStack);
    if(_Token.type == _left_bracket)    
    {
        //osetrime parametre
        _Result = ParamsRule(INF_PARAMETERS);
        if(_Result == IT_IS_OKAY)
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
                        _Result = IT_IS_OKAY;
                    }
                }
                //semanticka chyba poctu parametrov
                else
                {
                    _Result = SEMANTIC_PARAMS_ERROR;
                }
            }
            
            
            
        }
       
        
    }
    //neprisla zatvorka,v lokalnom kontexte nedefinovana premenna, semanticka chyba , v globalnom syntax
    else
    {
    
        if(_DefFlag == true) _Result = SEMANTIC_ERROR;
        else _Result = SYNTAX_ERROR;
    }
    
    return _Result;
}



int BuiltInFuncRule()
{
    _Result = SYNTAX_ERROR;
    

    if(strcmp(_Token.data.str_data, "inputs") == 0 ||
       strcmp(_Token.data.str_data, "inputi") == 0 ||
       strcmp(_Token.data.str_data, "inputf") == 0 )
    {
        
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 0
           
            _Result = ParamsRule(0);
        }
      
    }
    else if(strcmp(_Token.data.str_data, "len") == 0 ||
            strcmp(_Token.data.str_data, "chr") == 0)
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 
            
            _Result = ParamsRule(1);
            _ActualNumberOfParams = 0;
            
           
        }
        
    }
    else if(strcmp(_Token.data.str_data, "ord") == 0 )
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 2
            
            _Result = ParamsRule(2);
            _ActualNumberOfParams = 0;
            
        }
    }
    else if(strcmp(_Token.data.str_data, "substr") == 0 )
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 3
            
            _Result = ParamsRule(3);
            _ActualNumberOfParams = 0;
            
        }
    }
     else if(strcmp(_Token.data.str_data, "print") == 0 )
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            _Result = ParamsRule(INF_PARAMETERS);
            _ActualNumberOfParams = 0;
            
        }
    }
    else
    {
        _Result = SEMANTIC_ERROR;
    }
    
    return _Result;

}

int DefRule()
{
    _Result = SYNTAX_ERROR;
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

        _Token = getNextSymbol(stdin, LexStack);
        //musi byt zatvorka
        if(strcmp(_Token.data.str_data,"(") == 0)
        {
            //viem, ze som v definicii funkcie, cize vytvaram si lokalnu tabulku symbolov
            _STlocal = SymTableInit(SIZE_OF_SYMTABLE_LOCAL);
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            //musime si ich ukladat do tabulky symbolov 
            _Result = ParamsRule(DEF_PARAMETERS);
            //printf("actual number of params: %d\n", _ActualNumberOfParams);
            
          
            if(_Result == IT_IS_OKAY)
            {
                //musime osetrit, ci nie je redefinicia funkcie
                SymTableItem_t *item = SymTableSearch(_ST, Identifier.data.str_data, SIZE_OF_SYMTABLE_GLOBAL);
                //ak sme nenasli identifikator, vsetko je v poriadku
                if(item == NULL)
                {
                    //vlozime si indetifikator do tabulky symbolov a zapamatame si pocet paramaetrov
                    SymTableInsert(_ST, Identifier, _IDfunction, _ActualNumberOfParams, SIZE_OF_SYMTABLE_GLOBAL);
                   
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
                            _Result = SEMANTIC_PARAMS_ERROR;
                        }

                        
                    }
                    //inak to je redefinicia
                    else
                    {
                        _Result = SEMANTIC_ERROR;
                    }
                    _ActualNumberOfParams = 0;
                }

                _ActualNumberOfParams = 0;
                _Token = getNextSymbol(stdin, LexStack);
                //ak je vsetko v poriadku tak po dvojbodke ocavame _Token EOL
                if(_Token.type == _eol)
                {
                    //_Token = getNextSymbol(stdin, LexStack);
                    _Result = IndentRule();
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

                        
                        _Result = StatRule();
                        //lokalnu TS uz nepotrebujeme
                        SymTableDelete(_STlocal);
                        _STlocal = NULL;
                        
                        //a povieme, ze uz nie sme v definicii funkcie
                        _DefFlag = false;
                        
                    }
                }
                
            }
        }
    }
    
    //free(Identifier.data.str_data);
    return _Result;
}

//pravidlo pre parametre vstavanych a aj novo definonavynch funkcii- osetruje pocet parametrov a rozhoduje aj o semnatike
int ParamsRule(int numberOfParams)
{
    
    _Result = SYNTAX_ERROR;
    _PreviousToken = _Token;
    _Token = getNextSymbol(stdin, LexStack);
    //char data[MAX_ID_LENGTH];
    
    
    if(_Token.type == _int || _Token.type == _string || _Token.type == _id )
    {
        //vieme, ze v definicii funkcie mozu byt parametre iba identifikatori
        if(numberOfParams == DEF_PARAMETERS)
        {
            //ak som v definicii funkci, musim su ulozit parametre funkcie do lokalnej tabulky symobolov
            SymTableInsert(_STlocal, _Token, _IDvariable, ID_PARAMETERS, SIZE_OF_SYMTABLE_LOCAL);
            if(_Token.type == _id) 
            {
                _ActualNumberOfParams++;
                _Result = ParamsRule(numberOfParams);
            }
            else 
            {
                _Result = SEMANTIC_OTHERS_ERROR;
            }
        }
        //nie som v definicii funkcie, moze tu byt hocico
        else   
        {
            _ActualNumberOfParams++;
            _Result = ParamsRule(numberOfParams);
        }
            
    }
    else if(strcmp(_Token.data.str_data, ")") == 0)
    {
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol || _Token.type == _eof )
        {
            if((_ActualNumberOfParams == numberOfParams) && (_PreviousToken.type != _comma))
            {
                _Result = IT_IS_OKAY;
            }
            else if  ((numberOfParams == INF_PARAMETERS) && (_PreviousToken.type != _comma))
            {
                _Result = IT_IS_OKAY;
            }
            
            else
            {
                if(_PreviousToken.type == _comma)
                {
                    _Result = SYNTAX_ERROR;
                }
                else _Result = SEMANTIC_PARAMS_ERROR;
            }
        }
        else if((strcmp(_Token.data.str_data, ":") == 0) && (numberOfParams == DEF_PARAMETERS) )
        {
            if(_PreviousToken.type == _comma)
            {
                    _Result = SYNTAX_ERROR;
            }
            else _Result = IT_IS_OKAY;
        }
        
    }
    else if(_Token.type == _comma)
    {
        _Result = ParamsRule(numberOfParams);
    }
    else
    {
        _Result = SYNTAX_ERROR;
    }
    
    
    
    
    return _Result;
}




int KeywordsRule()
{
    _Result = 0;
    //simulujem pravidlo pre if
    if(strcmp(_Token.data.str_data,"if") == 0)
    {
       _Result = IfRule();
    }
    //simulujem pravidlo pre while
    else if(strcmp(_Token.data.str_data,"while") == 0)
    {
        _Result = WhileRule();
    }
    //simulujem pravidlo pre pass a None
    else if(strcmp(_Token.data.str_data,"pass") == 0 || (strcmp(_Token.data.str_data,"None") == 0))
    {
        //ak je to None a nasledujci _Token je pass je vsetko v poriadku
        _Token = getNextSymbol(stdin, LexStack);
        //ak je _Token eof, to znamena ze pass je poslendy prikaz
        if(_Token.type == _eol || _Token.type == _eof)
        {
            _Result = IT_IS_OKAY;
        }
        else
        {
            _Result = SYNTAX_ERROR;
        }
        
    }
    else if(strcmp(_Token.data.str_data,"return") == 0)
    {
        //otestujeme, ci volame return v definicii funkcie
        if(_DefFlag == true && _IndentFlag == true)
        {
            
            // po returne ocakavame identifikator
            // vieme, ze za returnom ocakavame vyraz, cize volame PSA
            //_Token = getNextSymbol(stdin, LexStack); //---> TOTO TU NEBUDE!
            _Result = Expression(&_Token,false);

            
        }
        else
        {
            _Result = SYNTAX_ERROR;
        }
        
    }

    //inak chyba
    else
    {
        _Result = SYNTAX_ERROR;
    }
    
    return _Result;
}

//pravidlo pre if 
int IfRule()
{
    
    //teraz som v pravidle if, a viem, ze v _Tokene mam if, cize teraz volam PSA
    //ona vola getnextSYmbol() a ked skocni, vrati mi errorcode a nasledujuci _Token do pravidla, cize by to mala byt dvojbodka
    _Result = Expression(&_Token,false);
    
    if(_Result!= IT_IS_OKAY) return _Result;
    
    //zavolame dalsi _Token a zisitme, ci tam je ":"
    //_Token = getNextSymbol(stdin, LexStack); //->>>>> TOTO TU NEBUDE LEBO MI VRATI _Token PSA
    if(strcmp(_Token.data.str_data, ":") == 0)
    {

        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol)
        {
            
            //ak tu je dvojbodka volame pravidlo indent
            _Result = IndentRule();

            if(_Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                _ActualIndent++;
                
                _Token = getNextSymbol(stdin, LexStack);
                //ak prisie indent, nasleduje znova sekvencia statementov
                _Result = StatRule();
                //ak je to v poriadku, osetrime dedent
                if(_Result == IT_IS_OKAY)
                {
                    //volame else rule
                    _Result = ElseRule();
                } 
                
                
            }
 
        }
  
    }
    else
    {
        _Result = SYNTAX_ERROR;
        
    }
    
    return _Result;
}

int ElseRule()
{

    _Result = 0;
    //volame dalsi _Token
    _Token = getNextSymbol(stdin, LexStack);
   
    //ak to je else, simulujeme pravidlo pre else statemts
    if(strcmp(_Token.data.str_data, "else") == 0 )
    {
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
                _Result = IndentRule();

                if(_Result == IT_IS_OKAY)
                {
                    _IndentFlag = true;
                    _ActualIndent++;
                    //ak prisie indent, nasleduje znova sekvencia statementov
                    _Token = getNextSymbol(stdin, LexStack);

                    _Result = StatRule();
                    
                }
            }
            
        }
        else
        {
            _Result = SYNTAX_ERROR;
        }

    }
    //ak else neexistuje, znova pokracuju pravidla programu
    else
    {
        _Result = IT_IS_OKAY;
        _ProgRuleCalled = true;
        //nastavime si actualny _Token, aby sme v dalsom pravidle necitali dalej
    }
    //TODO
    return _Result;
}

int IndentRule()
{
    
    _Token = getNextSymbol(stdin, LexStack);
    // _Token.type =_indent;
    // _Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci _Token indent
    if(_Token.type == _indent)
    {
        _Result = IT_IS_OKAY;
    }
    //ak prisiel eol je to korektne a znova ocakavame indent
    else if(_Token.type == _eol)
    {
        _Result = IndentRule();
    }
    else
    {
        _Result = SYNTAX_ERROR;
    }
    return _Result;
}


int DedentRule()
{
    
    //_Token = getNextSymbol(stdin, LexStack);
    _Token.type =_dedent;
    _Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci _Token dedent
    if(_Token.type == _dedent)
    {
        _Result = IT_IS_OKAY;
    }
    else
    {
        _Result = SYNTAX_ERROR;
    }
    
    return _Result;
}

int WhileRule()
{
    _Result = SYNTAX_ERROR;
    //zavolame PSA
    _Result = Expression(&_Token,false);
    if(_Result!= IT_IS_OKAY) return _Result;
    //teraz nam PSA vrati _Token, cize ocakavme :
    //_Token = getNextSymbol(stdin, LexStack); //<---------toto tu NEBUDE!!! vrati nam to PSA

     
    if(strcmp(_Token.data.str_data, ":") == 0  )
    {
        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin, LexStack);
        if(_Token.type == _eol)
        {

            //ak tu je dvojbodka volame pravidlo indent
            _Result = IndentRule();

            if(_Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                _ActualIndent++;
                //ak prisie indent, nasleduje znova sekvencia statementov
                _Token = getNextSymbol(stdin, LexStack);

                _Result = StatRule();
                

            }
        }
    }
    else
    {
        _Result = SYNTAX_ERROR;
    }
    

    return _Result;
}

void generateInstruction(int instType, int prefix1, void *addr1, int prefix2, void *addr2, int prefix3, void *addr3)
// vlozi novou instrukci do seznamu instrukci
{
   tInstr I;

   I.instType = instType;

   I.prefix1 = prefix1;
   I.addr1 = addr1;

   I.prefix2 = prefix2;
   I.addr2 = addr2;

   I.prefix3 = prefix3;
   I.addr3 = addr3;

   listInsertLast(_IL, I);
}

//ako parametre budu tabulka symbolov, lokalna a globalna, flag na vyraz(lebo prednacitavam token - podla neho zistime si je vyraz alebo if a vieme, cim ma koncit ten vyraz)
int Expression_analysis()
{
    do
    {
        _Token = getNextSymbol(stdin, LexStack);
        if (strcmp(_Token.data.str_data, ":") == 0) break;
        if (_Token.type == _eof)  break;
        /* code */
    } while ((_Token.type != _eol) );
    
    return IT_IS_OKAY;
}
