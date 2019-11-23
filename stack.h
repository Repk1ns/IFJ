#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct StackItem {
	void *data;
	int intdata;
	struct StackItem *nextItem;
} tStackItem;

typedef struct {
	unsigned int size;
	tStackItem *topItem;
} tStack;

void sInit(tStack *);
int sPush(tStack *, void *, int data);
void *sTop(tStack *);
int sLexTop(tStack *);
void sLexPop(tStack *);
void *sTopPop(tStack *);
int sDelete(tStack *);
int sDispose(tStack *);
bool sEmpty(tStack *);

#endif