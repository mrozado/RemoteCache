
#define REPLACEMENT_FIFO 0
#define REPLACEMENT_LRU 1
#define BS_REPLACEMENT_FIFO 2
#define BS_REPLACEMENT_LRU 3

void FIFO();
void LRU();

static void* WhereItemIsTheOlder(void* anItem, void* anotherItem);
static void* ItemLessUsed(void* anItem, void* anotherItem);
