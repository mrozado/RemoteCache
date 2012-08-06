
DataDecriptor* Allocate(size_t blockSize);
DataDecriptor* Get(double HashKey);
void Delete(DataDecriptor* item);
DataDecriptor* DeleteGet(double HashKey);
void Flush();

static void RemoveNextPartition();
static int SavingMemoryAndGetIndex(size_t blockSize);
static void CompactMemorySpace();
static int FindNextBlockIndex(int startingIndex);
static int GetEmptyBlock(size_t blockSize);
static DataDecriptor* GetEmptyDataDecriptor();
static void DataDecriptorsAging();
static int GetCacheFreeSpace();
static void IncrementUseageByOne(void* item);
DataDecriptor* IncrementUseageAndReturnElement(void* item);

//---------- Criterios de MemoryTable ------------
static bool WhereHashKeyIs(void* item, void* value);
static bool WhereDataIs(void* item, void* aValue);
static void IncrementAgeByOne(void* item);

int FindNextBitValue(int startingIndex, int bitValue);
