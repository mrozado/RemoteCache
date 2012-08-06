#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <math.h>

#include "commons/bitarray.h"
#include "./DataStructures.h"
#include "./ParticionesDinamicas.h"

extern MemoryStructures Cache;
extern sem_t *readSem;
extern sem_t *writeSem;
extern sem_t *readSemBitArray;
extern sem_t *writeSemBitArray;

DataDecriptor* Allocate(size_t blockSize)
{
	int auxSize = 0, cont = 0;
	if (blockSize > Cache.DataRegion.Size)
		return NULL;


	int AllocationIndex = SavingMemoryAndGetIndex(blockSize);


	if (AllocationIndex < 0)
		return NULL;
	while(auxSize<blockSize){
		auxSize = (int)pow(2,cont);
		cont++;
	}
	sem_wait(readSem);
	sem_wait(writeSem);
	DataDecriptor* emptyDecriptor = GetEmptyDataDecriptor();
	emptyDecriptor->Data = Cache.DataRegion.Data + AllocationIndex;
	emptyDecriptor->DataSize = blockSize;
	emptyDecriptor->DataFSSize = auxSize;
	DataDecriptorsAging();
	ForEachItemInMemoryTable(Cache.MemoryTable, &IncrementUseageByOne);
	sem_post(readSem);
	sem_post(writeSem);

	return emptyDecriptor;
}
DataDecriptor* Get(double HashKey)
{
	return IncrementUseageAndReturnElement(GetItemFromMemoryTable(Cache.MemoryTable, &WhereHashKeyIs, &HashKey));
}

DataDecriptor* DeleteGet(double HashKey)
{
	return GetItemFromMemoryTable(Cache.MemoryTable, &WhereHashKeyIs, &HashKey);
}



void Delete(DataDecriptor* item)
{
	int bitArrayIndex = item->Data - Cache.DataRegion.Data;


	if(!strcmp(Cache.AlgorithmSelected,"bs")){
		SetIntoBitArray(Cache.BitArray, bitArrayIndex, GetBSBlockSize(item->DataSize), false);
	}else{
		SetIntoBitArray(Cache.BitArray, bitArrayIndex, item->DataSize, false);
	}
	sem_wait(readSem);
	sem_wait(writeSem);
	memset(item, 0, sizeof(DataDecriptor));
	sem_post(readSem);
	sem_post(writeSem);
}
void Flush()
{

	sem_wait(readSem);
	sem_wait(writeSem);
	memset(Cache.MemoryTable.Data, 0, sizeof(DataDecriptor) * Cache.MemoryTable.Size);
	memset(Cache.DataRegion.Data, 0, Cache.DataRegion.Size);
	memset(Cache.BitArray.bitarray, 0, Cache.BitArray.size);
	sem_post(readSem);
	sem_post(writeSem);
}

static int SavingMemoryAndGetIndex(size_t blockSize)
{

	int index = -1;
	sem_wait(writeSemBitArray);
	if ((index = GetEmptyBlock(blockSize)) < 0)
	{
		if (GetCacheFreeSpace() >= blockSize)
		{
			sem_post(writeSemBitArray);
			CompactMemorySpace();

			return SavingMemoryAndGetIndex(blockSize);
		}
		else
		{
			sem_post(writeSemBitArray);
			RemoveNextPartition();

			return SavingMemoryAndGetIndex(blockSize);
		}
	}

	if(!strcmp(Cache.AlgorithmSelected,"bs"))
	{
		blockSize = GetBSBlockSize(blockSize);
	}
	sem_post(writeSemBitArray);
	SetIntoBitArray(Cache.BitArray, index, blockSize, true);

	return index;
}
static int GetEmptyBlock(size_t desiredBlockSize)
{
	return Cache.AllocationAlgorithm(desiredBlockSize);
}
static void RemoveNextPartition()
{
	Cache.ReplacementAlgorithm();
}
static void CompactMemorySpace()
{
	sem_wait(writeSem);
	sem_wait(readSem);
	int totalBits = Cache.BitArray.size * 8;

	for(int i = 0; i < totalBits; i++)
	{
		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
		{
			int nextBlockIndex = FindNextBlockIndex(i);
			if (nextBlockIndex < 0)
				break; //Si devuelve -1 significa que llego al final del array y no encontro ningun otro bloque de datos; o sea, la memoria ya esta compactada

			void* OriginalPosition = Cache.DataRegion.Data + nextBlockIndex;
			void* NewPosition = Cache.DataRegion.Data + i;

			DataDecriptor* BlockToMove = GetItemFromMemoryTable(Cache.MemoryTable, &WhereDataIs, OriginalPosition);

			memmove(NewPosition, OriginalPosition, BlockToMove->DataSize);

			SetIntoBitArray(Cache.BitArray, nextBlockIndex,(!strcmp(Cache.AlgorithmSelected,"bs"))?GetBSBlockSize(BlockToMove->DataSize):BlockToMove->DataSize, false);
			SetIntoBitArray(Cache.BitArray, i, (!strcmp(Cache.AlgorithmSelected,"bs"))?GetBSBlockSize(BlockToMove->DataSize):BlockToMove->DataSize, true);

			BlockToMove->Data = NewPosition;

			i += BlockToMove->DataSize - 1; //Le resto uno, porque luego se va a incrementar nuevamente por el for
		}
	}
	sem_post(writeSem);
	sem_post(readSem);
}
static int FindNextBlockIndex(int startingIndex)
{
	int totalBits = Cache.BitArray.size * 8;

	for(int i = startingIndex; i < totalBits; i++)
	{
		if (bitarray_test_bit(&Cache.BitArray, i) == 1)
			return i;
	}

	return -1;
}
static DataDecriptor* GetEmptyDataDecriptor()
{
	return GetItemFromMemoryTable(Cache.MemoryTable, &WhereDataIs, NULL);
}
static void DataDecriptorsAging()
{
	ForEachItemIn(Cache.MemoryTable, &IncrementAgeByOne);
}
static int GetCacheFreeSpace()
{
	int totalSize = Cache.BitArray.size * 8;
	int freeSpace = totalSize;

	for(int i = 0; i < totalSize; i++)
		freeSpace -= bitarray_test_bit(&Cache.BitArray, i);

	return freeSpace;
}

static bool WhereHashKeyIs(void* item, void* value)
{
	DataDecriptor* anItem = item;
	double aKey = *(double*)value;

	return anItem->HashKey == aKey;
}
static bool WhereDataIs(void* item, void* aValue)
{
	return ((DataDecriptor*)item)->Data == aValue;
}
static void IncrementAgeByOne(void* item)
{
	DataDecriptor* i = item;
	if (i->Data != NULL)
		i->Age++;
}
static void IncrementUseageByOne(void* item)
{
	DataDecriptor* i = item;
	if (i->Data != NULL)
		i->Useage++;
}
DataDecriptor* IncrementUseageAndReturnElement(void* item)
{
	if(item == NULL)
		return NULL;
	ForEachItemInMemoryTable(Cache.MemoryTable, &IncrementUseageByOne);
	DataDecriptor* i = item;
	if (i->Data != NULL)
		i->Useage = 1;

	return i;
}

//int FindNextBitValue(int startingIndex, int bitValue)
//{
//	int totalBits = Cache.BitArray.size * 8;
//
//	for(int i = startingIndex; i < totalBits; i++)
//	{
//		if (bitarray_test_bit(Cache.BitArray.bitarray, i) == bitValue)
//			return i;
//	}
//
//	return -1;
//}
