#include "stack.h"
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

int sPush(tStack *Stack, void *data, int intdata)
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
	NewStackItem->data = data;
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
	
	void* back_data = pom->data;

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