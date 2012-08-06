#include <stdlib.h>
#include <stdbool.h>

#include "./ReplacementAlgorithms.h"
#include "./DataStructures.h"
#include "./ParticionesDinamicas.h"

extern MemoryStructures Cache;

void FIFO()
{
	DataDecriptor* item = GetAggregatedItemFromMemoryTable(Cache.MemoryTable, &WhereItemIsTheOlder);
	Delete(item);
}

void LRU()
{
	DataDecriptor* item = GetAggregatedItemFromMemoryTable(Cache.MemoryTable, &ItemLessUsed);
	Delete(item);
}

static void* ItemLessUsed(void* anItem, void* anotherItem)
{
	DataDecriptor* A = anItem;
	DataDecriptor* B = anotherItem;

	if (A == NULL)
		return B;
	if (A->Data == NULL)
		return B;

	if (B == NULL)
		return A;
	if (B-> Data == NULL)
		return A;

	if (A->Useage > B->Useage)
		return A;
	else
		return B;
}

static void* WhereItemIsTheOlder(void* anItem, void* anotherItem)
{
	DataDecriptor* A = anItem;
	DataDecriptor* B = anotherItem;

	if (A == NULL)
		return B;
	if (A->Data == NULL)
		return B;

	if (B == NULL)
		return A;
	if (B-> Data == NULL)
		return A;

	if (A->Age > B->Age)
		return A;
	else
		return B;
}
