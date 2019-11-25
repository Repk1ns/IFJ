#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>



typedef struct StackItem {
	Precedence_table_symbol stackSymbol;
	enum Type typ;
	struct StackItem *nextItem;
} tStackItem;

typedef struct {
	unsigned int size;
	tStackItem *topItem;
} tStack;

void sInit(tStack *);
int sPush(tStack *, Precedence_table_symbol, Type);
tStackItem* TopTerminal(tStack*);
bool push_TopTerminal(tStack* Stack, Precedence_table_symbol symbol, Type);
void *sTop(tStack *);
void *sTopPop(tStack *);
int sDelete(tStack *);
int sDispose(tStack *);
bool sEmpty(tStack *);

#endif