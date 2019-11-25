#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexical_analysis.h"
#include "expression.h"
#include "error_codes.h"
#include "stack.h"

// Stack initialization
void sInit(tStack* Stack)
{
	/*tStack *Stack = (tStack *) malloc(sizeof(tStack));
    if(Stack == NULL)
    {
        return NULL;
    }*/
	
	Stack->topItem = NULL;
	Stack->size = 0;

	//return stack;
}

int sPush(tStack *Stack, Precedence_table_symbol newSymbol, Type type)
{
	if(Stack == NULL)
	{
		return INTERNAL_ERROR;
	}

	tStackItem *NewStackItem = (tStackItem*) malloc(sizeof(struct StackItem));
	if (NewStackItem == NULL)
	{
		return INTERNAL_ERROR;
	}

	NewStackItem->stackSymbol = newSymbol;
	//NewStackItem->data = data;
	NewStackItem->nextItem = Stack->topItem;
	NewStackItem->typ = type;
	Stack->topItem = NewStackItem;
	Stack->size++;

	return OKAY;
}

void *sTop(tStack *Stack)
{
	if(Stack == NULL || Stack->topItem == NULL)
	{
		return NULL;
	}

	return Stack->topItem->data;
}

void *sTopPop(tStack *Stack)
{
	if(Stack == NULL || Stack->topItem == NULL)
	{
		return NULL;
	}

	tStackItem* pom = Stack->topItem;
	Stack->topItem = Stack->topItem->nextItem;
	
	void* back_data = pom->data;

	free(pom);	
	Stack->size--;

	return back_data;


}

tStackItem* TopTerminal(tStack *Stack)
{
    tStackItem tmp = Stack->topItem;
    while(tmp != NULL)
    {
        if (tmp->stackSymbol < S_SHIFT)
        {
            return tmp;
        }
        else
        {
            tmp = tmp->nextItem;
        }
    return NULL;
    }
}

bool push_TopTerminal(tStack* Stack, Precedence_table_symbol symbol, Type type)
{
    tStackItem* prevItem = NULL;
    tStackItem* pom = Stack->topItem;
    while(pom != NULL)
    {
        if(pom->stackSymbol < SYMBOL_SHIFT)
        {
            tStackItem* newItem = (tStackItem*) malloc(sizeof(tStackItem));
            if(newItem == NULL)
            {
                return false;
            }
            newItem->stackSymbol = symbol;
            newItem->typ = type;

            if (prevItem == NULL)
            {
                newItem->nextItem = Stack->topItem;
                Stack->topItem = newItem;
            }
            else
            {
                newItem->nextItem = prevItem->nextItem;
                prevItem->nextItem = newItem;
            }

            return true;
        }
        
        prevItem = pom;
        pom = pom->nextItem;
    }

    return false;
}

int sDelete(tStack *Stack)
{
	if(Stack == NULL)
	{
		return INTERNAL_ERROR;
	}

	free(Stack);
	return OKAY;
}

int sDispose(tStack *Stack)
{
	if(Stack == NULL)
	{
		return INTERNAL_ERROR;
	}

	tStackItem* pom;

	while(Stack->topItem != NULL)
	{
		pom = Stack->topItem;
		Stack->topItem = Stack->topItem->nextItem;
		//free(pom->data);
		free(pom);
	}

	Stack->size = 0;

	return OKAY;
	/*while( sTopPop(Stack) )
	{

	}*/
}

bool sEmpty(tStack *Stack)
{
	if(Stack == NULL)
	{
		return true;
	}
	
	return (Stack->topItem == NULL) ? true : false;
}