#define MAX_KEY_SIZE 256
#define KEY_SIZE 42

typedef struct {
   void* 		key;
   size_t 		keySize;
   void*		data;
   size_t 		dataSize;
   int 			flags;
   bool 		isStored;
   rel_time_t 	expirationTime;
}DataItem;

typedef struct {
   size_t cache_max_size;
   size_t block_size_max;
   size_t chunk_size;
}EngineConfiguration;

typedef struct {
	ENGINE_HANDLE_V1 	engine;
	GET_SERVER_API 		get_server_api;
	EngineConfiguration config;
}EngineHandle;

MEMCACHED_PUBLIC_API ENGINE_ERROR_CODE create_instance(uint64_t interface, GET_SERVER_API get_server_api, ENGINE_HANDLE **handle);

static void CacheDestroy(ENGINE_HANDLE*, const bool force);
static void CacheItemRelease(ENGINE_HANDLE* , const void *cookie, item* item);
static bool CacheGetItemInfo(ENGINE_HANDLE *, const void *cookie, const item* item, item_info *item_info);
static ENGINE_ERROR_CODE CacheInitialize(ENGINE_HANDLE* , const char* config_str);
static ENGINE_ERROR_CODE CacheAllocate(ENGINE_HANDLE* , const void* cookie, item **item, const void* key, const size_t nkey, const size_t nbytes, const int flags, const rel_time_t exptime);
static ENGINE_ERROR_CODE CacheStore(ENGINE_HANDLE* , const void *cookie, item* item, uint64_t *cas, ENGINE_STORE_OPERATION operation, uint16_t vbucket);
static ENGINE_ERROR_CODE CacheGet(ENGINE_HANDLE* , const void* cookie, item** item, const void* key, const int nkey, uint16_t vbucket);
static ENGINE_ERROR_CODE CacheFlush(ENGINE_HANDLE* , const void* cookie, time_t when);
static ENGINE_ERROR_CODE CacheItemDelete(ENGINE_HANDLE* , const void* cookie, const void* key, const size_t nkey, uint64_t cas, uint16_t vbucket);

static void CacheDummp(int signal);
static void CacheResetStats(ENGINE_HANDLE* , const void *cookie);
static void CacheItemSetCas(ENGINE_HANDLE *, const void *cookie, item* item, uint64_t val);
static const engine_info* CacheGetInfo(ENGINE_HANDLE* );
static ENGINE_ERROR_CODE CacheGetStats(ENGINE_HANDLE* , const void* cookie, const char* stat_key, int nkey, ADD_STAT add_stat);
static ENGINE_ERROR_CODE CacheUnknownCommand(ENGINE_HANDLE* , const void* cookie, protocol_binary_request_header *request, ADD_RESPONSE response);

static void ConfigureCache(const char* configurationString, EngineHandle* engine);
static void InitializeMemorySpace(EngineHandle* engine);
static void InitializeMemoryTable(EngineHandle* engine);
static void InitializeDataRegion(EngineHandle* engine);
static void ReleaseBitArray();
static void	ReleaseMemoryTable();
static void	ReleaseDataRegion();
static void InitializeBitArray(EngineHandle* engine);
static void ConfigureAllocationAlgorithm(int algorithmNumber);
static void ConfigureReplacementAlgorithm(int algorithmNumber);
static double ToHashKey(const char* key, size_t keySize);
