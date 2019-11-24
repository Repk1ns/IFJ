#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS

#define INF_PARAMETERS -1
#define DEF_PARAMETERS -2




//parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
int Parse(SymTable_t *ST, void *Stack);
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
//fce pro generovani instrukci
void generateInstruction(int instType, int prefix1, void *addr1, int prefix2, void *addr2, int prefix3, void *addr3);

int Expression_analysis();

#endif
