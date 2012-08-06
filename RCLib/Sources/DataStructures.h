
#include "commons/bitarray.h"
#define MAX_KEY 42

typedef struct {
	int		Size;
	int		ItemSize;
	void*	Data;
}GenericArray;

typedef struct{
	double 	HashKey;
	int		DataSize;
	int 	DataFSSize;
	void*	Data;
	bool 	IsStored;
	short	Age;
	short	Useage;
	char 	RealKey[MAX_KEY];
}DataDecriptor;

typedef struct{
	int MinAllocationSize;
	GenericArray DataRegion;
	GenericArray MemoryTable;
	t_bitarray BitArray;
	char AlgorithmSelected[2];
	void (*ReplacementAlgorithm)();
	int (*AllocationAlgorithm)(size_t blockSize);
}MemoryStructures;

enum algorithms {
	BS_FF=1,
	BS_BF=2,
	PD_FF=3,
	PD_BF=4,
	BS_OLD=5,
	BS_LU=6,
	PD_OLD=7,
	PD_LU=8
};


//-------------- FUNCIONES PARA EL BITARRAY -------------------------------
void SetIntoBitArray(t_bitarray bitArray, int from, int size, bool value);
//int BitArrayGetAt(void* bitArray, int index);
//void BitArraySetAt(void* bitArray, int index, bool value);
//int GetBit(char byte, int index);
//void SetBit(char* byte, int index, int value);
//void FlipBit(char* byte, int index);

//-------------- FUNCIONES PARA EL GENERICARRAY ---------------------------
void* GetItemFrom(GenericArray array, bool (*condition)(void*, void*), void* value);
void* GetItemFromMemoryTable(GenericArray array, bool (*condition)(void*, void*), void* value);
void* GetAggregatedItemFrom(GenericArray array, void* (*agregation)(void*, void*));
void ForEachItemIn(GenericArray array, void (*transformation)(void*));
void ForEachItemInMemoryTable(GenericArray array, void (*transformation)(void*));
//-------------- Funcion para uso correcto de BS
int GetBSBlockSize(int blockSize);
