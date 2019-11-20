#ifndef SYNTAX_ANALYSIS
#define SYNTAX_ANALYSIS
//parse fukncia, chyba parameter pre vytvaranie trojadresnych instrukcii
int Parse(SymTable_t *ST);
//hlavne pravidlo celeho parseru
int ProgRule();
//pravidlo pre statementy
int StatRule();
//pravidlo pre identifikatori
int IdRule();
//pravidlo pre vstavane funckie
int FuncRule();
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

int Expression_analysis();

#endif