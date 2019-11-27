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
SymTable_t *_ST;
//globalna premmenna _Tokenu
Symbol_t _Token;
Symbol_t _PreviousToken;
bool _ProgRuleCalled = false;
bool _IndentFlag = false;
bool _DefFlag = false;
int _ActualIndent = 0;
int _ActualNumberOfParams = 0;
int _Result = 0;
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
    //dalsi symbol musi byt rovna sa
    _Token = getNextSymbol(stdin, LexStack);
    if(_Token.type == _equal)
    {
        //teraz sa musime rozhodnut, ci volame funckiu alebo priradzujeme vyraz
        _Token = getNextSymbol(stdin,LexStack);
        //vieme, ze ak volame funkciu, tak nasledujuci identifikator musi byt v TS a musi to byt funkciaS
        SymTableItem_t* prevDefFun = SymTableSearch(_ST, _Token.data.str_data);
        if(prevDefFun != NULL && prevDefFun->Type == _IDfunction)
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
                    //porovname pocet parametrov z predtym definovanou funkciou
                    if(prevDefFun->NumberOfParameters == _ActualNumberOfParams)
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
                _ActualNumberOfParams = 0;
                
            }
            //nenasli sme funckiu ,sematicka chyba
            
        }
        //ak sme nenasli, na zaklade dalsieho tokenu sa rozhodujeme, ci ide o volanie vstavanej funkcie alebo volame PSA
        else
        {
            
            //ak typ dalsieho tokena je je _func, to znamena, ze priradzujeme built in funkciu 
            if(_Token.type == _func)
            {
                //nedefinovana funkcia
                _Result = BuiltInFuncRule();
            }
            //inak sa jedna o vyraz
            else
            {   

                //ak dalsi symbol je zatvorka jedna sa o nedefinovanu funckiu
            
                if(_Token.type == _left_bracket)
                {
                    _Result = SEMANTIC_ERROR;
                }
                else
                {
                    //PSA musite povedat, ze sme nacitali jeden jej token, cize ona jedno nacitanie tokenu preskoci
                    _Result = Expression_analysis();
                    //skontrolujeme result, ako token mi ma vrati EOL
                    
                }
                     
            }
        }
        
    }
    else
    {
        //ak dalsi token je EOL mame nedefinovanu premmennu, inak je to SYNTAX ERROR
        
        if(_Token.type == _eol)
        {
            _Result = SEMANTIC_ERROR;
        }
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
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            //musime si ich ukladat do tabulky symbolov 
            _Result = ParamsRule(DEF_PARAMETERS);
            //printf("actual number of params: %d\n", _ActualNumberOfParams);
            
            //printf("params str: %s\n", _ParamsArray[0].data.str_data);
            SymTableInsert(_ST, Identifier, _IDfunction, _ActualNumberOfParams);
            _ActualNumberOfParams = 0;
            
            
            


            if(_Result == IT_IS_OKAY)
            {
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
                        _ActualIndent++;
                        //som v definicii funkcie, moze tam byt aj return
                        _DefFlag = true;
                        //dasli _Token mozu byt statementy alebo return
                        _Token = getNextSymbol(stdin, LexStack);

                        _Result = StatRule();

                        
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
    char data[MAX_ID_LENGTH];
    
    
    if(_Token.type == _int || _Token.type == _string || _Token.type == _id )
    {
        
        strcpy(data,_Token.data.str_data);
       
                
        
        _ActualNumberOfParams++;
      
        
        _Result = ParamsRule(numberOfParams);
            
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
            _Result = Expression_analysis();

            _DefFlag = false;
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
    _Result = Expression_analysis();;
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
    _Result = Expression_analysis();
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

int Expression_analysis()
{
    do
    {
        _Token = getNextSymbol(stdin, LexStack);
        if (strcmp(_Token.data.str_data, ":") == 0) break;
        /* code */
    } while ((_Token.type != _eol) );
    
    return IT_IS_OKAY;
}
