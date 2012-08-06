
#define ALLOCATION_FIRSTFIT 0
#define ALLOCATION_BESTFIT 1
#define ALLOCATION_BSFIRSTFIT 2
#define ALLOCATION_BSBESTFIT 3

int FirstFit(size_t desiredBlockSize);
int BestFit(size_t desiredBlockSize);

int FirstFitBS(size_t desiredBlockSize);
int BestFitBS(size_t desiredBlockSize);
