#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "./DataStructures.h"
#include <semaphore.h>
extern sem_t *readSemBitArray;
extern sem_t *writeSemBitArray;
//---------------  	Manejo del bitArray --------------------
void SetIntoBitArray(t_bitarray bitArray, int from, int size, bool value)
{
	int to = from + size;
	sem_wait(readSemBitArray);
	sem_wait(writeSemBitArray);
	for(int i = from; i < to; i++)
		if(value)
			bitarray_set_bit(&bitArray, i);
		else
			bitarray_clean_bit(&bitArray, i);
	sem_post(readSemBitArray);
	sem_post(writeSemBitArray);

}

//---------------  Manejo de GenericArray ------------------
void* GetItemFrom(GenericArray array, bool (*condition)(void*, void*), void* value)
{
	void* i = array.Data;

	while (i < array.Data + array.Size)
	{
		if (condition(i, value))
			return i;

		i += array.ItemSize;
	}

	return NULL;
}
void* GetItemFromMemoryTable(GenericArray array, bool (*condition)(void*, void*), void* value)
{
	void* data = array.Data;

	for(int i = 0; i < array.Size; i++)
	{
		if (condition(data, value))
			return data;
		data += array.ItemSize;
		//i += array.ItemSize;
	}

	return NULL;
}
void* GetAggregatedItemFromMemoryTable(GenericArray array, void* (*agregation)(void*, void*))
{
	void* i = array.Data;
	void* result = i;

	while (i < array.Data + array.Size * array.ItemSize)
	{
		result = agregation(i, result);

		i += array.ItemSize;
	}

	return result;
}
void* GetAggregatedItemFrom(GenericArray array, void* (*agregation)(void*, void*))
{
	void* i = array.Data;
	void* result = i;

	while (i < array.Data + array.Size)
	{
		result = agregation(i, result);

		i += array.ItemSize;
	}

	return result;
}
void ForEachItemIn(GenericArray array, void (*transformation)(void*))
{
	void* i = array.Data;

	while (i < array.Data + array.Size)
	{
		transformation(i);
		i += array.ItemSize;
	}
}

void ForEachItemInMemoryTable(GenericArray array, void (*transformation)(void*))
{
	void* i = array.Data;

	while (i < array.Data + array.Size * array.ItemSize)
	{
		transformation(i);
		i += array.ItemSize;
	}
}

int GetBSBlockSize(int blockSize)
{
	int auxBlocksize = 0, cont = 0;

	while(auxBlocksize<blockSize){
		auxBlocksize = (int)pow(2,cont);
		cont++;
	}
	return auxBlocksize;
}
