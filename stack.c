/* IFJ project 2019/2020: IFJcode19
* stack.c
* Author = Vojtech Mimochodek (xmimoc01)
*/
#include "lexical_analysis.h"
#include "expression.h"
#include "stack.h"
#include "generator.h"
#include "error_codes.h"

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

int sPush(tStack *Stack, int precedenceTableSymbol, int intdata)
{
	if(Stack == NULL)
	{
		return INTERNAL_ERROR;
	}

	tStackItem *NewStackItem = (tStackItem*) malloc(sizeof(struct StackItem) + sizeof(int));
	if (NewStackItem == NULL)
	{
		return INTERNAL_ERROR;
	}
	NewStackItem->intdata = intdata;
	NewStackItem->stackSymbol = precedenceTableSymbol;
	NewStackItem->nextItem = Stack->topItem;
	Stack->topItem = NewStackItem;
	Stack->size++;

	return IT_IS_OKAY;
}

void *sTop(tStack *Stack)
{
	if(Stack == NULL || Stack->topItem == NULL)
	{
		return NULL;
	}

	return Stack->topItem;
}

int sLexTop(tStack *Stack)
{
	if(Stack == NULL || Stack->topItem == NULL)
	{
		return INTERNAL_ERROR;
	}

	return Stack->topItem->intdata;
}

void sLexPop(tStack *Stack)
{
	

	tStackItem* pom = Stack->topItem;
	Stack->topItem = Stack->topItem->nextItem;
	

	free(pom);	
	Stack->size--;

	

}

void *sTopPop(tStack *Stack)
{
	if(Stack == NULL || Stack->topItem == NULL)
	{
		return NULL;
	}

	tStackItem* pom = Stack->topItem;
	Stack->topItem = Stack->topItem->nextItem;
	
	int* back_data = &(pom->stackSymbol);

	free(pom);	
	Stack->size--;

	return back_data;


}

int sDelete(tStack *Stack)
{
	if(Stack == NULL)
	{
		return INTERNAL_ERROR;
	}

	free(Stack);
	return IT_IS_OKAY;
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

	return IT_IS_OKAY;
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
tStackItem* TopTerminal(tStack *Stack)
{
    tStackItem *tmp = Stack->topItem;
    while(tmp != NULL)
    {
        if (tmp->stackSymbol < SYMBOL_SHIFT)
        {
            return tmp;
        }
        else
        {
            tmp = tmp->nextItem;
        }
    }
    return NULL;
}

bool push_TopTerminal(tStack* Stack, int symbol, int type)
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