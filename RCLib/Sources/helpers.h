#ifndef HELPERS_H_
#define HELPERS_H_

void DumpBitArray(FILE* fileDecriptor);
//void DumpMemoryTable(FILE* fileDecriptor);
void DumpFreeBlocks(FILE* fileDecriptor);
double ToHashKey(const char* key,const size_t keySize);


#endif
