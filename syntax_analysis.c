#include <stdio.h>
#include "lexical_analysis.h"
#include "symtable.h"
#include "syntax_analysis.h"
#include "error_codes.h"

//globalna tabulka symbolov
SymTable_t *_ST;
//globalna premmenna _Tokenu
Symbol_t _Token;
Symbol_t _PreviousToken;
bool _ProgRuleCalled = false;
bool _IndentFlag = false;
bool _DefFlag = false;
int _ActualNumberOfParams = 0;
int _Result = 0;

// parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
// funkcia getnextsymbol() sa nebude volat v maine ale v parse funkcii, TODO
int Parse(SymTable_t *ST)
{
    

    // do {
    //     _Token = getNextSymbol(stdin);
    //     switch (_Token.type){
    //     case _int:
    //     case _whitespace: {
    //         printf("type: %d, data: %d\n", _Token.type, _Token.data.int_data);
    //         break;
    //     }
    //     case _double: {
    //         printf("type: %d, data: %f\n", _Token.type, _Token.data.dbl_data);
    //         break;
    //     }
    //     default: {
    //         printf("type: %d, data: %s\n", _Token.type, _Token.data.str_data);
    //         break;
    //     }
    //     }
    //     printf("--------------------------------------\n");
    // } while (_Token.type != _eof);


    //testing hash table:

    //inserting items
    Symbol_t testsymbol;

    testsymbol.data.str_data = "totojetest1";
    testsymbol.type =  6;
    _ST = ST;

    SymTableInsert(_ST, testsymbol);
    //serach for item
    SymTableItem_t* found = SymTableSearch(_ST, "totojetest1");
    if(found != NULL) printf("YES");
    printf("---------------------------------------------\n");


 //pomocny _Token na prenasanie informacii medzi pravidlami
  
  

    //vysledok parseru

    
    _Result = ProgRule();

    
    return _Result;
}

int ProgRule()
{
    
    
    //nacitanie pociatocneho _Tokenu ak sme volame prvy krat prog rule
    if(!_ProgRuleCalled) 
    {

        _Token = getNextSymbol(stdin);
    }
    _ProgRuleCalled = false;

    //ak je _Token vstavana funkcia alebo _Token def
    if(_Token.type == _keyword && (strcmp(_Token.data.str_data,"def") == 0))
    {
        
        _Result = DefRule();

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
        _Result = StatRule();
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
        /* code */
        break;
        //ak je _Token _func, simulujem pravidla pre vstavane funkcie
        case _func:
            _Result = BuiltInFuncRule();
            //ak som v indente tak tam mozu byt iba statementy
            if(_IndentFlag) 
            {
                //_Token = getNextSymbol(stdin);
                _Token.type =_dedent;
                _Token.data.int_data = 3;
                if(_Token.type == _dedent)
                {
                    _IndentFlag = false;
                    break;
                }
                else
                {
                    _Result = StatRule();
                }
            }
        break;
        //ak je _Token _keyword, simulujem pravidla pre keywordy
        case _keyword:
            _Result = KeywordsRule();
            if(_IndentFlag) 
            {
                //_Token = getNextSymbol(stdin);
                _Token.type =_dedent;
                _Token.data.int_data = 3;
                if(_Token.type == _dedent)
                {
                    _IndentFlag = false;
                    break;
                }
                else
                {
                    _Result = StatRule();
                }
            }
        break;
        //ak je _Token eol, pokracujem dalej
        case _eol:
            // _Token = getNextSymbol(stdin);
            // _Result = StatRule();
        break;
        
    default:
        _Result = SYNTAX_ERROR;
        break;
    }

    return _Result;
}

int IdRule()
{
    return IT_IS_OKAY;
}



int BuiltInFuncRule()
{
    _Result = SYNTAX_ERROR;

    if(strcmp(_Token.data.str_data, "inputs") == 0 ||
       strcmp(_Token.data.str_data, "inputi") == 0 ||
       strcmp(_Token.data.str_data, "inputf") == 0 )
    {
        
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 0
            _Result = ParamsRule(0);
        }
      
    }
    else if(strcmp(_Token.data.str_data, "len") == 0 ||
            strcmp(_Token.data.str_data, "chr") == 0)
    {
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 1
            _Result = ParamsRule(1);
           
        }
        
    }
    else if(strcmp(_Token.data.str_data, "ord") == 0 )
    {
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 2
            _Result = ParamsRule(2);
            
        }
    }
    else if(strcmp(_Token.data.str_data, "substr") == 0 )
    {
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich ma byt 3
            _Result = ParamsRule(3);
            
        }
    }
     else if(strcmp(_Token.data.str_data, "print") == 0 )
    {
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, "(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            _Result = ParamsRule(PRINT_PARAMETERS);
            
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
    
    _Token = getNextSymbol(stdin);
    //ak to je identifikator
    if(_Token.type == _id)
    {
        SymTableInsert(_ST,_Token);
        SymTableItem_t* item = SymTableSearch(_ST, _Token.data.str_data);
        printf("TYPE: %d \n", item->SymData.type);


        //riesenie semantiky TODO

        _Token = getNextSymbol(stdin);
        //musi byt zatvorka
        if(strcmp(_Token.data.str_data,"(") == 0)
        {
            //zavolame pravidlo pre parametre, vieme, ze ich moze byt kolko len chce
            //musime si ich ukladat do tabulky symbolov 
            _Result = ParamsRule(DEF_PARAMETERS);
            if(_Result == IT_IS_OKAY)
            {
                _Token = getNextSymbol(stdin);
                //ak je vsetko v poriadku tak po dvojbodke ocavame _Token EOL
                if(_Token.type == _eol)
                {
                    //_Token = getNextSymbol(stdin);
                    _Result = IndentRule();
                    //musi prist indent
                    if(_Token.type == _indent)
                    {
                        //som v indente, idu stat rule
                        _IndentFlag = true;
                        //som v definicii funkcie, moze tam byt aj return
                        _DefFlag = true;
                        //dasli _Token mozu byt statementy alebo return
                        _Token = getNextSymbol(stdin);

                        _Result = StatRule();

                        
                    }
                }
            }
        }
    }
    
    return _Result;
}

//pravidlo pre parametre vstavanych a aj novo definonavynch funkcii- osetruje pocet parametrov a rozhoduje aj o semnatike
int ParamsRule(int numberOfParams)
{
    
    _Result = SYNTAX_ERROR;
    _PreviousToken = _Token;
    _Token = getNextSymbol(stdin);


    

    if(_Token.type == _int || _Token.type == _string || _Token.type == _id )
    {
        _ActualNumberOfParams++;
        //ak mame def funkcie, ukladame si parametre do tabulky symbolov
        if(numberOfParams == DEF_PARAMETERS)
        {

        }
        
        _Result = ParamsRule(numberOfParams);
            
    }
    else if(strcmp(_Token.data.str_data, ")") == 0)
    {
        _Token = getNextSymbol(stdin);
        if(_Token.type == _eol || _Token.type == _eof)
        {
            if((_ActualNumberOfParams == numberOfParams) && (_PreviousToken.type != _comma))
            {
                _Result = IT_IS_OKAY;
            }
            else if  ((numberOfParams == PRINT_PARAMETERS) && (_PreviousToken.type != _comma))
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
    
   
    _ActualNumberOfParams = 0;
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
        _Token = getNextSymbol(stdin);
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
            _Token = getNextSymbol(stdin); //---> TOTO TU NEBUDE!
            _Result = Expression_analysis();

            _DefFlag = false;
            _IndentFlag = false;
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
    _Result = 0;
    //teraz som v pravidle if, a viem, ze v _Tokene mam if, cize teraz volam PSA
    //ona vola getnextSYmbol() a ked skocni, vrati mi errorcode a nasledujuci _Token do pravidla, cize by to mala byt dvojbodka
    _Result = Expression_analysis();
    if(_Result!= IT_IS_OKAY) return _Result;
    
    //zavolame dalsi _Token a zisitme, ci tam je ":"
    _Token = getNextSymbol(stdin); //->>>>> TOTO TU NEBUDE LEBO MI VRATI _Token PSA
    if(strcmp(_Token.data.str_data, ":") == 0)
    {

        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin);
        if(_Token.type == _eol)
        {
            
            //ak tu je dvojbodka volame pravidlo indent
            _Result = IndentRule();

            if(_Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                
                _Token = getNextSymbol(stdin);
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
    _Token = getNextSymbol(stdin);
    
    //ak to je else, simulujeme pravidlo pre else statemts
    if(strcmp(_Token.data.str_data, "else") == 0 )
    {
        //nacitame dalsi _Token, co musi byt :
        _Token = getNextSymbol(stdin);
        if(strcmp(_Token.data.str_data, ":") == 0  )
        {
            // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
            _Token = getNextSymbol(stdin);
            if(_Token.type == _eol)
            {
                //ak je to eol zavolame dalsi _Token a riesime statementy
                //_Token = getNextSymbol(stdin);
                //ak tu je dvojbodka volame pravidlo indent
                _Result = IndentRule();

                if(_Result == IT_IS_OKAY)
                {
                    _IndentFlag = true;
                    //ak prisie indent, nasleduje znova sekvencia statementov
                    _Token = getNextSymbol(stdin);

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
    _Result = 0;
    //_Token = getNextSymbol(stdin);
    _Token.type =_indent;
    _Token.data.int_data = 3;
    //osetrime, ci je nasleudjuci _Token indent
    if(_Token.type == _indent)
    {
        _Result = IT_IS_OKAY;
    }
    else
    {
        _Result = SYNTAX_ERROR;
    }
    return _Result;
}


int DedentRule()
{
    _Result = 0;
    //_Token = getNextSymbol(stdin);
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
    _Token = getNextSymbol(stdin); //<---------toto tu NEBUDE!!! vrati nam to PSA

     
    if(strcmp(_Token.data.str_data, ":") == 0  )
    {
        // je tu dvojbodka, zavolame dalsi _Token, co by mal byt eol
        _Token = getNextSymbol(stdin);
        if(_Token.type == _eol)
        {

            //ak tu je dvojbodka volame pravidlo indent
            _Result = IndentRule();

            if(_Result == IT_IS_OKAY)
            {
                _IndentFlag = true;
                //ak prisie indent, nasleduje znova sekvencia statementov
                _Token = getNextSymbol(stdin);

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


int Expression_analysis()
{
    return IT_IS_OKAY;
}