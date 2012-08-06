#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "./mathExtensions.h"
#include "./DataStructures.h"
#include "./AllocationAlgorithms.h"

extern MemoryStructures Cache;

int FirstFit(size_t desiredBlockSize)
{
	int index = -1, candidateBlockSize = 0;
	int totalBits = Cache.BitArray.size * 8;

	for(int i = 0; i < totalBits; i++)
	{
		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
		{
			if (index < 0)
				index = i;
		}
		else
			index = -1;

		if (index >= 0)
			candidateBlockSize++;
		else
			candidateBlockSize = 0;

		if (candidateBlockSize >= desiredBlockSize)
			return index;
	}

	return -1;
}

int BestFit(size_t desiredBlockSize)
{
	int index = -1, candidateBlockSize = 0;
	int totalBits = Cache.BitArray.size * 8;
	int firstFit = -1;
	int auxBlockSize = 0;
	for(int i = 0; i < totalBits; i++)
	{
		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
		{
			if (index < 0)
				index = i;

		}
		else
			index = -1;

		if (index >= 0)
			candidateBlockSize++;
		else
			candidateBlockSize = 0;

		if(bitarray_test_bit(&Cache.BitArray, (i+1)) == 0)
			continue;

		if (candidateBlockSize >= desiredBlockSize){
			if(firstFit == -1){
				firstFit = index;
				auxBlockSize = candidateBlockSize;
			}else{
				if(candidateBlockSize < auxBlockSize){
					firstFit = index;
					auxBlockSize = candidateBlockSize;
				}
			}

		}

	}
	if(totalBits == candidateBlockSize || (candidateBlockSize >= desiredBlockSize && firstFit < 0))
		firstFit = index;
	return firstFit;
}

int FirstFitBS(size_t desiredBlockSize)
{
	int totalBits = Cache.BitArray.size * 8;
	int auxBlocksize = 0, cont = 0, maxExponent = 0;
	int cantidadParticiones = 0, indiceIni = 0, indiceFin = 0;
	bool test = false;
	//truco
	while(auxBlocksize<desiredBlockSize){
		auxBlocksize = (int)pow(2,cont);
		cont++;
	}
	desiredBlockSize = auxBlocksize;
	maxExponent = logBinario(totalBits);
	cantidadParticiones = potencia(2,maxExponent) / desiredBlockSize;
	for(int i = 0; i < cantidadParticiones; i++)
	{
		indiceIni = desiredBlockSize * i;
		indiceFin = indiceIni + desiredBlockSize;
		for(int e = indiceIni; e < indiceFin; e++)
		{
			if(bitarray_test_bit(&Cache.BitArray,e))
			{
				test = true;
				break;
			}

		}
		if(test)
		{
			test = false;
			continue;
		}else{
			return indiceIni;
		}
	}

//	for(int i = 0; i < totalBits; i++)
//	{
//		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
//		{
//			if (index < 0)
//				index = i;
//		}
//		else
//			index = -1;
//
//		if (index >= 0)
//			candidateBlockSize++;
//		else
//			candidateBlockSize = 0;
//
//		if (candidateBlockSize >= desiredBlockSize)
//			return index;
//	}

	return -1;
}

int BestFitBS(size_t desiredBlockSize)
{
//	int index = -1, candidateBlockSize = 0;
//	int BSDesiredBlock = 2;
//	int auxBlocksize = 0, cont = 0;
//	//truco
//	while(auxBlocksize<desiredBlockSize){
//		auxBlocksize = (int)pow(BSDesiredBlock,cont);
//		cont++;
//	}
//
//	desiredBlockSize = auxBlocksize;
//
//	int totalBits = Cache.BitArray.size * 8;
//	int firstFit = -1;
//	int auxBlockSize = 0;
//	for(int i = 0; i < totalBits; i++)
//	{
//		if(bitarray_test_bit(&Cache.BitArray, i) == 0)
//		{
//			if (index < 0)
//				index = i;
//		}
//		else
//			index = -1;
//
//		if (index >= 0)
//			candidateBlockSize++;
//		else
//			candidateBlockSize = 0;
//
//		if(bitarray_test_bit(&Cache.BitArray, (i+1)) == 0)
//			continue;
//
//		if (candidateBlockSize >= desiredBlockSize){
//			if(firstFit == -1){
//				firstFit = index;
//				auxBlockSize = candidateBlockSize;
//			}else{
//				if(candidateBlockSize < auxBlockSize){
//					firstFit = index;
//					auxBlockSize = candidateBlockSize;
//				}
//			}
//
//		}
//
//	}
//
//	return firstFit;

	int totalBits = Cache.BitArray.size * 8;
	int auxBlocksize = 0, cont = 0, maxExponent = 0;
	int cantidadParticiones = 0, indiceIni = 0, indiceFin = 0;
	bool test = false;
	//truco
	while(auxBlocksize<desiredBlockSize){
		auxBlocksize = (int)pow(2,cont);
		cont++;
	}
	desiredBlockSize = auxBlocksize;
	maxExponent = logBinario(totalBits);
	cantidadParticiones = potencia(2,maxExponent) / desiredBlockSize;
	for(int i = 0; i < cantidadParticiones; i++)
	{
		indiceIni = desiredBlockSize * i;
		indiceFin = indiceIni + desiredBlockSize;
		for(int e = indiceIni; e < indiceFin; e++)
		{
			if(bitarray_test_bit(&Cache.BitArray,e))
			{
				test = true;
				break;
			}

		}
		if(test)
		{
			test = false;
			continue;
		}else{
			return indiceIni;
		}
	}


	return -1;

}

