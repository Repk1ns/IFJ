#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS

#define INF_PARAMETERS -1
#define DEF_PARAMETERS -2
#define ID_PARAMETERS -3



//parse fukncia
int Parse(SymTable_t *ST, void *Stack, void *List);
//hlavne pravidlo celeho parseru
int ProgRule();
//pravidlo pre statementy
int StatRule();
//pravidla pre vsetky funckie
int DefRule();
//pravidlo pre identifikatori
int IdRule();
//pravidlo pre volanie funkcii
int FuncCallRule(SymTableItem_t *item);
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
//fce pro generovani instrukci
void generateInstruction(int instType, int prefix1, union Data data1, int prefix2, union Data data2, int prefix3, union Data data3);

int Expression_analysis();

#endif
