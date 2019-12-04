#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS

#define INF_PARAMETERS -1
#define DEF_PARAMETERS -2
#define ID_PARAMETERS -3



//parse fukncia
int Parse(SymTable_t *ST, void *Stack, void *List);
//hlavne pravidlo celeho parseru
int ProgRule(int Result);
//pravidlo pre statementy
int StatRule(int Result);
//pravidla pre vsetky funckie
int DefRule(int Result);
//pravidlo pre identifikatori
int IdRule(int Result);
//pravidlo pre volanie funkcii
int FuncCallRule(int Result,SymTableItem_t *item, bool CalledAlone);
//pravidlo pre vstavane funckie
int BuiltInFuncRule(int Result);
//pravidlo pre ostatne funkcie
int DefRule(int Result);
//pravidlo pre klucove slova
int KeywordsRule(int Result);
//pravidlo pre if
int IfRule(int Result);
//pravidlo pre else
int ElseRule(int Result);
//pravidlo pre while;
int WhileRule(int Result);
//pravidlo pre indent
int IndentRule(int Result);
//pravidlo pre dedent
int DedentRule(int Result);
//pravidlo pre parametre vo funkciach
int ParamsRule(int Result,int numberOfParams);
//fce pro generovani instrukci
void generateInstruction(int instType, int prefix1, union Data data1, int prefix2, union Data data2, int prefix3, union Data data3);

int Expression_analysis();

#endif
