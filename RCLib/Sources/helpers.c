#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "commons/bitarray.h"
#include "./DataStructures.h"
#include <semaphore.h>

#define MAX_KEY_SIZE 256
#define KEY_SIZE 42
extern MemoryStructures Cache;
sem_t *readSem;
sem_t *writeSem;

//double ToHashKey(const char* key,const size_t keySize)
//{
//	char fullKey[MAX_KEY_SIZE];
//	double hashedKey = 0;
//	int* byteGroups = NULL;
//	int byteGroupsSize = MAX_KEY_SIZE / sizeof(int);
//
//	memset(fullKey, '\0', MAX_KEY_SIZE);
//	memcpy(fullKey, key, keySize);
//	byteGroups = (int *)fullKey;
//
//	for(int i = 0; i < byteGroupsSize; i++)
//		hashedKey = hashedKey + byteGroups[i];
//
//	return hashedKey;
//}
int ToHashKey(const char* key,const size_t keySize)
{
	char aux[42];
	memset(aux, 0, 42);
	memcpy(aux, key, keySize);
	int result = atoi(key);

	if (result != 0)
		return result;

	return ComputeHashKey(key, keySize);
}
int ComputeHashKey(const char* key,const size_t keySize)
{
	int tamanoKey = KEY_SIZE + 2;
	char fullKey[tamanoKey];
	int hashedKey = 0;
	int* byteGroups = NULL;
	int byteGroupsSize = tamanoKey / sizeof(int);

	memset(fullKey, 0, tamanoKey);
	memcpy(fullKey, key, keySize);
	byteGroups = (int *)fullKey;

	for(int i = 0; i < byteGroupsSize; i++)
		hashedKey = hashedKey + byteGroups[i];

	return hashedKey;
}



void DumpBitArray(FILE* fileDecriptor)
{
	fprintf(fileDecriptor, "BitMap:\n");

	for (int i = 0; i < Cache.BitArray.size * 8; i++)
	{
		sem_wait(readSem);
		sem_wait(writeSem);
		short bit = bitarray_test_bit(&Cache.BitArray, i);
		sem_post(readSem);
		sem_post(writeSem);
		fprintf(fileDecriptor, "%d ", bit);
	}

	fprintf(fileDecriptor, "\n\n");
}
void DumpMemoryTable(FILE* fileDecriptor)
{
	fprintf(fileDecriptor, "Particiones almacenadas:\n");
	for (int i = 0; i < Cache.MemoryTable.Size; i++)
	{
		DataDecriptor* item = Cache.MemoryTable.Data + i * Cache.MemoryTable.ItemSize;

		if (item != NULL && item->DataSize != 0)
		{
			void* DireccionInicial = item->Data;
			void* DireccionFinal = (!strcmp(Cache.AlgorithmSelected,"pd"))? (item->Data + item->DataSize) : (item->Data + item->DataFSSize);
			int Tamano = (!strcmp(Cache.AlgorithmSelected,"pd"))? item->DataSize : item->DataFSSize;
			int EspacioReservado = (Tamano < Cache.MinAllocationSize) ? Cache.MinAllocationSize : Tamano;
			char* Key = item->RealKey;

			fprintf(fileDecriptor, "[%p - %p] - Ocupada - Tamano valor: [%d] bytes - Espacio ocupado [%d] bytes - key: [%s]\n", DireccionInicial, DireccionFinal, Tamano, EspacioReservado, Key);
		}
	}
	fprintf(fileDecriptor, "\n\n");
}

int FindNextBitValue(int startingIndex, int bitValue)
{
	int totalBits = Cache.BitArray.size * 8;

	for(int i = startingIndex; i < totalBits; i++)
	{
		if (bitarray_test_bit(&Cache.BitArray, i) == bitValue)
			return i;
	}

	return -1;
}

void DumpFreeBlocks(FILE* fileDecriptor)
{
	fprintf(fileDecriptor, "Particiones Libres:\n");
	sem_wait(readSem);
	sem_wait(writeSem);
	for (int i = 0; i < Cache.BitArray.size * 8; i++)
	{
		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
		{
			int lastFreeByte = FindNextBitValue(i, 1) - 1;
			if (lastFreeByte < 0)
				lastFreeByte = Cache.BitArray.size * 8 - 1;

			void* DireccionInicial = Cache.DataRegion.Data + i;
			void* DireccionFinal = Cache.DataRegion.Data + lastFreeByte;
			int Tamano = lastFreeByte - i + 1;

			fprintf(fileDecriptor, "[%p - %p] - Libre - Size: [%d] bytes\n", DireccionInicial, DireccionFinal, Tamano);

			i += Tamano - 1; //Le resto uno, porque luego se va a incrementar nuevamente por el for
		}
	}
	sem_post(readSem);
	sem_post(writeSem);
}
