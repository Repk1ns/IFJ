#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS

#define PRINT_PARAMETERS -1
#define DEF_PARAMETERS -2
#define MAX_NUMBER_OF_PARAMS 1000
#define MAX_SIZE_OF_STRING 1000



//parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
int Parse(SymTable_t *ST);
//hlavne pravidlo celeho parseru
int ProgRule();
//pravidlo pre statementy
int StatRule();
//pravidla pre vsetky funckie
int DefRule();
//pravidlo pre identifikatori
int IdRule();
//pravidlo pre vstavane funckie
int BuiltInFuncRule();
//pravidlo pre ostatne funkcie
int DefRule();
//pravidlo pre klucove slova
int KeywordsRule();
//pravidlo pre if
int IfRule();
//pravidlo pre else
int ElseRule();
//pravidlo pre while;
int WhileRule();
//pravidlo pre indent
int IndentRule();
//pravidlo pre dedent
int DedentRule();
//pravidlo pre parametre vo funkciach
int ParamsRule(int numberOfParams);
//pravidlo na zistenie typu erroru v parametroch funckiach
int ParamsTypeOfError();

int Expression_analysis();

#endif